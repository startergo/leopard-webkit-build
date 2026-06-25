/* [leopard] empty stub: framework soft-linking disabled on 10.6 */

/* [leopard] AVFoundation is 10.7+; stub the AVPlayerLayer class accessor to Nil so
   PlatformCALayerCocoa's video-layer checks compile and evaluate to "not a video layer". */
#ifdef __OBJC__
#ifdef __cplusplus
namespace PAL {
inline Class getAVPlayerLayerClass() { return Nil; }
}
#endif
#endif
