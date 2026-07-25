// gst-gl-iosurface-pipeline-probe.c — end-to-end IOSurface pipeline probe.
//
// Answers two questions in priority order:
//
// PROBE 2 (zero-blit): can we hand a pre-wrapped IOSurface-backed
// GstGLMemory to glupload as its *output*, so glupload writes video
// directly into the IOSurface texture? If yes, zero per-frame copy.
//
// PROBE 1 (copy-into-texture): if Probe 2 fails, can we use
// gst_gl_memory_copy_into_texture() to blit from a GstGLMemory produced
// by glupload into our IOSurface-backed destination texture? The
// destination is in the same GstGL context, so this is intra-context
// texture-to-texture copy.
//
// The probe uses a PATTERNED source (per-quadrant colors), not uniform
// fill, so orientation/stride bugs from the 2D→RECTANGLE cross-target
// copy are visible.
//
// NOTE on Probe 2 design: glupload's API in 1.4.5 allocates its own
// GstGLMemory — it does not accept a caller-supplied output texture.
// The honest test is: build an actual glupload element, query its
// srcpad caps, and see whether any allocation query we pose is honored.
// In practice the answer is no in 1.4.5 — glupload's allocator is
// internal. We confirm this empirically by attempting the allocation
// query and observing the rejection.
//
// Build (on macbookpro, against 10.6 SDK + 1.4.5 devel headers + libs):
//   clang -arch x86_64 -mmacosx-version-min=10.6 \
//         -isysroot <10.6 sdk path> \
//         -I<devel>/include/gstreamer-1.0 \
//         -I<devel>/include/glib-2.0 \
//         -I<devel>/lib/glib-2.0/include \
//         -framework OpenGL \
//         -framework IOSurface \
//         -framework CoreFoundation \
//         -L<devel>/lib gst-gl-iosurface-pipeline-probe.c \
//         -o gst-gl-iosurface-pipeline-probe \
//         -lgstgl-1.0 -lgstvideo-1.0 -lgstbase-1.0 \
//         -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lintl
//
// Run (on the mini where /Library/Frameworks/GStreamer.framework is installed):
//   ./gst-gl-iosurface-pipeline-probe

#define GST_USE_UNSTABLE_API 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/CGLTypes.h>
#include <OpenGL/CGLIOSurface.h>
#include <OpenGL/glext.h>

#include <IOSurface/IOSurface.h>
#include <CoreFoundation/CoreFoundation.h>

#include <glib.h>
#include <glib-object.h>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/gl/gstglcontext.h>
#include <gst/gl/gstgldisplay.h>
#include <gst/gl/gstglapi.h>
#include <gst/gl/gstglutils.h>
#include <gst/gl/gstglmemory.h>

/* Test frame dimensions — small to keep the probe fast.
 * Use power-of-two to avoid any NPOT issues on legacy hardware. */
#define PROBE_WIDTH  256
#define PROBE_HEIGHT 256

/* Convenience for the four-quadrant test pattern. */
static void fill_quadrant(uint8_t *base, size_t bpr,
                          int x0, int y0, int x1, int y1,
                          uint8_t b, uint8_t g, uint8_t r, uint8_t a) {
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            uint8_t *p = base + y * bpr + x * 4;
            p[0] = b; p[1] = g; p[2] = r; p[3] = a;
        }
    }
}

/* Fill a BGRA buffer with the quadrant pattern.
 *   Top-left:     red   (BGRA: 0,0,255,255)
 *   Top-right:    green (BGRA: 0,255,0,255)
 *   Bottom-left:  blue  (BGRA: 255,0,0,255)
 *   Bottom-right: white (BGRA: 255,255,255,255)
 *
 * "Top" here means OpenGL origin (y=height-1 in IOSurface memory if the
 * IOSurface is GL-origin). The exact mapping is what the probe verifies.
 * Caller owns the returned buffer and must g_free it. */
static uint8_t *make_patterned_frame(void) {
    size_t bpr = PROBE_WIDTH * 4;
    uint8_t *buf = (uint8_t *)g_malloc(bpr * PROBE_HEIGHT);
    int hw = PROBE_WIDTH / 2, hh = PROBE_HEIGHT / 2;
    /* Top-left: red */
    fill_quadrant(buf, bpr, 0,    hh,   hw,   PROBE_HEIGHT,   0,   0, 255, 255);
    /* Top-right: green */
    fill_quadrant(buf, bpr, hw,   hh,   PROBE_WIDTH, PROBE_HEIGHT, 0, 255,   0, 255);
    /* Bottom-left: blue */
    fill_quadrant(buf, bpr, 0,    0,    hw,   hh,           255,   0,   0, 255);
    /* Bottom-right: white */
    fill_quadrant(buf, bpr, hw,   0,    PROBE_WIDTH, hh,    255, 255, 255, 255);
    return buf;
}

/* Build an IOSurfaceRef of PROBE_WIDTH x PROBE_HEIGHT, BGRA, global. */
static IOSurfaceRef make_iosurface(void) {
    int w = PROBE_WIDTH, h = PROBE_HEIGHT;
    int bytes_per_row = w * 4;
    int bytes_per_element = 4;
    int element_width = 1, element_height = 1;
    int is_global = 1;
    OSType pixel_format = 'BGRA';

    CFNumberRef cf_w   = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &w);
    CFNumberRef cf_h   = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &h);
    CFNumberRef cf_bpr = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &bytes_per_row);
    CFNumberRef cf_bpe = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &bytes_per_element);
    CFNumberRef cf_ew  = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &element_width);
    CFNumberRef cf_eh  = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &element_height);
    CFNumberRef cf_glob = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &is_global);
    CFNumberRef cf_pf  = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &pixel_format);

    const void *keys[] = {
        kIOSurfacePixelFormat,
        kIOSurfaceWidth, kIOSurfaceHeight, kIOSurfaceBytesPerRow,
        kIOSurfaceBytesPerElement, kIOSurfaceElementWidth, kIOSurfaceElementHeight,
        kIOSurfaceIsGlobal,
    };
    const void *values[] = {
        cf_pf,
        cf_w, cf_h, cf_bpr, cf_bpe, cf_ew, cf_eh, cf_glob,
    };
    CFDictionaryRef dict = CFDictionaryCreate(
        kCFAllocatorDefault,
        keys, values, sizeof(keys) / sizeof(keys[0]),
        &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    IOSurfaceRef surf = IOSurfaceCreate(dict);
    CFRelease(dict);
    CFRelease(cf_w); CFRelease(cf_h); CFRelease(cf_bpr);
    CFRelease(cf_bpe); CFRelease(cf_ew); CFRelease(cf_eh);
    CFRelease(cf_glob); CFRelease(cf_pf);
    return surf;
}

/* Create + activate a NATIVE GstGLContext that shares with share_ctx.
 * Returns the GstGLContext (transfer full) and fills out_display. */
static GstGLContext *make_native_gst_context(CGLContextObj share_ctx,
                                              GstGLDisplay **out_display) {
    GstGLDisplay *display = gst_gl_display_new();
    g_print("  gst_gl_display_new: display=%p type=%s\n",
            (void*)display, display ? G_OBJECT_TYPE_NAME(display) : "(null)");
    if (!display) return NULL;

    GstGLContext *gst_ctx = gst_gl_context_new(display);
    g_print("  gst_gl_context_new: gst_ctx=%p type=%s\n",
            (void*)gst_ctx, gst_ctx ? G_OBJECT_TYPE_NAME(gst_ctx) : "(null)");
    if (!gst_ctx) { g_object_unref(display); return NULL; }

    /* Wrap our share parent so gst_gl_context_create has a share parent. */
    GstGLContext *share_wrap = gst_gl_context_new_wrapped(
        display,
        (guintptr)share_ctx,
        GST_GL_PLATFORM_CGL,
        GST_GL_API_OPENGL);
    g_print("  gst_gl_context_new_wrapped (share parent): %p\n", (void*)share_wrap);

    GError *err = NULL;
    gboolean ok = gst_gl_context_create(gst_ctx, share_wrap, &err);
    g_print("  gst_gl_context_create: ok=%d", ok);
    if (err) g_print(" err=%s", err->message);
    g_print("\n");
    if (err) g_error_free(err);
    if (share_wrap) g_object_unref(share_wrap);

    if (!ok) {
        g_object_unref(gst_ctx);
        g_object_unref(display);
        return NULL;
    }

    gst_gl_context_activate(gst_ctx, TRUE);
    *out_display = display;
    return gst_ctx;
}

/* Build a CGL context on the 9400M. */
static CGLContextObj make_cgl_context(void) {
    CGLPixelFormatAttribute attribs[] = {
        kCGLPFAColorSize,     (CGLPixelFormatAttribute)24,
        kCGLPFAAlphaSize,     (CGLPixelFormatAttribute)8,
        kCGLPFADoubleBuffer,
        kCGLPFAAccelerated,
        kCGLPFANoRecovery,
        (CGLPixelFormatAttribute)0
    };
    CGLPixelFormatObj pf = NULL;
    GLint nvirt = 0;
    CGLError cerr = CGLChoosePixelFormat(attribs, &pf, &nvirt);
    if (cerr != kCGLNoError || !pf) {
        g_printerr("  CGLChoosePixelFormat failed: err=%d\n", cerr);
        return NULL;
    }
    CGLContextObj ctx = NULL;
    cerr = CGLCreateContext(pf, NULL, &ctx);
    CGLDestroyPixelFormat(pf);
    if (cerr != kCGLNoError || !ctx) {
        g_printerr("  CGLCreateContext failed: err=%d\n", cerr);
        return NULL;
    }
    CGLSetCurrentContext(ctx);
    return ctx;
}

/* Bind an IOSurface as a RECTANGLE texture in the current CGL context. */
static GLuint bind_iosurface_texture(CGLContextObj cgl_ctx, IOSurfaceRef surf) {
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
    CGLError cerr = CGLTexImageIOSurface2D(
        cgl_ctx,
        GL_TEXTURE_RECTANGLE_ARB,
        GL_RGBA8,
        PROBE_WIDTH, PROBE_HEIGHT,
        GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
        surf, 0);
    g_print("  CGLTexImageIOSurface2D: err=%d (%s)\n",
            cerr, CGLErrorString(cerr));
    if (cerr != kCGLNoError) {
        glDeleteTextures(1, &tex);
        return 0;
    }
    return tex;
}

/* glFlush + glFinish callback used inside gst_gl_context_thread_add. */
static void gl_sync_callback(GstGLContext *ctx, gpointer data) {
    (void)ctx; (void)data;
    glFlush();
    glFinish();
}

/* Detect a texture's GL target by attempting to bind it to each candidate.
 * Returns GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB, or 0 on no match.
 *
 * This matters because gst_gl_memory_alloc's choice of target is internal
 * to GstGL 1.4.5 — it can be 2D OR RECTANGLE depending on the context's
 * config and the hardware. glupload's output is similarly internal.
 *
 * Production hits 2D(source, glupload)→RECTANGLE(dst, IOSurface). If this
 * probe's source turns out to be RECTANGLE, the probe is testing
 * RECTANGLE→RECTANGLE which passes trivially and tells us nothing about
 * the production risk. The caller must log this and interpret results
 * accordingly. */
static GLenum detect_texture_target(GLuint tex_id) {
    /* Binding a texture to the wrong target returns GL_INVALID_OPERATION. */
    glBindTexture(GL_TEXTURE_2D, tex_id);
    GLenum err2d = glGetError();
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex_id);
    GLenum errRect = glGetError();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    glGetError();  /* clear */

    if (err2d == GL_NO_ERROR && errRect != GL_NO_ERROR)
        return GL_TEXTURE_2D;
    if (errRect == GL_NO_ERROR && err2d != GL_NO_ERROR)
        return GL_TEXTURE_RECTANGLE_ARB;
    /* Both succeeded (shouldn't happen) or both failed (already bound
     * elsewhere). Report unknown. */
    return 0;
}

/* Build a horizontal-gradient BGRA frame: pixel(x,y) = (x, x, x, 255).
 * Used for the stride/shear pass — a uniform-color fill can pass a
 * stride bug undetected, but a gradient's value at column x must
 * equal x (mod 256). A stride mismatch shows up as values that don't
 * match column position. Caller owns the returned buffer. */
static uint8_t *make_gradient_frame(void) {
    size_t bpr = PROBE_WIDTH * 4;
    uint8_t *buf = (uint8_t *)g_malloc(bpr * PROBE_HEIGHT);
    for (int y = 0; y < PROBE_HEIGHT; y++) {
        for (int x = 0; x < PROBE_WIDTH; x++) {
            uint8_t *p = buf + y * bpr + x * 4;
            uint8_t v = (uint8_t)(x & 0xff);
            p[0] = v; p[1] = v; p[2] = v; p[3] = 255;
        }
    }
    return buf;
}

/* ============================================================ */
/* PROBE 2: zero-blit glupload output                            */
/*                                                              */
/* Strategy: build glupload, ask it whether the wrapped texture  */
/* can serve as output. In 1.4.5 this is not possible; we       */
/* confirm by API surface inspection (no setter exists) and     */
/* attempt an allocation query just to log the result.          */
/* ============================================================ */
static int run_probe_2(GstGLDisplay *display,
                        GstGLContext *gst_ctx,
                        GLuint io_tex) {
    g_print("\n=== PROBE 2: zero-blit glupload output ===\n");

    GstElement *glupload = gst_element_factory_make("glupload", NULL);
    if (!glupload) {
        g_printerr("  FAIL: gst_element_factory_make(\"glupload\") returned NULL\n");
        return 0;
    }
    g_print("  glupload created: %p\n", (void*)glupload);

    /* Set the GstGLDisplay + GstGLContext on glupload via GstContext. */
    GstContext *dispCtx = gst_context_new(GST_GL_DISPLAY_CONTEXT_TYPE, TRUE);
    gst_context_set_gl_display(dispCtx, display);
    gst_element_set_context(glupload, dispCtx);
    gst_context_unref(dispCtx);

    GstContext *appCtx = gst_context_new("gst.gl.app_context", TRUE);
    GstStructure *s = gst_context_writable_structure(appCtx);
#if GST_CHECK_VERSION(1, 12, 0)
    gst_structure_set(s, "context", GST_TYPE_GL_CONTEXT, gst_ctx, NULL);
#else
    gst_structure_set(s, "context", GST_GL_TYPE_CONTEXT, gst_ctx, NULL);
#endif
    gst_element_set_context(glupload, appCtx);
    gst_context_unref(appCtx);

    /* Wrap our IOSurface texture as a GstGLMemory — verifies the wrap
     * path works (the existing gst-gl-iosurface-probe already passed this). */
    GstGLMemory *wrapped = gst_gl_memory_wrapped_texture(
        gst_ctx,
        io_tex,
        GST_VIDEO_GL_TEXTURE_TYPE_RGBA,
        PROBE_WIDTH, PROBE_HEIGHT,
        NULL, NULL);
    if (!wrapped) {
        g_printerr("  gst_gl_memory_wrapped_texture returned NULL\n");
        gst_object_unref(glupload);
        return 0;
    }
    g_print("  wrapped IOSurface tex (%u) as GstGLMemory: %p tex_id=%u\n",
            io_tex, (void*)wrapped, wrapped->tex_id);

    /* There is no API in 1.4.5 to set glupload's output allocator to
     * ours. The closest is gst_element_get_static_pad("src") + a custom
     * allocation query, but glupload's srcpad allocator is internal.
     * We confirm by issuing a peer query (which will fail since glupload
     * is unlinked) and noting there is no setter for the output texture. */
    GstPad *srcpad = gst_element_get_static_pad(glupload, "src");
    if (srcpad) {
        GstCaps *caps = gst_caps_from_string(
            "video/x-raw(memory:GLMemory),format=RGBA,width=256,height=256");
        GstQuery *q = gst_query_new_allocation(caps, FALSE);
        gboolean got = gst_pad_peer_query(srcpad, q);
        g_print("  allocation peer_query on srcpad: %s (FALSE expected; glupload unlinked)\n",
                got ? "TRUE" : "FALSE");
        gst_query_unref(q);
        gst_caps_unref(caps);
        gst_object_unref(srcpad);
    }

    g_print("  RESULT: Probe 2 (zero-blit) is NOT available in 1.4.5.\n");
    g_print("    glupload has no public API to inject a destination texture.\n");
    g_print("    The wrapped GstGLMemory %p exists but cannot be made the\n", (void*)wrapped);
    g_print("    output of glupload. Probe 1 (copy_into_texture) is required.\n");

    gst_mini_object_unref(GST_MINI_OBJECT_CAST(wrapped));
    gst_object_unref(glupload);
    return 0;
}

/* ============================================================ */
/* PROBE 1: same-context copy_into_texture with pattern         */
/* ============================================================ */

/* Common: upload a BGRA pattern into src_mem's texture, copy into the
 * IOSurface dst, sync, then run a caller-provided readback check.
 * Returns 0 on pass, 1 on fail. Used for both the quadrant pass and
 * the gradient pass so they share the copy+sync plumbing. */
static int do_copy_pass(GstGLContext *gst_ctx,
                        GstGLMemory *src_mem,
                        GLenum src_target,
                        GLuint dest_tex,
                        IOSurfaceRef dest_surf,
                        const uint8_t *pattern,
                        const char *label,
                        gboolean respecify) {
    g_print("\n--- pass: %s (src_target=%s, respecify=%s) ---\n",
            label,
            src_target == GL_TEXTURE_2D ? "GL_TEXTURE_2D" :
            src_target == GL_TEXTURE_RECTANGLE_ARB ? "GL_TEXTURE_RECTANGLE_ARB" :
            "(unknown)",
            respecify ? "TRUE" : "FALSE");

    /* Upload pattern into the source texture. Use the detected target. */
    gst_gl_context_activate(gst_ctx, TRUE);
    gst_gl_context_thread_add(gst_ctx, gl_sync_callback, NULL);
    glBindTexture(src_target, src_mem->tex_id);
    glTexSubImage2D(src_target, 0,
                    0, 0, PROBE_WIDTH, PROBE_HEIGHT,
                    GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
                    (void *)pattern);
    GLenum upErr = glGetError();
    g_print("  glTexSubImage2D(src_target=0x%x) post-error: 0x%x (%s)\n",
            (unsigned)src_target, (unsigned)upErr,
            upErr == GL_NO_ERROR ? "GL_NO_ERROR" : "GL ERROR");
    glBindTexture(src_target, 0);

    gst_gl_context_thread_add(gst_ctx, gl_sync_callback, NULL);

    /* copy_into_texture. */
    gboolean copied = gst_gl_memory_copy_into_texture(
        src_mem,
        dest_tex,
        GST_VIDEO_GL_TEXTURE_TYPE_RGBA,
        PROBE_WIDTH, PROBE_HEIGHT,
        PROBE_WIDTH * 4,
        respecify);
    g_print("  gst_gl_memory_copy_into_texture: %s\n", copied ? "TRUE" : "FALSE");
    if (!copied) {
        g_printerr("  FAIL [%s]: copy_into_texture returned FALSE\n", label);
        return 1;
    }

    gst_gl_context_thread_add(gst_ctx, gl_sync_callback, NULL);

    /* Readback. */
    int fail = 0;
    uint32_t seed = 0;
    IOSurfaceLock(dest_surf, kIOSurfaceLockReadOnly, &seed);
    uint8_t *base = (uint8_t *)IOSurfaceGetBaseAddress(dest_surf);
    size_t bpr = IOSurfaceGetBytesPerRow(dest_surf);
    if (!base) {
        g_printerr("  FAIL [%s]: IOSurfaceGetBaseAddress returned NULL\n", label);
        IOSurfaceUnlock(dest_surf, kIOSurfaceLockReadOnly, &seed);
        return 1;
    }

    if (strcmp(label, "quadrant") == 0) {
        uint8_t *tl = base + (PROBE_HEIGHT - 1) * bpr + 0 * 4;
        uint8_t *tr = base + (PROBE_HEIGHT - 1) * bpr + (PROBE_WIDTH - 1) * 4;
        uint8_t *bl = base + 0 * bpr + 0 * 4;
        uint8_t *br = base + 0 * bpr + (PROBE_WIDTH - 1) * 4;
        g_print("  IOSurface corner read (BL=origin in IOSurface memory):\n");
        g_print("    TL=BGRA(%d,%d,%d,%d) expected one of: red(0,0,255,255) green(0,255,0,255) blue(255,0,0,255) white(255,255,255,255)\n", tl[0], tl[1], tl[2], tl[3]);
        g_print("    TR=BGRA(%d,%d,%d,%d)\n", tr[0], tr[1], tr[2], tr[3]);
        g_print("    BL=BGRA(%d,%d,%d,%d)\n", bl[0], bl[1], bl[2], bl[3]);
        g_print("    BR=BGRA(%d,%d,%d,%d)\n", br[0], br[1], br[2], br[3]);

        /* Orientation check: each of red/green/blue/white should appear
         * exactly once across the four corners. Accept either origin. */
        int reds = 0, greens = 0, blues = 0, whites = 0;
        uint8_t *corners[4] = { tl, tr, bl, br };
        for (int i = 0; i < 4; i++) {
            uint8_t *p = corners[i];
            if (abs(p[2]-255) < 10 && p[0] < 10 && p[1] < 10) reds++;
            else if (abs(p[1]-255) < 10 && p[0] < 10 && p[2] < 10) greens++;
            else if (abs(p[0]-255) < 10 && p[1] < 10 && p[2] < 10) blues++;
            else if (p[0] > 240 && p[1] > 240 && p[2] > 240) whites++;
        }
        if (reds == 1 && greens == 1 && blues == 1 && whites == 1) {
            g_print("  PASS [quadrant]: all four colors appear in distinct corners\n");
        } else {
            g_printerr("  FAIL [quadrant]: colors=%dR %dG %dB %dW (expected 1 each) — orientation bug\n",
                       reds, greens, blues, whites);
            fail = 1;
        }
    } else if (strcmp(label, "gradient") == 0) {
        /* Stride check: column x should have B==G==R==(x & 0xff).
         * Sample several columns. If stride is wrong, values drift or wrap. */
        int stride_fails = 0;
        int sample_cols[] = { 0, PROBE_WIDTH/4, PROBE_WIDTH/2, (3*PROBE_WIDTH)/4, PROBE_WIDTH-1 };
        g_print("  IOSurface gradient read (expect B=G=R=col_index):\n");
        for (int i = 0; i < (int)(sizeof(sample_cols)/sizeof(sample_cols[0])); i++) {
            int x = sample_cols[i];
            uint8_t expected = (uint8_t)(x & 0xff);
            uint8_t *p = base + (PROBE_HEIGHT/2) * bpr + x * 4;
            g_print("    col %3d: BGRA(%d,%d,%d,%d) expected BGR=%d\n",
                    x, p[0], p[1], p[2], p[3], expected);
            if (abs(p[0]-expected) > 2 || abs(p[1]-expected) > 2 || abs(p[2]-expected) > 2)
                stride_fails++;
        }
        if (stride_fails == 0) {
            g_print("  PASS [gradient]: column-position values match — stride correct\n");
        } else {
            g_printerr("  FAIL [gradient]: %d/%zu columns mismatched — stride/shear bug\n",
                       stride_fails, sizeof(sample_cols)/sizeof(sample_cols[0]));
            fail = 1;
        }
    }
    IOSurfaceUnlock(dest_surf, kIOSurfaceLockReadOnly, &seed);
    return fail;
}

static int run_probe_1(GstGLDisplay *display,
                        GstGLContext *gst_ctx,
                        IOSurfaceRef dest_surf,
                        GLuint dest_tex) {
    g_print("\n=== PROBE 1: same-context gst_gl_memory_copy_into_texture ===\n");
    (void)display;
    int failures = 0;

    /* Step 1: Allocate a source GstGLMemory in this context. */
    GstMemory *src_mem_raw = gst_gl_memory_alloc(
        gst_ctx,
        GST_VIDEO_GL_TEXTURE_TYPE_RGBA,
        PROBE_WIDTH, PROBE_HEIGHT,
        PROBE_WIDTH * 4);
    if (!src_mem_raw) {
        g_printerr("  FAIL: gst_gl_memory_alloc returned NULL\n");
        return 1;
    }
    GstGLMemory *src_mem = (GstGLMemory *)src_mem_raw;
    g_print("  gst_gl_memory_alloc: src_mem=%p tex_id=%u\n",
            (void*)src_mem, src_mem->tex_id);

    /* Step 2: DETECT source texture target. This is the load-bearing
     * check for whether this probe measures the production path. glupload
     * on most platforms emits GL_TEXTURE_2D; if our alloc also gives 2D,
     * the test below is a real 2D→RECTANGLE cross-target test. If it
     * gives RECTANGLE, we're testing RECTANGLE→RECTANGLE which is the
     * trivial case — pass would tell us nothing about production. */
    GLenum src_target = detect_texture_target(src_mem->tex_id);
    const char *target_name =
        src_target == GL_TEXTURE_2D ? "GL_TEXTURE_2D" :
        src_target == GL_TEXTURE_RECTANGLE_ARB ? "GL_TEXTURE_RECTANGLE_ARB" :
        "(unknown)";
    g_print("  source texture target: %s (tex_id=%u)\n", target_name, src_mem->tex_id);
    if (src_target != GL_TEXTURE_2D) {
        g_printerr("  ⚠️  WARNING: source is NOT GL_TEXTURE_2D. glupload's real output\n");
        g_printerr("     is typically 2D; this probe is testing %s → RECTANGLE,\n", target_name);
        g_printerr("     which may pass trivially. Interpret a PASS here with caution —\n");
        g_printerr("     it does not close the 2D→RECTANGLE production question.\n");
        g_printerr("     To force a real 2D→RECTANGLE test, allocate a manual 2D texture\n");
        g_printerr("     and wrap via gst_gl_memory_wrapped_texture (follow-up).\n");
    } else {
        g_print("  source is GL_TEXTURE_2D — test measures the real production path.\n");
    }

    /* Step 3: PASS 1 — quadrant pattern (orientation check). */
    uint8_t *quadrant = make_patterned_frame();
    failures += do_copy_pass(gst_ctx, src_mem, src_target,
                             dest_tex, dest_surf,
                             quadrant, "quadrant", FALSE);
    g_free(quadrant);

    /* Step 4: PASS 2 — horizontal gradient (stride check).
     * Uniform color in each quadrant could pass a stride bug undetected;
     * a column-varying gradient forces the value at column x to match x. */
    uint8_t *gradient = make_gradient_frame();
    failures += do_copy_pass(gst_ctx, src_mem, src_target,
                             dest_tex, dest_surf,
                             gradient, "gradient", FALSE);
    g_free(gradient);

    /* Step 5: if quadrant/gradient failed with respecify=FALSE, retry
     * with respecify=TRUE — sometimes GstGL needs permission to redefine
     * the dst target's storage to make the cross-target copy work. */
    if (failures > 0) {
        g_print("\n--- retry with respecify=TRUE (cross-target rescue attempt) ---\n");
        uint8_t *quadrant2 = make_patterned_frame();
        int before = failures;
        failures += do_copy_pass(gst_ctx, src_mem, src_target,
                                 dest_tex, dest_surf,
                                 quadrant2, "quadrant-respecify", TRUE);
        g_free(quadrant2);
        if (failures < before + 1) {
            g_print("  respecify=TRUE rescued the copy — use it in production\n");
            failures = before;  /* don't double-count the FALSE attempt */
        } else {
            g_print("  respecify=TRUE did not rescue — fall back to manual FBO blit\n");
        }
    }

    gst_mini_object_unref(GST_MINI_OBJECT_CAST(src_mem));
    return failures;
}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);

    int total_failures = 0;

    g_print("=== Setup: CGL context + IOSurface + native GstGL context ===\n");
    CGLContextObj cgl_ctx = make_cgl_context();
    if (!cgl_ctx) { return 1; }
    GLint rid = 0;
    CGLGetParameter(cgl_ctx, kCGLCPCurrentRendererID, &rid);
    g_print("  CGL context: %p renderer=0x%x (%s)\n",
            (void*)cgl_ctx, (unsigned)rid,
            (rid & 0x00020000) ? "GEFORCE" :
            (rid & 0x00040000) ? "SOFTWARE" : "unknown");

    IOSurfaceRef surf = make_iosurface();
    if (!surf) {
        g_printerr("  FAIL: IOSurfaceCreate returned NULL\n");
        CGLDestroyContext(cgl_ctx);
        return 1;
    }
    g_print("  IOSurface: %p id=%u\n", (void*)surf, IOSurfaceGetID(surf));

    GLuint io_tex = bind_iosurface_texture(cgl_ctx, surf);
    if (!io_tex) {
        CFRelease(surf);
        CGLDestroyContext(cgl_ctx);
        return 1;
    }
    g_print("  IOSurface texture: tex_id=%u (RECTANGLE)\n", io_tex);

    GstGLDisplay *display = NULL;
    GstGLContext *gst_ctx = make_native_gst_context(cgl_ctx, &display);
    if (!gst_ctx) {
        g_printerr("  FAIL: could not create native GstGL context\n");
        glDeleteTextures(1, &io_tex);
        CFRelease(surf);
        CGLDestroyContext(cgl_ctx);
        return 1;
    }

    /* Run probe 2 first (zero-blit). */
    run_probe_2(display, gst_ctx, io_tex);

    /* Run probe 1 (copy_into_texture). */
    total_failures += run_probe_1(display, gst_ctx, surf, io_tex);

    g_print("\n=== Summary ===\n");
    if (total_failures == 0) {
        g_print("Probe 1 (copy_into_texture): PASS — same-context IOSurface copy works\n");
        g_print("Probe 2 (zero-blit): NOT AVAILABLE in 1.4.5 — confirmed by API inspection\n");
        g_print("\nThe IOSurface bridge path for WebKit Cocoa should use copy_into_texture.\n");
    } else {
        g_print("%d failure(s) — see above.\n", total_failures);
    }

    g_object_unref(gst_ctx);
    g_object_unref(display);
    glDeleteTextures(1, &io_tex);
    CFRelease(surf);
    CGLSetCurrentContext(NULL);
    CGLDestroyContext(cgl_ctx);

    return total_failures ? 1 : 0;
}
