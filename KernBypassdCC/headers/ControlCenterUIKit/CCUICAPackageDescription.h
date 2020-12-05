@interface CCUICAPackageDescription : NSObject
@property (nonatomic, copy, readonly) NSURL *packageURL;
@property (assign, nonatomic) BOOL flipsForRightToLeftLayoutDirection;
+ (instancetype)descriptionForPackageNamed:(NSString *)name inBundle:(NSBundle *)bundle;
- (BOOL)flipsForRightToLeftLayoutDirection;
- (CCUICAPackageDescription *)initWithPackageName:(NSString *)name inBundle:(NSBundle *)bundle;
- (void)setFlipsForRightToLeftLayoutDirection:(BOOL)flips;
- (NSURL *)packageURL;
@end