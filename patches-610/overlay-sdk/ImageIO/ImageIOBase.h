/*
 * ImageIO/ImageIOBase.h shim — the 10.6 ImageIO framework does not ship this
 * umbrella header (it was added later).  WebKit's PAL/pal/spi/cg/ImageIOSPI.h
 * does `#include <ImageIO/ImageIOBase.h>` solely to obtain the IMAGEIO_EXTERN
 * linkage macro used on the kCGImageSource* constant declarations.  Provide a
 * minimal definition so that include resolves.
 */
#ifndef IMAGEIO_BASE_SHIM_H
#define IMAGEIO_BASE_SHIM_H

#include <CoreFoundation/CoreFoundation.h>

#ifndef IMAGEIO_EXTERN
#define IMAGEIO_EXTERN extern
#endif

#endif /* IMAGEIO_BASE_SHIM_H */
