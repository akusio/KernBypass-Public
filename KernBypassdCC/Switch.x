#import "headers/ControlCenterUIKit/CCUIToggleModule.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <firmware.h>
#include <spawn.h>
#include "../config.h"

#define IDENTIFIER @"jp.akusio.kernbypasscc"

static UIWindow *window = nil;
static UIAlertController *alertController;

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

static NSString *getPrefsUrlStringFromPathString(NSString *pathString) {
    NSArray *urlPathItems = [pathString componentsSeparatedByString:@"/"];

    NSString *urlString = [NSString stringWithFormat:@"prefs:root=%@", urlPathItems[0]];

    if (urlPathItems.count > 1) {
        urlString = [urlString stringByAppendingString:[NSString stringWithFormat:@"&path=%@", urlPathItems[1]]];

        if (urlPathItems.count > 2) {
            urlString = [urlString stringByAppendingString:[NSString stringWithFormat:@"/%@", urlPathItems[2]]];
        }
    }

    urlString = [urlString stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLFragmentAllowedCharacterSet]];

    return urlString;
}

static void openSettings() {
    NSString *urlString;
    NSString *settingsStr;

    BOOL PreferenceOrganizer2 = [[NSFileManager defaultManager] fileExistsAtPath:@"/Library/MobileSubstrate/DynamicLibraries/PreferenceOrganizer2.dylib"];
    NSString *PO2_PATH = @"/var/mobile/Library/Preferences/net.angelxwind.preferenceorganizer2.plist";
    NSDictionary *pref1 = [NSDictionary dictionaryWithContentsOfFile:PO2_PATH];
    BOOL ShowTweaks = pref1[@"ShowTweaks"] ? [pref1[@"ShowTweaks"] boolValue] : YES;

    BOOL Shuffle = [[NSFileManager defaultManager] fileExistsAtPath:@"/Library/MobileSubstrate/DynamicLibraries/shuffle.dylib"];
    NSString *SHU_PATH = @"/var/mobile/Library/Preferences/com.creaturecoding.shuffle.plist";
    NSDictionary *pref2 = [NSDictionary dictionaryWithContentsOfFile:SHU_PATH];
    BOOL kEnabled = pref2[@"kEnabled"] ? [pref2[@"kEnabled"] boolValue] : YES;

    if (PreferenceOrganizer2 && ShowTweaks) {
        NSString *tweaks = pref1[@"TweaksName"] ? [pref1[@"TweaksName"] copy] : @"Tweaks";
        if ([tweaks isEqualToString:@""]) tweaks = @"Tweaks";
        settingsStr = [NSString stringWithFormat:@"%@/KernBypass",tweaks];
        urlString = getPrefsUrlStringFromPathString(settingsStr);

        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 2.0 * NSEC_PER_SEC), dispatch_get_main_queue(), ^(void) {
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wdeprecated-declarations"
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:urlString]];
            #pragma clang diagnostic pop
        });
    } else if (Shuffle && kEnabled) {
        NSString *tweaks = pref2[@"kTweaksGroupName"] ? [pref2[@"kTweaksGroupName"] copy] : @"Tweaks";
        settingsStr = [NSString stringWithFormat:@"%@/KernBypass",tweaks];
        urlString = getPrefsUrlStringFromPathString(settingsStr);
    } else {
        urlString = getPrefsUrlStringFromPathString(@"KernBypass");
    }
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:urlString]];
    #pragma clang diagnostic pop
}

@interface SBControlCenterController
+ (id)sharedInstance;
- (BOOL)isVisible;
- (void)dismissAnimated:(BOOL)arg1;
// iOS 13
- (BOOL)isPresentedOrDismissing;
@end

@interface UIImage ()
+ (UIImage *)imageNamed:(NSString *)name inBundle:(NSBundle *)bundle;
@end

@interface KernBypassdCCLongPressGestureRecognizer : UILongPressGestureRecognizer
@end

@implementation KernBypassdCCLongPressGestureRecognizer
@end

@interface KernBypassdCC : CCUIToggleModule <UIGestureRecognizerDelegate>
@end

@implementation KernBypassdCC

- (UIImage *)iconGlyph {
    UIViewController *vc = [self respondsToSelector:@selector(contentViewController)] ? self.contentViewController:nil;
    if (vc) {
        KernBypassdCCLongPressGestureRecognizer *longTap = [[KernBypassdCCLongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPress:)];
        [longTap setNumberOfTapsRequired:0];
        [longTap setMinimumPressDuration:0.5f];
        [longTap setDelegate:self];
        for (UIGestureRecognizer *recognizer in vc.view.gestureRecognizers) {
            if (recognizer && [recognizer isKindOfClass:%c(KernBypassdCCLongPressGestureRecognizer)]) {
                [vc.view removeGestureRecognizer:recognizer];
            }
        }
        [vc.view addGestureRecognizer:longTap];
    }
	return [UIImage imageNamed:@"ccIcon" inBundle:[NSBundle bundleForClass:[self class]]];
}

- (UIColor *)selectedColor {
	return [UIColor colorWithRed:0.00 green:0.13 blue:0.34 alpha:1.0];
}

- (BOOL)isSelected {
    return isProcessRunning(@"changerootfs");
}

- (void)setSelected:(BOOL)selected {
    // Alert Memo for CC
    FILE *fp = fopen(alertMem, "w");
    fclose(fp);

    NSMutableDictionary *mutableDict = [[NSMutableDictionary alloc] initWithContentsOfFile:PREF_PATH]?:[NSMutableDictionary dictionary];

    if (isProcessRunning(@"changerootfs")) {
        // Create check file
        FILE *kill = fopen(changerootfsMem, "w");
        fclose(kill);
        // autoEnabled disabled
        [mutableDict setObject:@NO forKey:@"autoEnabled"];
    } else {
        [mutableDict setObject:@YES forKey:@"autoEnabled"];
    }
    // Write settings path
    [mutableDict writeToFile:PREF_PATH atomically:YES];
    // Settings change notification
    CFNotificationCenterPostNotification(CFNotificationCenterGetDarwinNotifyCenter(), CFSTR(Notify_Preferences), NULL, NULL, YES);
    // Kill Preferences
    easy_spawn((const char *[]){"/usr/bin/killall", "Preferences", NULL});
    // Run kernbypassd
    easy_spawn((const char *[]){"/usr/bin/kernbypassd", NULL});

    // Close ControlCenter
    if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_13_0_beta1 && [[%c(SBControlCenterController) sharedInstance] isPresentedOrDismissing]) {
        [[%c(SBControlCenterController) sharedInstance] dismissAnimated:YES];
    } else if (kCFCoreFoundationVersionNumber < kCFCoreFoundationVersionNumber_iOS_13_0_beta1 && [[%c(SBControlCenterController) sharedInstance] isVisible]) {
        [[%c(SBControlCenterController) sharedInstance] dismissAnimated:YES];
    }
    
	[super refreshState];
}

- (void)longPress:(KernBypassdCCLongPressGestureRecognizer *)recognizer {
    if (recognizer.state == UIGestureRecognizerStateBegan) {
        openSettings();
        [super refreshState];
    }
}
@end

#pragma mark - CC Long gesture
@interface CCUIContentModuleContainerView : UIView {
    NSString *_moduleIdentifier;
}
- (UIView *)containerView;
- (void)layoutSubviews;
- (NSString *)moduleIdentifier;
@end

%hook CCUIContentModuleContainerView
- (void)layoutSubviews {
    %orig;
    NSString *identifier = [self valueForKey:@"_moduleIdentifier"];
    if ([identifier isEqualToString:IDENTIFIER] &&
        kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_13_0) {
        UILongPressGestureRecognizer *KernBypassdCCLongPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(KernBypassdCCLongPress:)];
        
        KernBypassdCCLongPress.minimumPressDuration = 0.5f;
        
        [self addGestureRecognizer:KernBypassdCCLongPress];
    }
}
%new
- (void)KernBypassdCCLongPress:(UILongPressGestureRecognizer *)sender {
    if (sender.state == UIGestureRecognizerStateBegan) {
        openSettings();
    }
}
%end

static void toggleChanged(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo) {
    if (access(alertMem, F_OK) == 0) {
        remove(alertMem);
        if (isProcessRunning(@"changerootfs")) {
            alertController =
            [UIAlertController alertControllerWithTitle:nil
                                                message:@"Enabled KernBypass"
                                         preferredStyle:UIAlertControllerStyleAlert];
        } else {
            alertController =
            [UIAlertController alertControllerWithTitle:nil
                                                message:@"Disabled KernBypass"
                                         preferredStyle:UIAlertControllerStyleAlert];
        }
        window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        window.windowLevel = UIWindowLevelAlert;

        [window makeKeyAndVisible];
        window.rootViewController = [[UIViewController alloc] init];
        UIViewController *vc = window.rootViewController;

        UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
            window.hidden = YES;
            window = nil;
        }];

        [alertController addAction:okAction];

        [vc presentViewController:alertController animated:YES completion:nil];
    }
}

__attribute__((constructor)) static void init() {
    @autoreleasepool {
        CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                        NULL,
                                        toggleChanged,
                                        CFSTR(Notify_Alert),
                                        NULL,
                                        CFNotificationSuspensionBehaviorCoalesce);
    }
}
