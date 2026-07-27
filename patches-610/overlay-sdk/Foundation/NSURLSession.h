/*
 * NSURLSession.h stub for Mac OS X 10.6 SDK compatibility.
 * NSURLSession was introduced in 10.9.
 */
#import <Foundation/Foundation.h>

#ifndef NSURLSESSION_STUB_H
#define NSURLSESSION_STUB_H

enum {
    NSURLSessionAuthChallengeUseCredential = 0,
    NSURLSessionAuthChallengePerformDefaultHandling = 1,
    NSURLSessionAuthChallengeCancelAuthenticationChallenge = 2,
    NSURLSessionAuthChallengeRejectProtectionSpace = 3,
};
typedef NSUInteger NSURLSessionAuthChallengeDisposition;

enum {
    NSURLSessionResponseCancel = 0,
    NSURLSessionResponseAllow = 1,
    NSURLSessionResponseBecomeDownload = 2,
    NSURLSessionResponseBecomeStream = 3,
};
typedef NSUInteger NSURLSessionResponsePolicy;
// [leopard-webkit-build] The real type name (10.9+) is Disposition, not Policy;
// WebCoreNSURLSession.mm uses NSURLSessionResponseDisposition. Provide both.
typedef NSUInteger NSURLSessionResponseDisposition;
// NSURLSessionTaskPriorityDefault/Low/High — float constants (10.9+); WebCoreNSURLSession.mm
// references Default for the initial task priority.
#ifndef NSURLSessionTaskPriorityDefault
#define NSURLSessionTaskPriorityDefault 0.0f
#endif
#ifndef NSURLSessionTaskPriorityLow
#define NSURLSessionTaskPriorityLow 0.25f
#endif
#ifndef NSURLSessionTaskPriorityHigh
#define NSURLSessionTaskPriorityHigh 1.0f
#endif

/* [leopard-webkit-build] NSURLSessionTaskState — consumed by WebCoreNSURLSession's
 * task @property; absent from the 10.6 SDK (NSURLSession is 10.9+). */
enum {
    NSURLSessionTaskStateSuspended = 0,
    NSURLSessionTaskStateRunning = 1,
    NSURLSessionTaskStateCanceling = 2,
    NSURLSessionTaskStateCompleted = 3,
};
typedef NSInteger NSURLSessionTaskState;

@class NSURLSessionConfiguration;

@interface NSURLSessionConfiguration : NSObject <NSCopying>
+ (NSURLSessionConfiguration *)defaultSessionConfiguration;
+ (NSURLSessionConfiguration *)ephemeralSessionConfiguration;
@end

@interface NSURLSession : NSObject
+ (NSURLSession *)sessionWithConfiguration:(NSURLSessionConfiguration *)configuration;
+ (NSURLSession *)sharedSession;
@end

@interface NSURLSessionTask : NSObject
@property (readonly) NSUInteger taskIdentifier;
- (void)cancel;
- (void)resume;
@end

@interface NSURLSessionDataTask : NSURLSessionTask
@end

/* [leopard-webkit-build] NSURLSession is 10.9+; these are stub protocols so the
 * dead-on-10.6 WebCoreNSURLSession wrapper compiles. The inheritance mirrors real
 * NSURLSession (TaskDelegate <: Delegate, DataDelegate <: TaskDelegate) so that
 * `id<NSURLSessionTaskDelegate>` is assignable to `id<NSURLSessionDelegate>` and a
 * class-extension property typed as TaskDelegate matches the primary class's Delegate. */
@protocol NSURLSessionDelegate <NSObject>
@end

@protocol NSURLSessionTaskDelegate <NSURLSessionDelegate>
@end

@protocol NSURLSessionDataDelegate <NSURLSessionTaskDelegate>
@end

@interface NSURLSessionUploadTask : NSURLSessionDataTask
@end

@interface NSURLSessionDownloadTask : NSURLSessionTask
@end

@interface NSURLSessionStreamTask : NSURLSessionTask
@end

#endif
