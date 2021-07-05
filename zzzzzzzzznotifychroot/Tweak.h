@interface FBSSystemService : NSObject
+(instancetype)sharedService;
-(int)pidForApplication:(NSString *)bundleId;
@end

@interface RBSProcessIdentity
@property (nonatomic, readonly) NSString *embeddedApplicationIdentifier;
@end

@interface FBProcessExecutionContext
@property (nonatomic, assign) NSDictionary *environment;
@property (nonatomic, assign) RBSProcessIdentity *identity;
@end

@interface FBApplicationProcess
@property (nonatomic, assign) FBProcessExecutionContext *executionContext;
@end
