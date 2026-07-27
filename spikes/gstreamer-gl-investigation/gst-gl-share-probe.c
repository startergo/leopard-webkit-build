// gst-gl-share-probe.c — the load-bearing test for the GstGL consumer port:
// can a GstGLContextCocoa (created by gst_gl_context_new) actually share GL
// objects (textures) with our externally-managed CGL context?
//
// Why this matters: gst_gl_context_new_wrapped + gst_gl_context_create both
// succeeding means CGL accepted the share group, which means pixel formats
// are compatible. If CGLCreateContext(pf, share, &ctx) inside GstGL returns
// kCGLBadMatch, share silently fails — texture IDs are valid but renders black.
// That's the failure mode worth catching now, not during the consumer port.
//
// Build:
//   clang -arch x86_64 -mmacosx-version-min=10.6 \
//         -isysroot <10.6sdk> \
//         -I<devel>/include/gstreamer-1.0 \
//         -I<devel>/include/glib-2.0 -I<devel>/lib/glib-2.0/include \
//         -Wno-deprecated-declarations -framework OpenGL \
//         -L<devel>/lib gst-gl-share-probe.c -o gst-gl-share-probe \
//         -lgstgl-1.0 -lgstvideo-1.0 -lgstreamer-1.0 \
//         -lgobject-2.0 -lglib-2.0 -lintl

#define GST_USE_UNSTABLE_API 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/CGLTypes.h>
#include <OpenGL/gl.h>

#include <glib.h>
#include <glib-object.h>
#include <gst/gst.h>
#include <gst/gl/gstglcontext.h>
#include <gst/gl/gstgldisplay.h>
#include <gst/gl/gstglapi.h>

/* Probe result bucket */
typedef struct {
    volatile gboolean callback_ran;
    GLenum gl_err;
    GLuint tex_id;
    gboolean tex_valid;
    GLubyte texel_value;
} ProbeResult;

/* Create a texture in the CURRENT GL context with a known texel value */
static GLuint create_known_texture(void) {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    /* 1x1 RGBA texture: R=0xAB, G=0xCD, B=0xEF, A=0x77 */
    GLubyte pixels[4] = { 0xAB, 0xCD, 0xEF, 0x77 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

/* Read back a texel from a texture ID in the CURRENT GL context via FBO */
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

/* thread_add callback: try to bind and read the texture created in the
 * WRAPPED context — if we can read the known texel, share works. */
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
    g_print("  [share probe] glBindTexture(0x%x) succeeded — texture ID is valid in native ctx\n",
            r->tex_id);

    GLubyte rgba[4] = {0};
    if (read_texel(r->tex_id, rgba)) {
        r->tex_valid = TRUE;
        r->texel_value = rgba[0];
        g_print("  [share probe] read back texel: R=0x%02x (expected 0xAB) G=0x%02x B=0x%02x A=0x%02x\n",
                rgba[0], rgba[1], rgba[2], rgba[3]);
    } else {
        g_print("  [share probe] FBO readback FAILED — texture exists but isn't readable\n");
    }
}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);

    /* ---- 1. Create CGL context A with WebKit-like (CAOpenGLLayer) attributes ---- */
    CGLPixelFormatAttribute attribs[] = {
        kCGLPFAAccelerated,
        kCGLPFANoRecovery,
        kCGLPFAColorSize,     (CGLPixelFormatAttribute)24,
        kCGLPFAAlphaSize,     (CGLPixelFormatAttribute)8,
        kCGLPFADoubleBuffer,
        (CGLPixelFormatAttribute)0
    };
    CGLPixelFormatObj pf = NULL;
    GLint nvirt = 0;
    CGLError cerr = CGLChoosePixelFormat(attribs, &pf, &nvirt);
    g_print("CGLChoosePixelFormat (WebKit-like attrs): err=%d numVirtualScreens=%d\n", cerr, (int)nvirt);
    if (cerr != kCGLNoError || !pf) { g_printerr("FAIL: pixel format\n"); return 1; }

    CGLContextObj cgl_a = NULL;
    cerr = CGLCreateContext(pf, NULL, &cgl_a);
    g_print("CGLCreateContext (cgl_a, no share): err=%d cgl_a=%p\n", cerr, (void*)cgl_a);
    CGLDestroyPixelFormat(pf);
    if (cerr != kCGLNoError) { g_printerr("FAIL: cgl_a create\n"); return 2; }

    CGLSetCurrentContext(cgl_a);

    /* ---- 2. GstGL setup ---- */
    GstGLDisplay *display = gst_gl_display_new();
    GstGLContext *wrapped = gst_gl_context_new_wrapped(display, (guintptr)cgl_a,
                                                        GST_GL_PLATFORM_CGL, GST_GL_API_OPENGL);
    g_print("wrapped: %p (type=%s, handle=0x%lx)\n",
            (void*)wrapped, G_OBJECT_TYPE_NAME(wrapped),
            (unsigned long)gst_gl_context_get_gl_context(wrapped));

    /* ---- 3. THE LOAD-BEARING CALL: can GstGL's native context share with ours? ---- */
    GstGLContext *native = gst_gl_context_new(display);
    g_print("native: %p (type=%s)\n", (void*)native, G_OBJECT_TYPE_NAME(native));

    gboolean can_share = gst_gl_context_can_share(native, wrapped);
    g_print("gst_gl_context_can_share(native, wrapped): %d\n", can_share);

    GError *create_err = NULL;
    gboolean created = gst_gl_context_create(native, wrapped, &create_err);
    if (!created) {
        g_printerr("FAIL: gst_gl_context_create(native, wrapped): %s\n",
                   create_err ? create_err->message : "(no error message)");
        if (create_err) g_error_free(create_err);
        g_printerr("  -> This is the share-group failure. Pixel formats likely incompatible.\n");
        g_printerr("     Next step: align GstGL's chosen pixel format with WebKit's via the\n");
        g_printerr("     GstGLDisplay selection or by intercepting the Cocoa context vfunc.\n");
        return 3;
    }
    g_print("gst_gl_context_create(native, wrapped): SUCCESS — share group established\n");

    gboolean native_active = gst_gl_context_activate(native, TRUE);
    g_print("gst_gl_context_activate(native, TRUE): %d\n", native_active);
    if (!native_active) {
        g_printerr("FAIL: native context won't activate (unexpected after successful create)\n");
        return 4;
    }

    /* ---- 4. Create known texture in OUR wrapped context (cgl_a) ---- */
    CGLSetCurrentContext(cgl_a);
    GLuint tex = create_known_texture();
    GLenum cerr2 = glGetError();
    g_print("\nCreated known texture in wrapped (cgl_a): tex_id=0x%x gl_err=0x%x\n",
            tex, cerr2);
    if (cerr2 != GL_NO_ERROR) { g_printerr("FAIL: texture creation failed\n"); return 5; }

    /* ---- 5. Via thread_add on NATIVE context, try to bind + read our texture ----
     * This is the actual share test: if cgl_a and native share objects, the texture
     * ID created in cgl_a is bindable in native, and the texel content is visible. */
    g_print("\n--- share probe: bind wrapped's texture in native via thread_add ---\n");
    ProbeResult pr = { .callback_ran = FALSE, .gl_err = GL_NO_ERROR,
                       .tex_id = tex, .tex_valid = FALSE, .texel_value = 0 };
    gst_gl_context_thread_add(native, probe_share_thread_fn, &pr);
    if (!pr.callback_ran) {
        g_printerr("FAIL: thread_add didn't run the callback\n");
        return 6;
    }

    g_print("\n=== FINAL VERDICT ===\n");
    if (pr.tex_valid && pr.texel_value == 0xAB) {
        g_print("SHARE-GROUP SUCCESS: texture created in wrapped (cgl_a) is bindable\n");
        g_print("and readable in native (GstGL). CGL share group is functional on 10.6\n");
        g_print("with WebKit-like + GstGL pixel formats. Consumer port is unblocked.\n");
        return 0;
    } else if (pr.tex_valid) {
        g_print("PARTIAL: texture bindable but content mismatch (read 0x%02x vs expected 0xAB)\n",
                pr.texel_value);
        g_print("Share group exists but is broken at content level — unusual, investigate.\n");
        return 7;
    } else {
        g_print("SHARE-GROUP FAILURE: texture ID valid but not readable from native context.\n");
        g_print("Share group was set up (create succeeded) but doesn't propagate textures.\n");
        return 8;
    }
}
