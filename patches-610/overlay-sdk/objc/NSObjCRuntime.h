/*
 * objc/NSObjCRuntime.h shim — the 10.6 SDK's objc/ umbrella lacks this header
 * (NSObjCRuntime.h lives in Foundation).  Route the include to the real header
 * so `#include <objc/NSObjCRuntime.h>` (e.g. NetworkExtensionContentFilter.h)
 * resolves.
 */
#import <Foundation/NSObjCRuntime.h>
