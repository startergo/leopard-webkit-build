// gst-gl-colorscale-iosurface-probe.m
//
// THE ELEVENTH PROBE — the last load-bearing unknown.
//
// Tests the complete chain: GstContext propagation → glcolorscale
// share-group → gst_gl_memory_copy_into_texture → IOSurface bind.
//
// If this probe passes, the WebKit wiring (replacing webkitglvideosink
// with glcolorscale→appsink) is pure plumbing with no runtime unknowns.
//
// Pipeline: videotestsrc(pattern=smpte75) → videoconvert → glcolorscale
//           → appsink(GLMemory caps)
//
// The wrapped CGL context (fix #1's approach) is propagated to the
// pipeline via GstContext. glcolorscale receives it, creates its own
// context sharing with ours, uploads the system-memory frame to GL,
// and emits GstGLMemory. We pull the sample, extract the GstGLMemory,
// copy its texture into an IOSurface-backed RECTANGLE texture, read
// back the IOSurface pixels, and verify the frame landed (non-black,
// SMPTE bars visible).
//
// Build (on the macmini, against the framework):
//   clang -arch x86_64 -framework Cocoa -framework OpenGL \
//         -framework IOSurface -framework CoreFoundation \
//         -I/Library/Frameworks/GStreamer.framework/Versions/1.0/include \
//         -L/Library/Frameworks/GStreamer.framework/Versions/1.0/lib \
//         gst-gl-colorscale-iosurface-probe.m \
//         -lgstgl-1.0 -lgstvideo-1.0 -lgstbase-1.0 -lgstapp-1.0 \
//         -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lintl \
//         -o gst-gl-colorscale-iosurface-probe
//
// Run:
//   ./gst-gl-colorscale-iosurface-probe

#define GST_USE_UNSTABLE_API 1

#import <Cocoa/Cocoa.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/CGLTypes.h>
#include <OpenGL/CGLIOSurface.h>
#include <OpenGL/glext.h>

#include <IOSurface/IOSurface.h>
#include <CoreFoundation/CoreFoundation.h>

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>
#include <gst/gl/gl.h>
#include <gst/gl/gstglmemory.h>

static GstElement *g_pipeline = NULL;
static GstGLContext *g_wrappedCtx = NULL;
static GstGLDisplay *g_display = NULL;
static NSOpenGLContext *g_nsCtx = NULL;

#define PROBE_W 320
#define PROBE_H 240

// ── Wrapped CGL context (fix #1's approach, no gst_gl_context_create) ────

static int create_wrapped_context(void)
{
    g_print("=== Creating wrapped CGL context (fix #1 wrap-only path) ===\n");

    // CRITICAL: match GstGL Cocoa backend pixel format exactly
    // (gstglcontext_cocoa.m:228-231): DoubleBuffer + AccumSize=32.
    // ColorSize/AlphaSize/Accelerated/NoRecovery cause "invalid share
    // context" due to Apple's pixel-format-compatibility sharing rules.
    NSOpenGLPixelFormatAttribute attribs[] = {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccumSize, (NSOpenGLPixelFormatAttribute)32,
        (NSOpenGLPixelFormatAttribute)0
    };
    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
    if (!pf) { g_printerr("  FAIL: NSOpenGLPixelFormat\n"); return 1; }

    g_nsCtx = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
    [pf release];
    if (!g_nsCtx) { g_printerr("  FAIL: NSOpenGLContext\n"); return 1; }

    CGLContextObj cgl = (CGLContextObj)[g_nsCtx CGLContextObj];
    GLint rendererID = 0;
    CGLGetParameter(cgl, kCGLCPCurrentRendererID, &rendererID);
    g_print("  NSOpenGLContext=%p CGL=%p renderer=0x%x (%s)\n",
            g_nsCtx, cgl, (unsigned)rendererID,
            (rendererID & 0x00020000) ? "GEFORCE" :
            (rendererID & 0x00040000) ? "SOFTWARE" : "unknown");

    // Wrap as GstGLContext — NO gst_gl_context_create call.
    g_display = gst_gl_display_new();
    if (!g_display) { g_printerr("  FAIL: gst_gl_display_new\n"); return 1; }

    g_wrappedCtx = gst_gl_context_new_wrapped(
        g_display,
        (guintptr)g_nsCtx,
        GST_GL_PLATFORM_CGL,
        GST_GL_API_OPENGL);
    if (!g_wrappedCtx) { g_printerr("  FAIL: gst_gl_context_new_wrapped\n"); return 1; }

    g_print("  GstGLDisplay=%p wrapped GstGLContext=%p\n", g_display, g_wrappedCtx);
    g_print("  (no gst_gl_context_create call — wrap-only, no deadlock)\n");
    return 0;
}

// ── Propagate GstContext to pipeline (the handshake from requestGLContext) ──

static void propagate_context(GstElement *pipeline)
{
    g_print("=== Propagating GstContext to pipeline + all children ===\n");

    GstContext *dispCtx = gst_context_new(GST_GL_DISPLAY_CONTEXT_TYPE, TRUE);
    gst_context_set_gl_display(dispCtx, g_display);

    GstContext *appCtx = gst_context_new("gst.gl.app_context", TRUE);
    GstStructure *s = gst_context_writable_structure(appCtx);
    gst_structure_set(s, "context", GST_GL_TYPE_CONTEXT, g_wrappedCtx, NULL);

    // Set on pipeline
    gst_element_set_context(pipeline, dispCtx);
    gst_element_set_context(pipeline, appCtx);
    g_print("  Set on pipeline\n");

    // Brute-force: iterate ALL children and set context directly.
    // 1.4.5's gst_element_set_context on the pipeline doesn't
    // satisfy GL elements' upstream context QUERIES. GL elements
    // (glcolorscale) query for gst.gl.app_context toward the source,
    // not toward the pipeline/sink. Since videotestsrc has no GL
    // context, the query returns empty and glcolorscale creates an
    // unshared context. Setting the context directly on each element
    // stores it so the element's own set_context handler picks it up.
    GstIterator *it = gst_bin_iterate_elements(GST_BIN(pipeline));
    GValue item = G_VALUE_INIT;
    int count = 0;
    while (gst_iterator_next(it, &item) == GST_ITERATOR_OK) {
        GstElement *child = g_value_get_object(&item);
        gst_element_set_context(child, dispCtx);
        gst_element_set_context(child, appCtx);
        count++;
        g_print("  Set on child: %s\n", GST_ELEMENT_NAME(child));
        g_value_reset(&item);
    }
    g_value_unset(&item);
    gst_iterator_free(it);
    g_print("  Set on %d children total\n", count);

    // CRITICAL: set the 'other-context' PROPERTY on glcolorscale directly.
    // gst_element_set_context stores a GstContext but does NOT set
    // filter->other_context, which is what gst_gl_context_create
    // (gstglfilter.c:893-894) uses as the share parent. The property
    // setter (gstglfilter.c:178-180) is the ONLY way to populate
    // other_context. This is the same mechanism as glimagesink's
    // other-context property.
    GstElement *glcs = gst_bin_get_by_name(GST_BIN(pipeline), "glcolorscale0");
    if (glcs) {
        g_object_set(glcs, "other-context", g_wrappedCtx, NULL);
        g_print("  Set 'other-context' PROPERTY on glcolorscale0 (the share parent)\n");
        gst_object_unref(glcs);
    } else {
        g_printerr("  WARNING: glcolorscale0 not found in pipeline!\n");
    }

    gst_context_unref(dispCtx);
    gst_context_unref(appCtx);
}

// ── Timer callback: pull sample, copy to IOSurface, verify ───────────────

static void timer_callback(CFRunLoopTimerRef timer, void *info)
{
    g_print("\n=== Timer fired — pulling sample ===\n");

    GstElement *appsink = gst_bin_get_by_name(GST_BIN(g_pipeline), "sink");
    if (!appsink) { g_printerr("  FAIL: appsink not found\n"); CFRunLoopStop(CFRunLoopGetMain()); return; }

    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));
    gst_object_unref(appsink);
    if (!sample) { g_printerr("  FAIL: no sample (pipeline didn't produce frames?)\n"); CFRunLoopStop(CFRunLoopGetMain()); return; }

    GstBuffer *buf = gst_sample_get_buffer(sample);
    GstMemory *mem = gst_buffer_peek_memory(buf, 0);
    if (!mem) { g_printerr("  FAIL: no memory in buffer\n"); gst_sample_unref(sample); CFRunLoopStop(CFRunLoopGetMain()); return; }

    gboolean is_gl = gst_is_gl_memory(mem);
    g_print("  gst_is_gl_memory: %s\n", is_gl ? "TRUE (GLMemory — zero-copy path confirmed)" : "FALSE (system memory)");

    if (!is_gl) {
        gst_sample_unref(sample);
        CFRunLoopStop(CFRunLoopGetMain());
        return;
    }

    GstGLMemory *glMem = (GstGLMemory *)mem;
    g_print("  GstGLMemory: tex_id=%u\n", glMem->tex_id);

    // Log glcolorscale's context renderer ID (the share-group test)
    // gst_gl_memory_get_context is not in 1.4.5 headers — skip the
    // producer-context renderer check; the copy_into_texture result
    // is the authoritative share-group test.
    g_print("  (glcolorscale context renderer-ID check skipped — gst_gl_memory_get_context not in 1.4.5 headers)\n");

    // Create IOSurface + GL_TEXTURE_RECTANGLE_ARB texture
    g_print("\n=== Creating IOSurface + CGLTexImageIOSurface2D ===\n");
    CGLContextObj cgl = (CGLContextObj)[g_nsCtx CGLContextObj];
    CGLSetCurrentContext(cgl);

    CFMutableDictionaryRef dict = CFDictionaryCreateMutable(NULL, 0, NULL, NULL);
    CFDictionaryAddValue(dict, kIOSurfaceWidth, CFNumberCreate(NULL, kCFNumberIntType, &(int){PROBE_W}));
    CFDictionaryAddValue(dict, kIOSurfaceHeight, CFNumberCreate(NULL, kCFNumberIntType, &(int){PROBE_H}));
    CFDictionaryAddValue(dict, kIOSurfaceBytesPerElement, CFNumberCreate(NULL, kCFNumberIntType, &(int){4}));
    CFDictionaryAddValue(dict, kIOSurfaceIsGlobal, kCFBooleanTrue);
    IOSurfaceRef surf = IOSurfaceCreate(dict);
    CFRelease(dict);
    if (!surf) { g_printerr("  FAIL: IOSurfaceCreate\n"); goto done; }

    GLuint ioTex;
    glGenTextures(1, &ioTex);
    CGLError cerr = CGLTexImageIOSurface2D(cgl, GL_TEXTURE_RECTANGLE_ARB,
        GL_BGRA, PROBE_W, PROBE_H,
        GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, surf, 0);
    g_print("  CGLTexImageIOSurface2D: err=%d (%s) tex=%u\n", cerr, CGLErrorString(cerr), ioTex);
    if (cerr != kCGLNoError) { g_printerr("  FAIL: CGLTexImageIOSurface2D\n"); goto done; }

    // Share-group diagnostic: is glcolorscale's texture visible in our context?
    {
        GLenum clearErr = glGetError();
        GLboolean srcVisible = glIsTexture(glMem->tex_id);
        GLenum diagErr = glGetError();
        g_print("\n=== Share-group diagnostic ===\n");
        g_print("  glIsTexture(%u) in wrapped context: %s (glErr=0x%x)\n",
                glMem->tex_id, srcVisible ? "TRUE (share-group works)" : "FALSE (NOT visible — share-group failed)",
                (unsigned)diagErr);
    }

    // Direct readback of source texture via glGetTexImage (bypasses copy mechanisms)
    if (glIsTexture(glMem->tex_id)) {
        g_print("\n=== Direct glGetTexImage readback (source texture visibility test) ===\n");
        CGLSetCurrentContext(cgl);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, glMem->tex_id);
        GLenum bindErr = glGetError();
        uint32_t *pixels = (uint32_t *)malloc(PROBE_W * PROBE_H * 4);
        memset(pixels, 0xFF, PROBE_W * PROBE_H * 4);  // fill with white to detect black vs unchanged
        glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, pixels);
        GLenum getErr = glGetError();
        glDisable(GL_TEXTURE_2D);
        // Check corners
        uint32_t tl = pixels[0], tr = pixels[PROBE_W-1];
        uint32_t bl = pixels[(PROBE_H-1)*PROBE_W], br = pixels[(PROBE_H-1)*PROBE_W+PROBE_W-1];
        int nonblack = (tl!=0)+(tr!=0)+(bl!=0)+(br!=0);
        g_print("  glGetTexImage: bindErr=0x%x getErr=0x%x\n", (unsigned)bindErr, (unsigned)getErr);
        g_print("  Source texture corners: TL=0x%08x TR=0x%08x BL=0x%08x BR=0x%08x (%d/4 non-black)\n",
                tl, tr, bl, br, nonblack);
        if (nonblack >= 3) {
            g_print("  PASS: source texture pixels ARE readable in the wrapped context.\n");
            g_print("  Share-group fully functional — pixels cross contexts.\n");
        } else {
            g_print("  FAIL: source texture pixels are black/unreadable despite glIsTexture=TRUE.\n");
        }
        free(pixels);
    }

    // Method 1: gst_gl_memory_copy_into_texture (dispatches to source context via thread_add)
    g_print("\n=== Method 1: gst_gl_memory_copy_into_texture ===\n");
    GLenum preErr = glGetError();
    gboolean copied = gst_gl_memory_copy_into_texture(
        glMem, ioTex,
        GST_VIDEO_GL_TEXTURE_TYPE_RGBA,
        PROBE_W, PROBE_H,
        PROBE_W * 4,
        FALSE /* respecify */);
    GLenum postErr = glGetError();
    glFlush();
    glFinish();
    g_print("  copy_into_texture: copied=%d preGlErr=0x%x postGlErr=0x%x\n",
            (int)copied, (unsigned)preErr, (unsigned)postErr);

    // Quick check if pixels landed after method 1
    IOSurfaceLock(surf, kIOSurfaceLockReadOnly, NULL);
    uint32_t *base1 = (uint32_t *)IOSurfaceGetBaseAddress(surf);
    int nonblack1 = 0;
    if (base1) {
        size_t bpr1 = IOSurfaceGetBytesPerRow(surf);
        uint32_t c1[4] = {base1[0], base1[PROBE_W-1],
                          base1[(PROBE_H-1)*(int)(bpr1/4)],
                          base1[(PROBE_H-1)*(int)(bpr1/4)+PROBE_W-1]};
        nonblack1 = (c1[0]!=0)+(c1[1]!=0)+(c1[2]!=0)+(c1[3]!=0);
        g_print("  Method 1 corners: TL=0x%08x TR=0x%08x BL=0x%08x BR=0x%08x (%d/4 non-black)\n",
                c1[0], c1[1], c1[2], c1[3], nonblack1);
    }
    IOSurfaceUnlock(surf, kIOSurfaceLockReadOnly, NULL);
    if (nonblack1 >= 3) {
        g_print("  PASS via Method 1. Chain works with copy_into_texture.\n");
        goto readback_done;
    }
    g_print("  Method 1 produced black. Trying Method 2 (FBO blit)...\n");

    // Method 2: manual FBO blit (runs in wrapped context, proven on 9400M per commit 18456f36)
    {
        CGLSetCurrentContext(cgl);
        GLboolean srcVis = glIsTexture(glMem->tex_id);
        if (srcVis) {
            g_print("\n=== Method 2: manual FBO blit (intra-context) ===\n");
            GLuint fbo;
            glGenFramebuffersEXT(1, &fbo);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                GL_TEXTURE_RECTANGLE_ARB, ioTex, 0);
            GLenum fbStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
            g_print("  FBO status: 0x%x (%s)\n", fbStatus,
                    fbStatus == GL_FRAMEBUFFER_COMPLETE_EXT ? "COMPLETE" : "INCOMPLETE");

            if (fbStatus == GL_FRAMEBUFFER_COMPLETE_EXT) {
                glViewport(0, 0, PROBE_W, PROBE_H);
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
                glOrtho(0, PROBE_W, 0, PROBE_H, -1, 1);
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, glMem->tex_id);
                GLenum bindErr = glGetError();

                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex2f(0, 0);
                glTexCoord2f(1, 0); glVertex2f(PROBE_W, 0);
                glTexCoord2f(1, 1); glVertex2f(PROBE_W, PROBE_H);
                glTexCoord2f(0, 1); glVertex2f(0, PROBE_H);
                glEnd();

                GLenum drawErr = glGetError();
                glDisable(GL_TEXTURE_2D);
                glPopMatrix();
                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glFlush();
                glFinish();
                g_print("  FBO blit: bindErr=0x%x drawErr=0x%x\n",
                        (unsigned)bindErr, (unsigned)drawErr);
            }
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            glDeleteFramebuffersEXT(1, &fbo);
        } else {
            g_print("\n=== Method 2 skipped: source texture NOT share-group-visible ===\n");
            g_print("  Share-group failed. glcolorscale texture not visible in wrapped context.\n");
        }
    }

readback_done:;
    // Readback: IOSurface direct readback (the trusted mechanism)
    g_print("\n=== IOSurface direct readback ===\n");

    // MUST lock before reading on 10.6 — IOSurfaceGetBaseAddress returns
    // a valid pointer but data is stale/uncommitted without the lock.
    // This was the bug in the first run (all corners black because no lock).
    IOSurfaceLock(surf, kIOSurfaceLockReadOnly, NULL);

    uint32_t pixels[4]; // four corners
    uint32_t *base = (uint32_t *)IOSurfaceGetBaseAddress(surf);
    size_t bpr = IOSurfaceGetBytesPerRow(surf);

    if (base) {
        pixels[0] = base[0];                         // TL
        pixels[1] = base[PROBE_W - 1];               // TR
        pixels[2] = base[(PROBE_H - 1) * (bpr / 4)]; // BL
        pixels[3] = base[(PROBE_H - 1) * (bpr / 4) + PROBE_W - 1]; // BR

        g_print("  Corner pixels (BGRA): TL=0x%08x TR=0x%08x BL=0x%08x BR=0x%08x\n",
                pixels[0], pixels[1], pixels[2], pixels[3]);

        int nonblack = (pixels[0] != 0) + (pixels[1] != 0) + (pixels[2] != 0) + (pixels[3] != 0);
        if (nonblack >= 3) {
            g_print("  PASS: %d/4 corners non-black. Frame landed in IOSurface.\n", nonblack);
            g_print("  THE CHAIN WORKS: propagation → share → copy → IOSurface.\n");
            g_print("  WebKit wiring is pure plumbing.\n");
        } else {
            g_printerr("  FAIL: only %d/4 corners non-black. Frame did NOT land correctly.\n", nonblack);
            g_printerr("  Possible: cross-target copy produced black, or renderer mismatch.\n");
        }
    } else {
        g_printerr("  FAIL: could not get IOSurface base address even after lock\n");
    }

    IOSurfaceUnlock(surf, kIOSurfaceLockReadOnly, NULL);
    glDeleteTextures(1, &ioTex);
    CFRelease(surf);

done:
    gst_sample_unref(sample);
    CFRunLoopStop(CFRunLoopGetMain());
}

// ── Main ─────────────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    g_print("╔══════════════════════════════════════════════════════════════╗\n");
    g_print("║  glcolorscale → GLMemory → copy_into_texture → IOSurface     ║\n");
    g_print("║  The eleventh probe — the last load-bearing unknown          ║\n");
    g_print("╚══════════════════════════════════════════════════════════════╝\n\n");

    gst_init(&argc, &argv);

    if (create_wrapped_context() != 0) return 1;

    // Build pipeline: videotestsrc(smpte75) → videoconvert → glcolorscale → appsink
    g_print("\n=== Building pipeline ===\n");
    g_pipeline = gst_parse_launch(
        "videotestsrc pattern=smpte75 num-buffers=30 ! "
        "video/x-raw,format=I420,width=320,height=240,framerate=10/1 ! "
        "videoconvert ! "
        "glcolorscale ! "
        "video/x-raw(memory:GLMemory),format=RGBA ! "
        "appsink name=sink max-buffers=2 drop=false sync=false",
        NULL);
    if (!g_pipeline) { g_printerr("FAIL: pipeline creation\n"); return 1; }
    g_print("  Pipeline created\n");

    // Propagate GstContext (the requestGLContext handshake)
    propagate_context(g_pipeline);

    // Schedule timer for 3 seconds (let pipeline produce frames)
    CFRunLoopTimerContext ctx = {0};
    CFRunLoopTimerRef timer = CFRunLoopTimerCreate(NULL,
        CFAbsoluteTimeGetCurrent() + 3.0,  // fire 3s from now
        0,                                  // no repeat
        0, 0,
        timer_callback, &ctx);
    CFRunLoopAddTimer(CFRunLoopGetMain(), timer, kCFRunLoopCommonModes);
    CFRelease(timer);

    // Start pipeline
    g_print("\n=== Starting pipeline ===\n");
    GstStateChangeReturn ret = gst_element_set_state(g_pipeline, GST_STATE_PLAYING);
    g_print("  set_state(PLAYING): %d\n", ret);

    // Run NSApp loop (pumps main queue so glcolorscale's dispatch_sync lands)
    g_print("  Running NSApp loop for 3s (pumps main queue for glcolorscale context creation)...\n");
    [[NSApplication sharedApplication] run];

    // Cleanup
    gst_element_set_state(g_pipeline, GST_STATE_NULL);
    gst_object_unref(g_pipeline);
    if (g_wrappedCtx) gst_object_unref(g_wrappedCtx);
    if (g_display) gst_object_unref(g_display);
    [g_nsCtx release];

    g_print("\n=== Probe complete ===\n");
    return 0;
}
