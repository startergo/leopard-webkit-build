// [leopard] ObjC class stubs for 10.7+/10.9+ classes absent on 10.6 that WebCore references.
// Soft-referenced behind OS-version guards; empty stubs satisfy the _OBJC_CLASS_ link refs.
// Compiled with -fno-objc-arc; no heavy headers (avoids Foundation->Security SecKeychain issue).
@protocol NSObject
@end
__attribute__((objc_root_class))
@interface _LeopardStubBase { Class isa; }
@end
@implementation _LeopardStubBase
@end

@interface NSAppearance : _LeopardStubBase
@end
@implementation NSAppearance
@end

@interface WebActionDisablingCALayerDelegate : _LeopardStubBase
@end
@implementation WebActionDisablingCALayerDelegate
@end
