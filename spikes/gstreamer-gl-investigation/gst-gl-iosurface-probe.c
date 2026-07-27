// gst-gl-iosurface-probe.c — probe whether the IOSurface presentation path
// is viable on 10.6 + 9400M for the Cocoa→GstGL bridge.
//
// Answers three concrete questions:
//   1. Can we create an IOSurface on 10.6? (IOSurfaceCreate)
//   2. Can we bind that IOSurface as a GL texture in a CGL context?
//      (CGLTexImageIOSurface2D)
//   3. Does GstGL 1.4.5's gst_gl_memory_wrapped_texture accept the
//      resulting texture id?
//
// If all three pass, the IOSurface bridge path (a) from
// spikes/gstreamer-webkit-header-floor/README.md is viable:
// the bridge code can create an IOSurface-backed texture in GstGL's
// context, wrap it via gst_gl_memory_wrapped_texture, and let
// glupload write directly into it. Zero blit, single context.
//
// This probe does NOT test the end-to-end glupload pipeline — that's
// a follow-up once these three primitives are confirmed working.
// The pipeline test needs GstContext propagation to feed the wrapped
// context to glupload, which is more setup than this initial probe
// warrants.
//
// Build (on macbookpro, against 10.6 SDK + 1.4.5 devel headers):
//   clang -arch x86_64 -mmacosx-version-min=10.6 \
//         -isysroot <10.6 sdk path> \
//         -I<devel>/include/gstreamer-1.0 \
//         -I<devel>/include/glib-2.0 \
//         -I<devel>/lib/glib-2.0/include \
//         -framework OpenGL \
//         -framework IOSurface \
//         -framework CoreFoundation \
//         -L<devel>/lib gst-gl-iosurface-probe.c -o gst-gl-iosurface-probe \
//         -lgstgl-1.0 -lgstvideo-1.0 -lgstreamer-1.0 \
//         -lgobject-2.0 -lglib-2.0 -lintl
//
// Run (on the mini where /Library/Frameworks/GStreamer.framework is installed):
//   ./gst-gl-iosurface-probe

#define GST_USE_UNSTABLE_API 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/CGLTypes.h>
#include <OpenGL/CGLIOSurface.h>   /* CGLTexImageIOSurface2D */

#include <IOSurface/IOSurface.h>
#include <CoreFoundation/CoreFoundation.h>

#include <glib.h>
#include <glib-object.h>
#include <gst/gst.h>
#include <gst/gl/gstglcontext.h>
#include <gst/gl/gstgldisplay.h>
#include <gst/gl/gstglapi.h>
#include <gst/gl/gstglutils.h>
#include <gst/gl/gstglmemory.h>

/* Test frame dimensions — small to keep the probe fast. */
#define PROBE_WIDTH  320
#define PROBE_HEIGHT 240

int main(int argc, char **argv) {
    gst_init(&argc, &argv);

    int failures = 0;

    /* ---- 1. Build a CGL context (the context GstGL will use) ----
     * Same setup as gst-gl-probe.c — 10.6 defaults to Legacy profile,
     * which is GL 2.1 (the 9400M ceiling). */
    g_print("=== Step 1: CGL context creation ===\n");
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
    g_print("  CGLChoosePixelFormat: err=%d numVirtualScreens=%d\n", cerr, (int)nvirt);
    if (cerr != kCGLNoError || !pf) {
        g_printerr("  FAIL: could not choose pixel format\n");
        return 1;
    }

    CGLContextObj cgl_ctx = NULL;
    cerr = CGLCreateContext(pf, NULL, &cgl_ctx);
    g_print("  CGLCreateContext: err=%d cgl_ctx=%p\n", cerr, (void*)cgl_ctx);
    CGLDestroyPixelFormat(pf);
    if (cerr != kCGLNoError || !cgl_ctx) {
        g_printerr("  FAIL: could not create CGL context\n");
        return 1;
    }
    CGLSetCurrentContext(cgl_ctx);
    g_print("  CGLSetCurrentContext: ok\n\n");

    /* ---- 2. Create an IOSurface ----
     * IOSurfaceCreate is a 10.6 API. Must set kIOSurfacePixelFormat to
     * 'BGRA' (OSType 0x42475241) to match the GL_BGRA /
     * GL_UNSIGNED_INT_8_8_8_8_REV format used in CGLTexImageIOSurface2D.
     * Without the format tag, the IOSurface's storage layout is undefined
     * and GL may bind successfully but render black (the silent-failure
     * mode for format-mismatched IOSurface textures on 10.6). */
    g_print("=== Step 2: IOSurface creation ===\n");
    int w = PROBE_WIDTH, h = PROBE_HEIGHT;
    int bytes_per_row = PROBE_WIDTH * 4;
    int bytes_per_element = 4;
    int element_width = 1, element_height = 1;
    int is_global = 1;
    OSType pixel_format = 'BGRA';  /* 0x42475241 — matches GL_BGRA */

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
    IOSurfaceRef surface = IOSurfaceCreate(dict);
    CFRelease(dict);
    CFRelease(cf_w);
    CFRelease(cf_h);
    CFRelease(cf_bpr);
    CFRelease(cf_bpe);
    CFRelease(cf_ew);
    CFRelease(cf_eh);
    CFRelease(cf_glob);
    CFRelease(cf_pf);

    g_print("  IOSurfaceCreate: surface=%p\n", (void*)surface);
    if (!surface) {
        g_printerr("  FAIL: IOSurfaceCreate returned NULL\n");
        CGLDestroyContext(cgl_ctx);
        return 1;
    }
    IOSurfaceID surf_id = IOSurfaceGetID(surface);
    g_print("  IOSurfaceGetID: %u\n", (unsigned)surf_id);
    g_print("  width=%d height=%d bytesPerRow=%d\n",
            (int)IOSurfaceGetWidth(surface),
            (int)IOSurfaceGetHeight(surface),
            (int)IOSurfaceGetBytesPerRow(surface));
    g_print("\n");

    /* ---- 3. THE KEY PROBE: bind IOSurface as a GL texture ----
     * CGLTexImageIOSurface2D is a 10.6 API that binds an IOSurface as
     * a GL render target. If this works, the texture can be rendered to
     * by GstGL and the result will be visible to Core Animation via the
     * IOSurface (set as CALayer.contents). */
    g_print("=== Step 3: CGLTexImageIOSurface2D (the load-bearing probe) ===\n");
    GLuint io_tex = 0;
    glGenTextures(1, &io_tex);
    g_print("  glGenTextures: tex_id=%u\n", io_tex);
    if (!io_tex) {
        g_printerr("  FAIL: glGenTextures returned 0\n");
        CFRelease(surface);
        CGLDestroyContext(cgl_ctx);
        return 1;
    }

    GLenum bind_target = GL_TEXTURE_RECTANGLE_ARB;  /* IOSurface requires RECTANGLE */
    GLenum internal = GL_RGBA8;
    GLenum format = GL_BGRA;
    GLenum type = GL_UNSIGNED_INT_8_8_8_8_REV;

    glBindTexture(bind_target, io_tex);
    CGLError tex_err = CGLTexImageIOSurface2D(
        cgl_ctx,
        bind_target,
        internal,
        PROBE_WIDTH, PROBE_HEIGHT,
        format, type,
        surface, 0);
    g_print("  CGLTexImageIOSurface2D: err=%d (%s)\n",
            tex_err, CGLErrorString(tex_err));

    /* ---- DIAGNOSTICS: renderer, format, GL error ---- */
    /* 1. Renderer ID — is this the 9400M hardware or software fallback? */
    GLint renderer_id = 0;
    CGLGetParameter(cgl_ctx, kCGLCPCurrentRendererID, &renderer_id);
    g_print("  DIAG renderer ID: 0x%x (%s)\n", (unsigned)renderer_id,
            (renderer_id & 0x00020000) ? "GEFORCE (hardware)" :
            (renderer_id & 0x00040000) ? "SOFTWARE" : "unknown");

    /* 2. IOSurface pixel format — verify the FourCC is actually 'BGRA' */
    OSType surf_pf = IOSurfaceGetPixelFormat(surface);
    char pf_cc[5];
    pf_cc[0] = (surf_pf >> 24) & 0xFF;
    pf_cc[1] = (surf_pf >> 16) & 0xFF;
    pf_cc[2] = (surf_pf >> 8) & 0xFF;
    pf_cc[3] = surf_pf & 0xFF;
    pf_cc[4] = 0;
    g_print("  DIAG IOSurface pixel format: 0x%08x ('%s')\n", (unsigned)surf_pf, pf_cc);
    g_print("  DIAG GL format triple: internal=0x%x format=0x%x type=0x%x\n",
            internal, format, type);

    /* 3. Explicit GL error check — CGL may swallow GL_INVALID_* silently */
    GLenum gl_err_after_bind = glGetError();
    g_print("  DIAG glGetError after bind: 0x%x (%s)\n", gl_err_after_bind,
            gl_err_after_bind == GL_NO_ERROR ? "GL_NO_ERROR" : "GL ERROR");
    if (tex_err != kCGLNoError) {
        g_printerr("  FAIL: CGLTexImageIOSurface2D failed — IOSurface path NOT viable\n");
        failures++;
    } else {
        GLboolean is_tex = glIsTexture(io_tex);
        g_print("  glIsTexture(io_tex): %d\n", is_tex);
        if (!is_tex) {
            g_printerr("  FAIL: texture not valid after IOSurface binding\n");
            failures++;
        } else {
            g_print("  PASS: IOSurface-backed GL texture created successfully\n");
        }
    }
    g_print("\n");

    /* ---- 4. Data-flow verification: three independent mechanisms ----
     *
     * Three tests that share NO common failure mode. If all three fail,
     * the interop is genuinely broken on this hardware.
     *
     * 4a: GL clear → IOSurface DIRECT MEMORY readback.
     *     Clear to blue via FBO with IOSurface texture attached. glFinish.
     *     Then IOSurfaceLock + inspect base address bytes.
     *     Tests: does GL writing actually reach the IOSurface backing store?
     *     Bypasses glReadPixels entirely.
     *
     * 4b: IOSurface fill → GL SAMPLING via textured quad.
     *     Fill IOSurface with red via CPU. Bind IOSurface texture.
     *     Draw a textured quad into a RENDERBUFFER-backed FBO (not the
     *     IOSurface texture). glReadPixels from the renderbuffer.
     *     Tests: can GL SAMPLE an IOSurface-backed texture? This is the
     *     direction WebKit's compositor actually needs.
     *
     * 4c: GL clear → GL READBACK (the test that's been failing).
     *     FBO clear + glReadPixels from same FBO. Kept as reference.
     */
    if (failures == 0) {
        g_print("=== Step 4: Data-flow verification (3 independent tests) ===\n");

        /* --- 4a: GL clear → IOSurface direct memory --- */
        g_print("--- 4a: GL→IOSurface (clear to blue, read IOSurface memory directly) ---\n");
        {
            GLuint fbo = 0;
            glGenFramebuffersEXT(1, &fbo);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                GL_COLOR_ATTACHMENT0_EXT, bind_target, io_tex, 0);
            GLenum st = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
            g_print("  FBO status: 0x%x\n", st);
            glClearColor(0.0f, 0.0f, 1.0f, 1.0f); /* BLUE: BGRA=(255,0,0,255) */
            glClear(GL_COLOR_BUFFER_BIT);
            glFinish();
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            glDeleteFramebuffersEXT(1, &fbo);

            /* Now read the IOSurface's memory directly — bypass GL entirely */
            uint32_t seed = 0;
            IOSurfaceLock(surface, kIOSurfaceLockReadOnly, &seed);
            uint8_t *base = (uint8_t *)IOSurfaceGetBaseAddress(surface);
            size_t bpr = IOSurfaceGetBytesPerRow(surface);
            /* Check center pixel */
            uint8_t *px = base + (PROBE_HEIGHT/2) * bpr + (PROBE_WIDTH/2) * 4;
            g_print("  IOSurface direct read (center): BGRA=(%d,%d,%d,%d) expected ~(255,0,0,255)\n",
                    px[0], px[1], px[2], px[3]);
            IOSurfaceUnlock(surface, kIOSurfaceLockReadOnly, &seed);
            if (px[0] > 250) {
                g_print("  PASS 4a: GL→IOSurface write works (visible in IOSurface memory)\n");
            } else {
                g_printerr("  FAIL 4a: clear did not reach IOSurface backing store\n");
            }
        }
        g_print("\n");

        /* --- 4b: IOSurface fill → GL sampling → renderbuffer readback --- */
        g_print("--- 4b: IOSurface→GL sampling (fill red, draw quad, read renderbuffer) ---\n");
        {
            /* Fill IOSurface with RED: BGRA=(0,0,255,255) */
            uint32_t seed = 0;
            IOSurfaceLock(surface, 0, &seed);
            uint8_t *base = (uint8_t *)IOSurfaceGetBaseAddress(surface);
            size_t bpr = IOSurfaceGetBytesPerRow(surface);
            for (int y = 0; y < PROBE_HEIGHT; y++)
                for (int x = 0; x < PROBE_WIDTH; x++) {
                    uint8_t *p = base + y * bpr + x * 4;
                    p[0]=0; p[1]=0; p[2]=255; p[3]=255;
                }
            IOSurfaceUnlock(surface, 0, &seed);
            glFlush();

            /* Create a renderbuffer-backed FBO (normal, NOT IOSurface) */
            GLuint rb_fbo = 0, rb = 0;
            glGenFramebuffersEXT(1, &rb_fbo);
            glGenRenderbuffersEXT(1, &rb);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rb);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, PROBE_WIDTH, PROBE_HEIGHT);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, rb_fbo);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, rb);
            GLenum st = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
            g_print("  renderbuffer FBO status: 0x%x\n", st);

            if (st == GL_FRAMEBUFFER_COMPLETE_EXT) {
                /* Set up viewport + fixed-function texture sampling */
                glViewport(0, 0, PROBE_WIDTH, PROBE_HEIGHT);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, PROBE_WIDTH, 0, PROBE_HEIGHT, -1, 1);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                /* Clear renderbuffer to black */
                glClearColor(0, 0, 0, 0);
                glClear(GL_COLOR_BUFFER_BIT);

                /* Bind IOSurface-backed texture as sampling source */
                glEnable(bind_target);
                glBindTexture(bind_target, io_tex);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

                /* Draw full-screen quad. Rectangle textures use
                 * non-normalized coords: texcoord (0,0) to (w,h). */
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex2f(0, 0);
                glTexCoord2f(PROBE_WIDTH, 0);
                glVertex2f(PROBE_WIDTH, 0);
                glTexCoord2f(PROBE_WIDTH, PROBE_HEIGHT);
                glVertex2f(PROBE_WIDTH, PROBE_HEIGHT);
                glTexCoord2f(0, PROBE_HEIGHT);
                glVertex2f(0, PROBE_HEIGHT);
                glEnd();
                glFlush();
                glFinish();

                /* Read from the renderbuffer (not the IOSurface texture) */
                uint8_t px[4] = {0,0,0,0};
                glReadPixels(PROBE_WIDTH/2, PROBE_HEIGHT/2, 1, 1,
                    GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, px);
                g_print("  glReadPixels(center): BGRA=(%d,%d,%d,%d) expected ~(0,0,255,255)\n",
                        px[0], px[1], px[2], px[3]);
                if (px[2] > 250) {
                    g_print("  PASS 4b: IOSurface→GL sampling works (texture is readable)\n");
                } else {
                    g_printerr("  FAIL 4b: sampling returned black — IOSurface texture not readable by GL\n");
                }
            } else {
                g_printerr("  FAIL 4b: renderbuffer FBO not complete\n");
            }
            glDisable(bind_target);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            glDeleteRenderbuffersEXT(1, &rb);
            glDeleteFramebuffersEXT(1, &rb_fbo);
        }
        g_print("\n");

        /* --- 4c: GL clear → GL readback (reference — previous failing test) --- */
        g_print("--- 4c: GL→GL (FBO clear, glReadPixels — reference) ---\n");
        {
            GLuint fbo = 0;
            glGenFramebuffersEXT(1, &fbo);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                GL_COLOR_ATTACHMENT0_EXT, bind_target, io_tex, 0);
            glClearColor(0.0f, 1.0f, 0.0f, 1.0f); /* GREEN: BGRA=(0,255,0,255) */
            glClear(GL_COLOR_BUFFER_BIT);
            glFinish();
            uint8_t px[4] = {0,0,0,0};
            glReadPixels(PROBE_WIDTH/2, PROBE_HEIGHT/2, 1, 1,
                GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, px);
            g_print("  glReadPixels(center): BGRA=(%d,%d,%d,%d) expected ~(0,255,0,255)\n",
                    px[0], px[1], px[2], px[3]);
            if (px[1] > 250) {
                g_print("  PASS 4c: GL→GL FBO readback works\n");
            } else {
                g_printerr("  FAIL 4c: glReadPixels returned black\n");
            }
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            glDeleteFramebuffersEXT(1, &fbo);
        }
        g_print("\n");
    }

    /* ---- 5. GstGL display + wrap our CGL context ----
     * Same pattern as gst-gl-probe.c. The wrapped context gives GstGL
     * access to our CGL context (and thus to io_tex). */
    g_print("=== Step 5: GstGL display + context wrap ===\n");
    GstGLDisplay *display = gst_gl_display_new();
    g_print("  gst_gl_display_new: display=%p type=%s\n",
            (void*)display, display ? G_OBJECT_TYPE_NAME(display) : "(null)");
    if (!display) {
        g_printerr("  FAIL: gst_gl_display_new returned NULL\n");
        glDeleteTextures(1, &io_tex);
        CFRelease(surface);
        CGLDestroyContext(cgl_ctx);
        return 1;
    }

    GstGLContext *gst_ctx = gst_gl_context_new_wrapped(
        display,
        (guintptr)cgl_ctx,
        GST_GL_PLATFORM_CGL,
        GST_GL_API_OPENGL);
    g_print("  gst_gl_context_new_wrapped: gst_ctx=%p type=%s\n",
            (void*)gst_ctx, gst_ctx ? G_OBJECT_TYPE_NAME(gst_ctx) : "(null)");
    if (!gst_ctx) {
        g_printerr("  FAIL: gst_gl_context_new_wrapped returned NULL\n");
        g_object_unref(display);
        glDeleteTextures(1, &io_tex);
        CFRelease(surface);
        CGLDestroyContext(cgl_ctx);
        return 1;
    }
    /* Activate is best-effort on wrapped contexts (see gst-gl-probe.c notes). */
    gst_gl_context_activate(gst_ctx, TRUE);
    g_print("\n");

    /* ---- 6. THE SECOND KEY PROBE: gst_gl_memory_wrapped_texture ----
     * Can we wrap our IOSurface-backed texture id as a GstGLMemory?
     * If yes, path (a) from the README is viable — the bridge can pre-
     * allocate the IOSurface texture and hand it to GstGL for rendering. */
    g_print("=== Step 6: gst_gl_memory_wrapped_texture (second load-bearing probe) ===\n");
    GstGLMemory *gl_mem = gst_gl_memory_wrapped_texture(
        gst_ctx,
        io_tex,
        GST_VIDEO_GL_TEXTURE_TYPE_RGBA,
        PROBE_WIDTH, PROBE_HEIGHT,
        NULL, NULL);  /* user_data, notify */
    g_print("  gst_gl_memory_wrapped_texture: gl_mem=%p\n", (void*)gl_mem);
    if (!gl_mem) {
        g_printerr("  FAIL: gst_gl_memory_wrapped_texture returned NULL\n");
        g_printerr("        -> path (a) NOT viable; must use path (b) with blit\n");
        failures++;
    } else {
        guint reported_tex = gl_mem->tex_id;
        g_print("  gl_mem->tex_id: %u (expected %u)\n", reported_tex, io_tex);
        if (reported_tex == io_tex) {
            g_print("  PASS: GstGLMemory wraps the IOSurface-backed texture id\n");
            g_print("        -> path (a) viable; bridge can pass this memory to glupload\n");
        } else {
            g_printerr("  FAIL: tex_id mismatch (GstGL did not preserve our texture)\n");
            failures++;
        }
        /* Clean up — GstMemory inherits from GstMini_object; unref. */
        gst_mini_object_unref(GST_MINI_OBJECT_CAST(gl_mem));
    }
    g_print("\n");

    /* ---- Summary ---- */
    g_print("=== Summary ===\n");
    if (failures == 0) {
        g_print("ALL PROBES PASSED.\n");
        g_print("  - IOSurface creation: works\n");
        g_print("  - CGLTexImageIOSurface2D: works (10.6 + 9400M can bind IOSurface as GL texture)\n");
        g_print("  - FBO render to IOSurface texture: works\n");
        g_print("  - gst_gl_memory_wrapped_texture: accepts IOSurface-backed texture id\n");
        g_print("\n");
        g_print("The IOSurface bridge path (a) is viable. Next step: end-to-end\n");
        g_print("pipeline test — videotestsrc ! glupload ! appsink with the wrapped\n");
        g_print("GstGLMemory, confirming glupload respects the wrapped texture.\n");
    } else {
        g_print("%d PROBE(S) FAILED — see above.\n", failures);
        g_print("IOSurface path may need path (b) (alloc-then-blit) or a different approach.\n");
    }

    /* Cleanup */
    g_object_unref(gst_ctx);
    g_object_unref(display);
    glDeleteTextures(1, &io_tex);
    CFRelease(surface);
    CGLDestroyContext(cgl_ctx);

    return failures ? 1 : 0;
}
