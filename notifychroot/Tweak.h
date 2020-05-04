@interface FBSSystemService : NSObject
+(instancetype)sharedService;
-(int)pidForApplication:(NSString *)bundleId;
@end

@interface SBApplication : NSObject
@property (nonatomic, readonly) NSString *bundleIdentifier;
@end
