

@protocol CCUIContentModuleContentViewController <NSObject>
@property (nonatomic,readonly) CGFloat preferredExpandedContentHeight; 
@property (nonatomic,readonly) CGFloat preferredExpandedContentWidth; 
@property (nonatomic,readonly) BOOL providesOwnPlatter; 
@optional
- (BOOL)shouldBeginTransitionToExpandedContentModule;
- (void)willResignActive;
- (void)willBecomeActive;
- (void)willTransitionToExpandedContentMode:(BOOL)willTransition;
- (BOOL)shouldFinishTransitionToExpandedContentModule;
- (void)didTransitionToExpandedContentMode:(BOOL)didTransition;
- (BOOL)canDismissPresentedContent;
- (void)dismissPresentedContentAnimated:(BOOL)animated completion:(id)completion;
- (CGFloat)preferredExpandedContentWidth;
- (BOOL)providesOwnPlatter;
- (void)controlCenterWillPresent;
- (void)controlCenterDidDismiss;

@required
- (CGFloat)preferredExpandedContentHeight;

@end
