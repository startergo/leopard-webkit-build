// gst-gl-share-probe3.m — final form. Same share-group test as v2, but
// with proper NSApplication main-loop threading. The 1.4.5 Cocoa create_context
// path uses dispatch_sync(main_queue, ...) which deadlocks if main is blocked
// on a condvar. In a real Cocoa app (WebKit, Safari, gst-launch) main runs
// NSRunLoop / g_main_loop, the 200ms iteration timer fires, drains the queue,
// dispatch_sync completes. This probe mirrors that pattern: spawn the create
// call on a worker, run NSRunLoop on main until the worker signals.
//
// If this gets past gst_gl_context_create and into the actual texture-share
// test, we get the answer to the only question that matters: does
// -[NSOpenGLContext initWithFormat:shareContext:] actually establish a
// working share group between our NSOpenGLContext and GstGL's?

#define GST_USE_UNSTABLE_API 1

#import <Cocoa/Cocoa.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>

#include <glib.h>
#include <glib-object.h>
#include <gst/gst.h>
#include <gst/gl/gstglcontext.h>
#include <gst/gl/gstgldisplay.h>
#include <gst/gl/gstglapi.h>

typedef struct {
    volatile gboolean callback_ran;
    GLenum gl_err;
    GLuint tex_id;
    gboolean tex_valid;
    GLubyte texel_value;
} ProbeResult;

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

static void probe_share_thread_fn(GstGLContext *ctx, gpointer data) {
    ProbeResult *r = (ProbeResult *)data;
    r->callback_ran = TRUE;
    glBindTexture(GL_TEXTURE_2D, r->tex_id);
    r->gl_err = glGetError();
    if (r->gl_err != GL_NO_ERROR) {
        g_print("  [share probe] glBindTexture(0x%x) FAILED gl_err=0x%x\n", r->tex_id, r->gl_err);
        return;
    }
    g_print("  [share probe] glBindTexture(0x%x) OK\n", r->tex_id);
    GLubyte rgba[4] = {0};
    if (read_texel(r->tex_id, rgba)) {
        r->tex_valid = TRUE;
        r->texel_value = rgba[0];
        g_print("  [share probe] texel: R=0x%02x (expected 0xAB) G=0x%02x B=0x%02x A=0x%02x\n",
                rgba[0], rgba[1], rgba[2], rgba[3]);
    } else {
        g_print("  [share probe] FBO readback FAILED\n");
    }
}

/* Worker thread: calls gst_gl_context_create, then signals main to stop. */
typedef struct {
    GstGLContext *native;
    GstGLContext *wrapped;
    GError *err;
    gboolean created;
} CreateJob;

static gpointer create_worker(gpointer data) {
    CreateJob *job = (CreateJob *)data;
    job->created = gst_gl_context_create(job->native, job->wrapped, &job->err);
    /* Wake main run loop */
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSApp stop:nil];
    });
    return NULL;
}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    /* ---- 1. NSApplication setup — needed so dispatch_sync(main_queue, ...) in
     * ----    gstglcontext_cocoa.m:gst_gl_context_cocoa_create_context can land. */
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    /* ---- 2. Create our NSOpenGLContext A (WebKit-like attrs + GstGL-matching) ---- */
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
    if (!our_ctx) { g_printerr("FAIL: NSOpenGLContext A\n"); return 2; }
    [our_ctx makeCurrentContext];
    g_print("our NSOpenGLContext: %p (CGL=%p)\n", our_ctx, [our_ctx CGLContextObj]);

    /* ---- 3. GstGL setup ---- */
    GstGLDisplay *display = gst_gl_display_new();
    GstGLContext *wrapped = gst_gl_context_new_wrapped(
        display, (guintptr)our_ctx, GST_GL_PLATFORM_CGL, GST_GL_API_OPENGL);
    g_print("wrapped: %p handle=0x%lx\n", (void*)wrapped,
            (unsigned long)gst_gl_context_get_gl_context(wrapped));

    GstGLContext *native = gst_gl_context_new(display);
    g_print("native:  %p type=%s\n", (void*)native, G_OBJECT_TYPE_NAME(native));

    /* ---- 4. gst_gl_context_create on WORKER thread, main runs NSApp.run ----
     * The 1.4.5 Cocoa create_context calls dispatch_sync(main_queue, ...) which
     * needs main to be running an NSRunLoop. Spawn worker, stop main when done. */
    CreateJob job = { .native = native, .wrapped = wrapped, .err = NULL, .created = FALSE };
    g_thread_new("create-worker", create_worker, &job);

    g_print("main running NSApp.run — waiting for create worker\n");
    [NSApp run];
    g_print("create worker returned: created=%d err=%s\n",
            job.created, job.err ? job.err->message : "(none)");

    if (!job.created) {
        g_printerr("FAIL: gst_gl_context_create did not establish share group\n");
        if (job.err) {
            g_printerr("  Error: %s\n", job.err->message);
            g_error_free(job.err);
        }
        return 3;
    }
    g_print("gst_gl_context_create: SUCCESS — share group established\n");

    gboolean native_active = gst_gl_context_activate(native, TRUE);
    g_print("gst_gl_context_activate(native, TRUE): %d\n", native_active);
    if (!native_active) {
        g_printerr("FAIL: native won't activate after successful create\n");
        return 4;
    }

    /* ---- 5. Create known texture in OUR NSOpenGLContext ---- */
    [our_ctx makeCurrentContext];
    GLuint tex = create_known_texture();
    GLenum e = glGetError();
    g_print("\nCreated texture in our NSOpenGLContext: tex_id=0x%x gl_err=0x%x\n", tex, e);

    /* ---- 6. thread_add on native — bind + read our texture ---- */
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
        g_print("established a working share group. Texture created in our context\n");
        g_print("is bindable + readable from GstGL's native context. Consumer port\n");
        g_print("is unblocked at the GL level.\n");
    } else if (pr.tex_valid) {
        g_print("PARTIAL: bindable but content mismatch (0x%02x vs 0xAB)\n", pr.texel_value);
    } else {
        g_print("SHARE-GROUP FAILURE: texture ID not readable from native ctx.\n");
    }

    [pool release];
    return 0;
}
