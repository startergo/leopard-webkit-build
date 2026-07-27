// gst-gl-share-probe2.c — same share-group test as v1 but uses NSOpenGLContext
// instead of raw CGLContextObj. The 1.4.5 GstGLContextCocoa backend at line 219
// of gstglcontext_cocoa.m casts the wrapped handle directly to NSOpenGLContext*,
// so passing a CGL handle was UB. With NSOpenGLContext, the share-group setup
// path (line 256: -[NSOpenGLContext initWithFormat:shareContext:]) gets a
// valid argument and either succeeds (share works) or returns nil (pixel
// format mismatch — the actual answer we want).
//
// Build:
//   clang -arch x86_64 -mmacosx-version-min=10.6 -isysroot <10.6sdk> \
//         -I<devel>/include/gstreamer-1.0 \
//         -I<devel>/include/glib-2.0 -I<devel>/lib/glib-2.0/include \
//         -Wno-deprecated-declarations -framework OpenGL -framework AppKit \
//         -L<devel>/lib gst-gl-share-probe2.c -o gst-gl-share-probe2 \
//         -lgstgl-1.0 -lgstvideo-1.0 -lgstreamer-1.0 \
//         -lgobject-2.0 -lglib-2.0 -lintl

#define GST_USE_UNSTABLE_API 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#import <Cocoa/Cocoa.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>

#include <glib.h>
#include <glib-object.h>
#include <gst/gst.h>
#include <gst/gl/gstglcontext.h>
#include <gst/gl/gstgldisplay.h>
#include <gst/gl/gstglapi.h>

/* Probe result */
typedef struct {
    volatile gboolean callback_ran;
    GLenum gl_err;
    GLuint tex_id;
    gboolean tex_valid;
    GLubyte texel_value;
} ProbeResult;

/* Create a 1x1 RGBA texture with known content (R=0xAB) in the CURRENT ctx */
static GLuint create_known_texture(void) {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLubyte pixels[4] = { 0xAB, 0xCD, 0xEF, 0x77 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

/* FBO readback of a 1x1 texture's texel */
static gboolean read_texel(GLuint tex_id, GLubyte *out_rgba) {
    GLuint fbo;
    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, tex_id, 0);
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDeleteFramebuffersEXT(1, &fbo);
        return FALSE;
    }
    glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, out_rgba);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &fbo);
    return TRUE;
}

/* thread_add callback on the NATIVE (non-wrapped) GstGL context:
 * tries to bind and read a texture created in our wrapped NSOpenGLContext */
static void probe_share_thread_fn(GstGLContext *ctx, gpointer data) {
    ProbeResult *r = (ProbeResult *)data;
    r->callback_ran = TRUE;

    glBindTexture(GL_TEXTURE_2D, r->tex_id);
    r->gl_err = glGetError();
    if (r->gl_err != GL_NO_ERROR) {
        g_print("  [share probe] glBindTexture(0x%x) FAILED with gl_err=0x%x\n",
                r->tex_id, r->gl_err);
        return;
    }
    g_print("  [share probe] glBindTexture(0x%x) OK — texture ID valid in native ctx\n",
            r->tex_id);

    GLubyte rgba[4] = {0};
    if (read_texel(r->tex_id, rgba)) {
        r->tex_valid = TRUE;
        r->texel_value = rgba[0];
        g_print("  [share probe] read back texel: R=0x%02x (expected 0xAB) G=0x%02x B=0x%02x A=0x%02x\n",
                rgba[0], rgba[1], rgba[2], rgba[3]);
    } else {
        g_print("  [share probe] FBO readback FAILED\n");
    }
}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    /* ---- 1. Create NSOpenGLContext A (WebKit-like) ----
     * Use attrs that are likely to be share-compatible with GstGL's choice
     * (DoubleBuffer + AccumSize 32). Add color+alpha sizes which are common. */
    NSOpenGLPixelFormatAttribute attribs[] = {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccumSize,    32,
        NSOpenGLPFAColorSize,    24,
        NSOpenGLPFAAlphaSize,    8,
        (NSOpenGLPixelFormatAttribute)0
    };
    NSOpenGLPixelFormat *fmt = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
    if (!fmt) { g_printerr("FAIL: NSOpenGLPixelFormat\n"); return 1; }

    NSOpenGLContext *our_ctx = [[NSOpenGLContext alloc] initWithFormat:fmt shareContext:nil];
    [fmt release];
    if (!our_ctx) { g_printerr("FAIL: NSOpenGLContext\n"); return 2; }

    /* Make current so any GL call binds to it */
    [our_ctx makeCurrentContext];
    g_print("Created our NSOpenGLContext: %p (CGL handle: %p)\n",
            our_ctx, [our_ctx CGLContextObj]);

    /* ---- 2. GstGL setup: wrap our NSOpenGLContext as the share parent ---- */
    GstGLDisplay *display = gst_gl_display_new();
    GstGLContext *wrapped = gst_gl_context_new_wrapped(
        display,
        (guintptr)our_ctx,           /* <-- NSOpenGLContext * this time, not CGLContextObj */
        GST_GL_PLATFORM_CGL,         /* enum value in 1.4 — Cocoa backend ignores and casts to NSOpenGLContext* */
        GST_GL_API_OPENGL
    );
    g_print("wrapped: %p type=%s handle=0x%lx\n",
            (void*)wrapped, G_OBJECT_TYPE_NAME(wrapped),
            (unsigned long)gst_gl_context_get_gl_context(wrapped));

    /* ---- 3. Create native GstGLContextCocoa and request share with our wrapped ---- */
    GstGLContext *native = gst_gl_context_new(display);
    g_print("native: %p type=%s\n", (void*)native, G_OBJECT_TYPE_NAME(native));

    GError *create_err = NULL;
    gboolean created = gst_gl_context_create(native, wrapped, &create_err);
    if (!created) {
        g_printerr("FAIL: gst_gl_context_create(native, wrapped): %s\n",
                   create_err ? create_err->message : "(no message)");
        g_printerr("  -> Most likely: NSOpenGLContext pixel format mismatch between\n");
        g_printerr("     ours and GstGL's (GstGL uses {DoubleBuffer, AccumSize=32}).\n");
        g_printerr("     Fix: align attrs or patch gstglcontext_cocoa.m to share with CGL.\n");
        if (create_err) g_error_free(create_err);
        return 3;
    }
    g_print("gst_gl_context_create(native, wrapped): SUCCESS — share group established\n");

    gboolean native_active = gst_gl_context_activate(native, TRUE);
    g_print("gst_gl_context_activate(native, TRUE): %d\n", native_active);
    if (!native_active) {
        g_printerr("FAIL: native won't activate (unexpected after successful create)\n");
        return 4;
    }

    /* ---- 4. Create known texture in OUR NSOpenGLContext ---- */
    [our_ctx makeCurrentContext];
    GLuint tex = create_known_texture();
    GLenum e = glGetError();
    g_print("\nCreated texture in our NSOpenGLContext: tex_id=0x%x gl_err=0x%x\n", tex, e);
    if (e != GL_NO_ERROR) { g_printerr("FAIL: texture creation failed\n"); return 5; }

    /* ---- 5. thread_add on native — try to bind + read our texture ---- */
    g_print("\n--- share probe via thread_add on native ---\n");
    ProbeResult pr = { .callback_ran = FALSE, .gl_err = GL_NO_ERROR,
                       .tex_id = tex, .tex_valid = FALSE, .texel_value = 0 };
    gst_gl_context_thread_add(native, probe_share_thread_fn, &pr);
    if (!pr.callback_ran) {
        g_printerr("FAIL: thread_add didn't run callback\n");
        return 6;
    }

    g_print("\n=== FINAL VERDICT ===\n");
    if (pr.tex_valid && pr.texel_value == 0xAB) {
        g_print("SHARE-GROUP SUCCESS: NSOpenGLContext wrap + gst_gl_context_create\n");
        g_print("established a working share group. Texture created in our NSOpenGLContext\n");
        g_print("is bindable + readable from GstGL's native context. Consumer port unblocked.\n");
    } else if (pr.tex_valid) {
        g_print("PARTIAL: texture bindable but content mismatch (0x%02x vs 0xAB)\n", pr.texel_value);
    } else {
        g_print("SHARE-GROUP FAILURE: texture ID valid but not readable in native ctx.\n");
    }

    [pool release];
    return 0;
}
