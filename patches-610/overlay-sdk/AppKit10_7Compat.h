/*
 * AppKit10_7Compat.h
 * Shared ObjC category declarations for AppKit 10.7+ properties.
 * Include this AFTER #import <AppKit/AppKit.h> in .mm files that use
 * NSEvent phase/scrolling properties, NSView layoutDirection, etc.
 */
#ifndef APPKIT10_7_COMPAT_H
#define APPKIT10_7_COMPAT_H

#import <AppKit/AppKit.h>

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070

@interface NSEvent (WebKit10_7Compat)
@property (nonatomic, readonly) NSInteger phase;
@property (nonatomic, readonly) NSInteger momentumPhase;
@property (nonatomic, readonly) NSInteger stage;
@property (nonatomic, readonly) float pressure;
@property (nonatomic, readonly) BOOL hasPreciseScrollingDeltas;
@property (nonatomic, readonly) CGFloat scrollingDeltaX;
@property (nonatomic, readonly) CGFloat scrollingDeltaY;
@end

@interface NSView (WebKit10_7Compat)
@property (nonatomic) NSUserInterfaceLayoutDirection userInterfaceLayoutDirection;
@property (nonatomic, readonly, copy) NSArray *constraints;
@end

@interface NSWindow (WebKit10_7Compat)
@property (nonatomic, readonly) NSInteger occlusionState;
- (NSRect)convertRectToScreen:(NSRect)rect;
@end

@interface NSColor (WebKit10_7Compat)
@property (nonatomic, readonly) CGColorRef CGColor;
@end

@interface NSFileManager (WebKit10_7Compat)
- (BOOL)createDirectoryAtURL:(NSURL *)url withIntermediateDirectories:(BOOL)createIntermediates attributes:(NSDictionary *)attributes error:(NSError **)error;
@end

#endif /* __MAC_OS_X_VERSION_MAX_ALLOWED < 1070 */

#endif /* APPKIT10_7_COMPAT_H */
