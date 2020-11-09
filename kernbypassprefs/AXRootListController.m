#import <UIKit/UIKit.h>
#import "Preferences.h"
#include <spawn.h>
#include "../config.h"

static int actions;
static UIAlertController *alertController;

static void easy_spawn(const char * args[]) {
    pid_t pid;
    int status;
    posix_spawn(&pid, args[0], NULL, NULL, (char * const*)args, NULL);
    waitpid(pid, &status, WEXITED);
}

#if __cplusplus
extern "C" {
#endif
    CFSetRef SBSCopyDisplayIdentifiers();
    NSString *SBSCopyLocalizedApplicationNameForDisplayIdentifier(NSString *identifier);
    mach_port_t SBSSpringBoardServerPort();
    int SBBundlePathForDisplayIdentifier(mach_port_t port, const char *identifier, char *path);
#if __cplusplus
}
#endif

#pragma mark - iOS 12 or higher
@interface PSAppDataUsagePolicyCache : NSObject
+ (id)sharedInstance;
- (id)fetchUsagePolicyFor:(id)arg1;
- (void)willEnterForeground;
@end

@interface AXRootListController : PSListController
- (NSArray *)specifiers;
- (NSDictionary*)trimDataSource:(NSDictionary*)dataSource;
- (NSDictionary*)userApplications:(NSDictionary*)app;
- (NSDictionary*)sortedDictionary:(NSDictionary*)dict;
@end

@implementation AXRootListController
- (instancetype)init {
    self = [super init];
    // Notifications
    CFNotificationCenterRemoveObserver(CFNotificationCenterGetDarwinNotifyCenter(), &actions, CFSTR(Notify_Alert), NULL);
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"kernbypassAlertButton" object:nil];

    CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(), &actions, (CFNotificationCallback)kernbypassAlertButtonCallBack, CFSTR(Notify_Alert), NULL, CFNotificationSuspensionBehaviorDeliverImmediately);
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(kernbypassAlertButton:) name:@"kernbypassAlertButton" object:nil];

    return self;
}

void kernbypassAlertButtonCallBack () {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"kernbypassAlertButton" object:nil];
}

- (void)kernbypassAlertButton:(NSNotification *)notification {
    if (access(kernbypassMem, F_OK) == 0) {
        alertController =
        [UIAlertController alertControllerWithTitle:nil
                                            message:@"Enabled KernBypass"
                                     preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"OK"
                                                            style:UIAlertActionStyleDefault
                                                          handler:^(UIAlertAction *action) {
                                                          }]];
    } else {
        alertController =
        [UIAlertController alertControllerWithTitle:nil
                                            message:@"Disabled KernBypass"
                                     preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"OK"
                                                            style:UIAlertActionStyleDefault
                                                          handler:^(UIAlertAction *action) {
                                                          }]];
    }
    [self presentViewController:alertController animated:YES completion:nil];
    [self reloadSpecifiers];
}

- (NSArray *)specifiers {
	if (!_specifiers) {
        NSMutableArray *specifiers = [NSMutableArray array];
        PSSpecifier *spec;

        spec = [PSSpecifier emptyGroupSpecifier];
        [specifiers addObject:spec];

        spec = [PSSpecifier preferenceSpecifierNamed:@"kernbypassd"
                                              target:self
                                                 set:@selector(setPreferenceValue:specifier:)
                                                 get:@selector(readPreferenceValue:)
                                              detail:Nil
                                                cell:PSSwitchCell
                                                edit:Nil];
        [spec setProperty:@"autoEnabled" forKey:@"key"];
        [spec setProperty:@NO forKey:@"default"];
        [spec setProperty:NSClassFromString(@"PSSubtitleSwitchTableCell") forKey:@"cellClass"];
        [spec setProperty:@"Automatically runs the command when enabled" forKey:@"cellSubtitleText"];
        [specifiers addObject:spec];

        if (access(kernbypassMem, F_OK) != 0) {
            spec = [PSSpecifier preferenceSpecifierNamed:@"Enable KernBypass"
                                                  target:self
                                                     set:NULL
                                                     get:NULL
                                                  detail:Nil
                                                    cell:PSButtonCell
                                                    edit:Nil];
            spec->action = @selector(tapEnable);
            [spec setProperty:@"tapEnable" forKey:@"key"];
            [specifiers addObject:spec];
        } else {
            spec = [PSSpecifier preferenceSpecifierNamed:@"Disable KernBypass"
                                                  target:self
                                                     set:NULL
                                                     get:NULL
                                                  detail:Nil
                                                    cell:PSButtonCell
                                                    edit:Nil];
            spec->action = @selector(tapDisable);
            [spec setProperty:@"tapDisable" forKey:@"key"];
            [specifiers addObject:spec];
        }

        spec = [PSSpecifier preferenceSpecifierNamed:@"Enabled Applications"
                                              target:self
                                                 set:NULL
                                                 get:NULL
                                              detail:Nil
                                                cell:PSGroupCell
                                                edit:Nil];
        [specifiers addObject:spec];

        PSAppDataUsagePolicyCache *cache = [NSClassFromString(@"PSAppDataUsagePolicyCache") sharedInstance];
        [cache willEnterForeground];

        NSArray *displayIdentifiers = [(__bridge NSSet *)SBSCopyDisplayIdentifiers() allObjects];

        NSMutableDictionary *apps = [[NSMutableDictionary alloc] init];

        for (NSString *appIdentifier in displayIdentifiers) {
            [cache fetchUsagePolicyFor:appIdentifier];
            NSString *appName = SBSCopyLocalizedApplicationNameForDisplayIdentifier(appIdentifier);
            if (appName) {
                [apps setObject:appName forKey:appIdentifier];
            }
        }

        NSDictionary *finalApps = [apps copy];
        finalApps = [self trimDataSource:finalApps];
        finalApps = [self userApplications:finalApps];
        finalApps = [self sortedDictionary:finalApps];

        NSMutableArray *applicationSpecifiers = [NSMutableArray new];

        for (NSString *displayIdentifier in finalApps.allKeys) {
            NSString *displayName = finalApps[displayIdentifier];

            spec = [PSSpecifier preferenceSpecifierNamed:displayName
                                                  target:self
                                                     set:@selector(setPreferenceValue:specifier:)
                                                     get:@selector(readPreferenceValue:)
                                                  detail:nil
                                                    cell:PSSwitchCell
                                                    edit:nil];
            [spec setProperty:displayIdentifier forKey:@"appIDForLazyIcon"];
            [spec setProperty:@YES forKey:@"useLazyIcons"];
            [spec setProperty:displayIdentifier forKey:@"key"];
            [spec setProperty:@NO forKey:@"default"];
            [applicationSpecifiers addObject:spec];
        }

        NSSortDescriptor *sort = [[NSSortDescriptor alloc] initWithKey:@"name" ascending:YES selector:@selector(caseInsensitiveCompare:)];
        [applicationSpecifiers sortUsingDescriptors:[NSArray arrayWithObject:sort]];
        [specifiers addObjectsFromArray:applicationSpecifiers];

        _specifiers = [specifiers copy];
	}

	return _specifiers;
}

- (NSDictionary*)trimDataSource:(NSDictionary*)dataSource {
    NSMutableDictionary *mutableDict = [dataSource mutableCopy];

    NSArray *filterdIdentifiers = [[NSArray alloc] initWithObjects:
                                   @"com.apple.AdSheet",
                                   @"com.apple.AdSheetPhone",
                                   @"com.apple.AdSheetPad",
                                   @"com.apple.DataActivation",
                                   @"com.apple.DemoApp",
                                   @"com.apple.fieldtest",
                                   @"com.apple.iosdiagnostics",
                                   @"com.apple.iphoneos.iPodOut",
                                   @"com.apple.TrustMe",
                                   @"com.apple.WebSheet",
                                   @"com.apple.springboard",
                                   @"com.apple.purplebuddy",
                                   @"com.apple.datadetectors.DDActionsService",
                                   @"com.apple.FacebookAccountMigrationDialog",
                                   @"com.apple.iad.iAdOptOut",
                                   @"com.apple.ios.StoreKitUIService",
                                   @"com.apple.TextInput.kbd",
                                   @"com.apple.MailCompositionService",
                                   @"com.apple.mobilesms.compose",
                                   @"com.apple.quicklook.quicklookd",
                                   @"com.apple.ShoeboxUIService",
                                   @"com.apple.social.remoteui.SocialUIService",
                                   @"com.apple.WebViewService",
                                   @"com.apple.gamecenter.GameCenterUIService",
                                   @"com.apple.appleaccount.AACredentialRecoveryDialog",
                                   @"com.apple.CompassCalibrationViewService",
                                   @"com.apple.WebContentFilter.remoteUI.WebContentAnalysisUI",
                                   @"com.apple.PassbookUIService",
                                   @"com.apple.uikit.PrintStatus",
                                   @"com.apple.Copilot",
                                   @"com.apple.MusicUIService",
                                   @"com.apple.AccountAuthenticationDialog",
                                   @"com.apple.MobileReplayer",
                                   @"com.apple.SiriViewService",
                                   @"com.apple.TencentWeiboAccountMigrationDialog",
                                   @"com.apple.AskPermissionUI",
                                   @"com.apple.Diagnostics",
                                   @"com.apple.GameController",
                                   @"com.apple.HealthPrivacyService",
                                   @"com.apple.InCallService",
                                   @"com.apple.mobilesms.notification",
                                   @"com.apple.PhotosViewService",
                                   @"com.apple.PreBoard",
                                   @"com.apple.PrintKit.Print-Center",
                                   @"com.apple.SharedWebCredentialViewService",
                                   @"com.apple.share",
                                   @"com.apple.CoreAuthUI",
                                   @"com.apple.webapp",
                                   @"com.apple.webapp1",
                                   @"com.apple.family",
                                   nil];
    for (NSString *key in filterdIdentifiers) {
        [mutableDict removeObjectForKey:key];
    }

    return [mutableDict copy];
}

- (NSDictionary*)userApplications:(NSDictionary*)app {
    NSMutableDictionary *mutableDict = [app mutableCopy];

    char path[1024];

    for (NSString *display in app.allKeys) {
        if (SBBundlePathForDisplayIdentifier(SBSSpringBoardServerPort(),[display UTF8String], path) == 0) {
            NSDictionary *info = [NSDictionary dictionaryWithContentsOfFile:[[NSString stringWithUTF8String:path] stringByAppendingString:@"/Info.plist"]];
            if (info != nil) {
                NSString *appPath = [NSString stringWithFormat:@"%s",path];
                if (![appPath hasPrefix:@"/private/"] || [info[@"CFBundleIdentifier"] hasPrefix:@"com.apple"]) {
                    [mutableDict removeObjectForKey:display];
                }
            }
        }
    }

    return [mutableDict copy];
}

- (NSDictionary*)sortedDictionary:(NSDictionary*)dict {
    NSArray *sortedValues;
    NSMutableDictionary *mutableDict = [NSMutableDictionary dictionary];

    sortedValues = [[dict allValues] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];

    for (NSString *value in sortedValues) {
        NSString *key = [[dict allKeysForObject:value] objectAtIndex:0];
        [mutableDict setObject:value forKey:key];
    }

    return [mutableDict copy];
}

- (void)setPreferenceValue:(id)value specifier:(PSSpecifier *)specifier {
    @autoreleasepool {
        NSMutableDictionary *EnablePrefsCheck = [[NSMutableDictionary alloc] initWithContentsOfFile:PREF_PATH]?:[NSMutableDictionary dictionary];
        [EnablePrefsCheck setObject:value forKey:[specifier identifier]];
        [EnablePrefsCheck writeToFile:PREF_PATH atomically:YES];

        CFNotificationCenterPostNotification(CFNotificationCenterGetDarwinNotifyCenter(), CFSTR(Notify_Preferences), NULL, NULL, YES);
    }
}

- (id)readPreferenceValue:(PSSpecifier*)specifier {
    @autoreleasepool {
        NSDictionary *EnablePrefsCheck = [[NSDictionary alloc] initWithContentsOfFile:PREF_PATH];
        return EnablePrefsCheck[[specifier identifier]]?:[[specifier properties] objectForKey:@"default"];
    }
}

- (void)tapEnable {
    alertController =
    [UIAlertController alertControllerWithTitle:nil
                                        message:@"Enable KernBypass"
                                 preferredStyle:UIAlertControllerStyleAlert];

    [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel"
                                                        style:UIAlertActionStyleDefault
                                                      handler:^(UIAlertAction *action) {}]];

    [alertController addAction:[UIAlertAction actionWithTitle:@"YES"
                                                        style:UIAlertActionStyleDefault
                                                      handler:^(UIAlertAction *action) {
        NSMutableDictionary *mutableDict = [[NSMutableDictionary alloc] initWithContentsOfFile:PREF_PATH]?:[NSMutableDictionary dictionary];
        [mutableDict setObject:@YES forKey:@"autoEnabled"];
        [mutableDict writeToFile:PREF_PATH atomically:YES];

        CFNotificationCenterPostNotification(CFNotificationCenterGetDarwinNotifyCenter(), CFSTR(Notify_Preferences), NULL, NULL, YES);
        easy_spawn((const char *[]){"/usr/bin/kernbypassd", NULL});
    }]];
    [self presentViewController:alertController animated:YES completion:nil];
}

- (void)tapDisable {
    alertController =
    [UIAlertController alertControllerWithTitle:nil
                                        message:@"Disable KernBypass"
                                 preferredStyle:UIAlertControllerStyleAlert];

    [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel"
                                                        style:UIAlertActionStyleDefault
                                                      handler:^(UIAlertAction *action) {}]];

    [alertController addAction:[UIAlertAction actionWithTitle:@"YES"
                                                        style:UIAlertActionStyleDefault
                                                      handler:^(UIAlertAction *action) {
        NSMutableDictionary *mutableDict = [[NSMutableDictionary alloc] initWithContentsOfFile:PREF_PATH]?:[NSMutableDictionary dictionary];
        [mutableDict setObject:@NO forKey:@"autoEnabled"];
        [mutableDict writeToFile:PREF_PATH atomically:YES];

        FILE *fp = fopen(changerootfsMem, "w");
        fclose(fp);

        CFNotificationCenterPostNotification(CFNotificationCenterGetDarwinNotifyCenter(), CFSTR(Notify_Preferences), NULL, NULL, YES);

        easy_spawn((const char *[]){"/usr/bin/kernbypassd", NULL});
    }]];
    [self presentViewController:alertController animated:YES completion:nil];
}
@end
