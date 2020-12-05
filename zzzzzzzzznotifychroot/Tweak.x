#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#include <spawn.h>
#include "../config.h"

static UIWindow *window = nil;
static BOOL autoEnabled;

static void easy_spawn(const char *args[]) {
    pid_t pid;
    int status;
    posix_spawn(&pid, args[0], NULL, NULL, (char * const*)args, NULL);
    waitpid(pid, &status, WEXITED);
}

static BOOL isProcessRunning(NSString *processName) {
    BOOL running = NO;

    NSString *command = [NSString stringWithFormat:@"ps ax | grep %@ | grep -v grep | wc -l", processName];

    FILE *pf;
    char data[512];

    pf = popen([command cStringUsingEncoding:NSASCIIStringEncoding],"r");

    if (!pf) {
        fprintf(stderr, "Could not open pipe for output.\n");
        return NO;
    }

    fgets(data, 512, pf);

    int val = (int)[[NSString stringWithUTF8String:data] integerValue];
    if (val != 0) {
        running = YES;
    }

    if (pclose(pf) != 0) {
        fprintf(stderr," Error: Failed to close command stream \n");
    }

    return running;
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
    if (autoEnabled && isProcessRunning(@"changerootfs") == NO) {
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
    // Notification from Settings (Only work enable) // dirty code
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(runKernBypassd:) name:@Notify_KernBypassd object:nil];
}
%new
- (void)runKernBypassd:(NSNotification *)notification {
    if (isProcessRunning(@"changerootfs") == NO) {
        // run kernbypassd (dirty code)
        // Runs on SpringBoard because the changerootfs will be killed when called from settings
        // (Disabled after restart of SpringBoard)
        easy_spawn((const char *[]){"/usr/bin/kernbypassd", NULL});
    }
}
%end

%hook FBApplicationProcess
- (void)launchWithDelegate:(id)delegate {
    NSDictionary *env = self.executionContext.environment;
    %orig;
    // Choicy compatible? Note:It doesn't work
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

static void callKernBypassd(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo) {
    NSString *identifier = [[NSBundle mainBundle] bundleIdentifier];
    if ([identifier isEqualToString:@"com.apple.springboard"]) {
        // dirty code
        [[NSNotificationCenter defaultCenter] postNotificationName:@Notify_KernBypassd object:nil userInfo:nil];
    }
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

    // Call KernBypassd Notifications
    CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                    NULL,
                                    callKernBypassd,
                                    CFSTR(Notify_KernBypassd),
                                    NULL,
                                    CFNotificationSuspensionBehaviorCoalesce);

    NSString *identifier = [[NSBundle mainBundle] bundleIdentifier];

    if ([identifier isEqualToString:@"com.apple.springboard"]) {
        %init(SpringBoardHook);
    } else {
        bypassApplication(identifier);
    }
}
