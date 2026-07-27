/*
 * OpenGL/gl3.h shim — gl3.h (the OpenGL 3.x core-profile declarations) was added
 * to the system OpenGL.framework in 10.7; the MacOSX 10.6 SDK ships only
 * <OpenGL/gl.h> + <OpenGL/glext.h> (OpenGL 2.1).  WebKit's WebGL layer includes
 * <OpenGL/gl3.h> unconditionally on PLATFORM(MAC), so without this header that
 * include is a fatal "file not found".
 *
 * This shim redirects to the 10.6 OpenGL headers and supplies the handful of GL3
 * query enumerators that the WebGL extension layer references
 * (GL_NUM_EXTENSIONS / GL_MAJOR_VERSION / GL_MINOR_VERSION).  The GL3 *functions*
 * (glGetStringi, glGenVertexArrays, ...) are DELIBERATELY NOT re-declared here:
 * they are absent from the 10.6 libGL binary, so any TU that calls one must gate
 * the call site behind a `__MAC_OS_X_VERSION_MAX_ALLOWED >= 1070` version check
 * (see Extensions3DOpenGLCommon.cpp::initializeAvailableExtensions) rather than
 * rely on a declaration that would only defer the error to link time.
 */
#ifndef OPENGL_GL3_SHIM_605_H
#define OPENGL_GL3_SHIM_605_H

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

/* GL3 context-query enumerators (OpenGL 3.0), absent from the 10.6 headers. */
#ifndef GL_MAJOR_VERSION
#define GL_MAJOR_VERSION 0x821B
#endif
#ifndef GL_MINOR_VERSION
#define GL_MINOR_VERSION 0x821C
#endif
#ifndef GL_NUM_EXTENSIONS
#define GL_NUM_EXTENSIONS 0x821D
#endif
/* GL_MAX_VARYING_VECTORS is an OpenGL ES 2.0 enumerator (0x8DFC) absent from the
   desktop 10.6 glext.h; referenced by GraphicsContext3DOpenGL.cpp's
   MAX_VARYING_VECTORS query (the GLES2-compliant branch). */
#ifndef GL_MAX_VARYING_VECTORS
#define GL_MAX_VARYING_VECTORS 0x8DFC
#endif
/* GL_MAX_VARYING_COMPONENTS (0x8B4B) is present on modern SDKs only as the
   ARB-suffixed GL_MAX_VARYING_COMPONENTS_ARB in the 10.6 glext.h; the unsuffixed
   desktop-GL3 spelling is absent.  Referenced by GraphicsContext3DOpenGL.cpp. */
#ifndef GL_MAX_VARYING_COMPONENTS
#define GL_MAX_VARYING_COMPONENTS 0x8B4B
#endif

#endif /* OPENGL_GL3_SHIM_605_H */
