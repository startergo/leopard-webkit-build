@protocol NSObject
@end
@protocol NSURLConnectionDelegate <NSObject>
@optional
- (void)connection:(id)connection didFailWithError:(id)error;
- (void)connection:(id)connection didReceiveResponse:(id)response;
- (void)connection:(id)connection didReceiveData:(id)data;
- (void)connection:(id)connection willSendRequest:(id)request redirectResponse:(id)redirectResponse;
- (void)connectionDidFinishLoading:(id)connection;
@end
@interface NSObject <NSObject>
@end
@interface _NSURLConnectionDelegateProtocolHolder : NSObject <NSURLConnectionDelegate>
@end
@implementation _NSURLConnectionDelegateProtocolHolder
@end
