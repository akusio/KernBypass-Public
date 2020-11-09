#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#include <spawn.h>
#include "../config.h"

#define rebootMem "/var/mobile/kernbypassReboot"

static UIWindow *window = nil;
static BOOL autoEnabled;

static void easy_spawn(const char *args[]) {
    pid_t pid;
    int status;
    posix_spawn(&pid, args[0], NULL, NULL, (char * const*)args, NULL);
    waitpid(pid, &status, WEXITED);
}

@interface FBSSystemService : NSObject
+ (instancetype)sharedService;
- (int)pidForApplication:(NSString *)bundleId;
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

extern CFNotificationCenterRef CFNotificationCenterGetDistributedCenter(void);

BOOL isEnableApplication(NSString *bundleID) {
    NSDictionary *pref = [NSDictionary dictionaryWithContentsOfFile:PREF_PATH];

    if (!pref || pref[bundleID] == nil) {
        return NO;
    }

    return [pref[bundleID] boolValue];
}

void bypassApplication(NSString *bundleID) {
    int pid = [[%c(FBSSystemService) sharedService] pidForApplication:bundleID];

    if (isEnableApplication(bundleID) && pid != -1) {
        NSDictionary *info = @{
            @"Pid" : [NSNumber numberWithInt:pid]
        };

        CFNotificationCenterPostNotification(CFNotificationCenterGetDistributedCenter(), CFSTR(Notify_Chrooter), NULL, (__bridge CFDictionaryRef)info, YES);

        kill(pid, SIGSTOP);
    }
}

%group SpringBoardHook

%hook SpringBoard
- (void)applicationDidFinishLaunching:(id)arg1 {
    %orig;
    // Automatically enabled on Reboot and Re-Jailbreak etc
    if (autoEnabled && access(kernbypassMem, F_OK) != 0) {
        easy_spawn((const char *[]){"/usr/bin/kernbypassd", NULL});
    }
    // Alert prompting for Reboot when using previous version
    if ([[NSFileManager defaultManager] removeItemAtPath:@rebootMem error:nil]) {
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"KernByPass Unofficial"
                                                                       message:@"[Note] Please reboot before Enable!!"
                                                                preferredStyle:UIAlertControllerStyleAlert];

        window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        window.windowLevel = UIWindowLevelAlert;

        [window makeKeyAndVisible];
        window.rootViewController = [[UIViewController alloc] init];
        UIViewController *vc = window.rootViewController;

        UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
            window.hidden = YES;
            window = nil;
        }];

        [alert addAction:okAction];

        [vc presentViewController:alert animated:YES completion:nil];
    }
}
%end

%hook FBApplicationProcess
- (void)launchWithDelegate:(id)delegate {
    NSDictionary *env = self.executionContext.environment;
    %orig;
    // Choicy compatible
    if (env[@"_MSSafeMode"] || env[@"_SafeMode"]) {
        bypassApplication(self.executionContext.identity.embeddedApplicationIdentifier);
    }
}
%end

%end // SpringBoardHook End

static void settingsChanged(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo) {
    NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:PREF_PATH];
    autoEnabled = (BOOL)[dict[@"autoEnabled"] ?: @NO boolValue];
}

%ctor {
    // Settings Notifications
    CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                    NULL,
                                    settingsChanged,
                                    CFSTR(Notify_Preferences),
                                    NULL,
                                    CFNotificationSuspensionBehaviorCoalesce);

    settingsChanged(NULL, NULL, NULL, NULL, NULL);

    NSString *identifier = [[NSBundle mainBundle] bundleIdentifier];

    if ([identifier isEqualToString:@"com.apple.springboard"]) {
        %init(SpringBoardHook);
    } else {
        bypassApplication(identifier);
    }
}
