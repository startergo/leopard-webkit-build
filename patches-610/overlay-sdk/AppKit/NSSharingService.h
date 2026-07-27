/*
 * AppKit/NSSharingService.h shim for Mac OS X 10.6 SDK compatibility.
 *
 * NSSharingService and NSSharingServicePicker were added to AppKit in 10.8 (Mountain
 * Lion); the 10.6 SDK ships no AppKit/NSSharingService.h, so PAL's
 * NSSharingServicePickerSPI.h (which #imports it) fails with "file not found".
 *
 * Declares the two classes so the SPI header's NSSharingServicePicker (Private)
 * category compiles.  The sharing-service code paths are gated at runtime on 10.8+
 * availability and are never reached on 10.6, so no real implementation is needed:
 * @interface without @implementation emits no symbols and the classes do not exist on
 * the 10.6 runtime (any message to them is simply never sent).
 */
#import <Foundation/Foundation.h>

#ifndef APPKIT_NSSHARINGSERVICE_SHIM_605_H
#define APPKIT_NSSHARINGSERVICE_SHIM_605_H

@interface NSSharingService : NSObject
@end

@interface NSSharingServicePicker : NSObject
@end

#endif /* APPKIT_NSSHARINGSERVICE_SHIM_605_H */
