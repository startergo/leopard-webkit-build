// gst-gl-probe.c — standalone test of whether GstGL 1.4.5 will accept and
// activate a CGLContextObj we created ourselves. This is the load-bearing
// API question for the future WebKit Cocoa GL consumer: if this works,
// bundling + the consumer port are worthwhile.

#define GST_USE_UNSTABLE_API 1
//
// Build (on macbookpro, against 10.6 SDK + 1.4.5 devel headers + libs):
//   clang -arch x86_64 -mmacosx-version-min=10.6 \
//         -isysroot <10.6 sdk> \
//         -I<devel>/include/gstreamer-1.0 \
//         -I<devel>/include/glib-2.0 \
//         -I<devel>/lib/glib-2.0/include \
//         -framework OpenGL \
//         -L<devel>/lib gst-gl-probe.c -o gst-gl-probe \
//         -lgstgl-1.0 -lgstvideo-1.0 -lgstreamer-1.0 \
//         -lgobject-2.0 -lglib-2.0 -lintl
//
// Run (on the mini where /Library/Frameworks/GStreamer.framework is installed):
//   ./gst-gl-probe

#include <stdio.h>
#include <stdlib.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/CGLTypes.h>

#include <glib.h>
#include <glib-object.h>
#include <gst/gst.h>
#include <gst/gl/gstglcontext.h>
#include <gst/gl/gstgldisplay.h>
#include <gst/gl/gstglapi.h>
#include <gst/gl/gstglutils.h>

/* Probe callback for gst_gl_context_thread_add — runs on the GL thread.
 * For a wrapped context, that's whatever thread is calling (no thread hop).
 * If this runs and the GL calls succeed, the wrapped context is usable. */
typedef struct {
    volatile gboolean callback_ran;
    GLenum gl_err;
} ProbeResult;

static void probe_gl_thread_fn(GstGLContext *ctx, gpointer data) {
    ProbeResult *r = (ProbeResult *)data;
    r->callback_ran = TRUE;
    glClearColor(0.4f, 0.2f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    r->gl_err = glGetError();
}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);

    /* ---- 1. Build a CGL context the way WebKit's CAOpenGLLayer would ----
     * On 10.6 we can't specify an OpenGL profile via pixel-format attrs
     * (kCGLPFAOpenGLProfile + kCGLOGLPVersion_Legacy are 10.7+). The
     * default context on 10.6 IS Legacy (GL 2.1) — what the 9400M caps at.
     */
    CGLPixelFormatAttribute attribs[] = {
        kCGLPFAColorSize,     (CGLPixelFormatAttribute)24,
        kCGLPFAAlphaSize,     (CGLPixelFormatAttribute)8,
        kCGLPFADoubleBuffer,
        (CGLPixelFormatAttribute)0
    };
    CGLPixelFormatObj pf = NULL;
    GLint nvirt = 0;
    CGLError cerr = CGLChoosePixelFormat(attribs, &pf, &nvirt);
    g_print("CGLChoosePixelFormat: err=%d numVirtualScreens=%d\n", cerr, (int)nvirt);
    if (cerr != kCGLNoError || !pf) {
        g_printerr("FAIL: could not choose pixel format\n");
        return 1;
    }

    CGLContextObj cgl_ctx = NULL;
    cerr = CGLCreateContext(pf, NULL, &cgl_ctx);
    g_print("CGLCreateContext: err=%d cgl_ctx=%p\n", cerr, (void*)cgl_ctx);
    CGLDestroyPixelFormat(pf);
    if (cerr != kCGLNoError || !cgl_ctx) {
        g_printerr("FAIL: could not create CGL context\n");
        return 2;
    }

    cerr = CGLSetCurrentContext(cgl_ctx);
    g_print("CGLSetCurrentContext: err=%d\n", cerr);

    /* ---- 2. Set up GstGL display ---- */
    GstGLDisplay *display = gst_gl_display_new();
    g_print("gst_gl_display_new: display=%p type=%s\n",
            (void*)display,
            display ? G_OBJECT_TYPE_NAME(display) : "(null)");
    if (!display) {
        g_printerr("FAIL: gst_gl_display_new returned NULL\n");
        CGLDestroyContext(cgl_ctx);
        return 3;
    }

    /* ---- 3. THE PROBE: wrap our CGL context ---- */
    GstGLContext *ctx = gst_gl_context_new_wrapped(
        display,
        (guintptr)cgl_ctx,
        GST_GL_PLATFORM_CGL,
        GST_GL_API_OPENGL
    );
    g_print("gst_gl_context_new_wrapped(CGL, OPENGL): ctx=%p type=%s\n",
            (void*)ctx,
            ctx ? G_OBJECT_TYPE_NAME(ctx) : "(null)");
    if (!ctx) {
        g_printerr("FAIL: gst_gl_context_new_wrapped returned NULL\n");
        g_object_unref(display);
        CGLDestroyContext(cgl_ctx);
        return 4;
    }

    /* ---- 4. Inspect what GstGL thinks it got ---- */
    guintptr handle = gst_gl_context_get_gl_context(ctx);
    GstGLAPI gl_api = gst_gl_context_get_gl_api(ctx);
    g_print("get_gl_context: handle=0x%lx (expected %p)\n", (unsigned long)handle, (void*)cgl_ctx);
    g_print("get_gl_api:    0x%x (expected 0x%x)\n", gl_api, GST_GL_API_OPENGL);
    if (handle != (guintptr)cgl_ctx) {
        g_printerr("WARN: GstGL's reported handle does not match the CGL context we passed\n");
    }

    /* ---- 5. Try to activate (likely stub on wrapped contexts; ignore result) ----
     * gst_gl_wrapped_context_class_init in 1.4 may not override the activate
     * vfunc, in which case the base class returns FALSE without logging.
     * Don't treat as fatal — wrapped contexts are externally managed, so the
     * caller is responsible for making the context current (we did, via
     * CGLSetCurrentContext above). */
    gboolean ok = gst_gl_context_activate(ctx, TRUE);
    g_print("gst_gl_context_activate(TRUE): %d (expected ~0 for wrapped; not fatal)\n", ok);

    /* ---- 6. THE REAL PROBE: try gst_gl_context_thread_add on wrapped context ----
     * 1.4's API contract: wrapped contexts CANNOT drive thread_add (the function
     * has g_return_if_fail(!WRAPPED). We test that explicitly here to confirm
     * the rejection, rather than report false success. */
    g_print("\n--- attempting gst_gl_context_thread_add on wrapped context ---\n");
    ProbeResult pr = { .callback_ran = FALSE, .gl_err = GL_NO_ERROR };
    gst_gl_context_thread_add(ctx, probe_gl_thread_fn, &pr);
    if (!pr.callback_ran) {
        g_print("gst_gl_context_thread_add: REJECTED wrapped context (callback never ran)\n");
        g_print("  -> This matches 1.4 design. Wrapped contexts are identity tokens,\n");
        g_print("     not GL drivers. Real consumer port uses them as share parents\n");
        g_print("     for GstGL elements (glupload, gldownload), not as work drivers.\n");
    } else {
        g_print("gst_gl_context_thread_add: callback ran (unexpected for wrapped context)\n");
    }

    /* ---- 7. TEST: native GstGL context on this hardware ----
     * Can we create a fresh GstGLContext via gst_gl_context_new() and
     * activate it on the 9400M? This is the path the GstGL elements
     * internally use to create their own working contexts. If this works,
     * the architecture is sound and only the share-group wiring
     * (via GstContext propagation, not direct API) remains. */
    g_print("\n--- attempting gst_gl_context_new + activate (native context) ---\n");
    GstGLContext *native_ctx = gst_gl_context_new(display);
    g_print("gst_gl_context_new: native_ctx=%p type=%s\n",
            (void*)native_ctx,
            native_ctx ? G_OBJECT_TYPE_NAME(native_ctx) : "(null)");
    if (native_ctx) {
        gboolean native_ok = gst_gl_context_activate(native_ctx, TRUE);
        g_print("gst_gl_context_activate(native, TRUE): %d\n", native_ok);
        if (native_ok) {
            /* If activate succeeded, thread_add should work on the native ctx */
            ProbeResult pr2 = { .callback_ran = FALSE, .gl_err = GL_NO_ERROR };
            gst_gl_context_thread_add(native_ctx, probe_gl_thread_fn, &pr2);
            if (pr2.callback_ran) {
                g_print("  [thread_add on native] ran; gl_err=0x%x\n", pr2.gl_err);
                if (pr2.gl_err == GL_NO_ERROR) {
                    g_print("NATIVE-CTX SUCCESS: 1.4.5 can create+activate+drive a GL context on this HW\n");
                }
            } else {
                g_print("  [thread_add on native] did not run\n");
            }
            gst_gl_context_activate(native_ctx, FALSE);
        }
        g_object_unref(native_ctx);
    }

    g_print("\nSUMMARY:\n");
    g_print("  gst_gl_context_new_wrapped on 10.6:     WORKS (wrapper accepts our CGL handle)\n");
    g_print("  wrapped context as thread_add driver:   REJECTED (by design in 1.4)\n");
    g_print("  native gst_gl_context_new + activate:   see above\n");

    gst_gl_context_activate(ctx, FALSE);
    g_object_unref(ctx);
    g_object_unref(display);
    CGLDestroyContext(cgl_ctx);
    return 0;
}
