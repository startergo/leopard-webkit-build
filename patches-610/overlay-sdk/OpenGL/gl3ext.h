/*
 * OpenGL/gl3ext.h shim — gl3ext.h (the OpenGL 3.x extension declarations,
 * paired with gl3.h on the core profile) was added to the OpenGL framework in
 * 10.7; the MacOSX 10.6 SDK ships only <OpenGL/gl.h> + <OpenGL/glext.h>.
 * Extensions3DOpenGLCommon.cpp includes <OpenGL/gl3ext.h> unconditionally on
 * PLATFORM(MAC), so without this header that include is a fatal "file not found".
 *
 * The 10.6 GL stack is OpenGL 2.1 (no core profile), so the gl3ext set is empty
 * in practice; redirect to the 10.6 glext.h, which already provides the legacy
 * extension declarations this TU may reference.  glext.h is idempotent
 * (include-guarded), and gl3.h already pulled it in, so this is a safe no-op.
 */
#ifndef OPENGL_GL3EXT_SHIM_605_H
#define OPENGL_GL3EXT_SHIM_605_H

#include <OpenGL/glext.h>

#endif /* OPENGL_GL3EXT_SHIM_605_H */
