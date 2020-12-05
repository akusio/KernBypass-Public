#import "CCUIContentModuleContentViewController-Protocol.h"

@protocol CCUIContentModule <NSObject>
@property (nonatomic,readonly) UIViewController<CCUIContentModuleContentViewController> *contentViewController; 
@property (nonatomic,readonly) UIViewController *backgroundViewController; 
@optional
- (void)setContentModuleContext:(id)context;
- (UIViewController *)backgroundViewController;

@required
- (UIViewController<CCUIContentModuleContentViewController> *)contentViewController;

@end