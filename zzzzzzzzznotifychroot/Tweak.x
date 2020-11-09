#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import "Tweak.h"
#include <spawn.h>

#define kCFCoreFoundationVersionNumber_iOS_12_0 1556.00

#define PLIST_PATH @"/var/mobile/Library/Preferences/jp.akusio.kernbypass2.plist"
#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)

static void easy_spawn(const char* args[]){
    pid_t pid;
    int status;
    posix_spawn(&pid, args[0], NULL, NULL, (char* const*)args, NULL);
    waitpid(pid, &status, WEXITED);
}

extern CFNotificationCenterRef CFNotificationCenterGetDistributedCenter(void);

BOOL isEnableApplication(NSString *bundleID){
    NSDictionary* pref = [NSDictionary dictionaryWithContentsOfFile:PLIST_PATH];
    if(!pref || pref[bundleID] == nil){
        return NO;
    }
    BOOL ret = [pref[bundleID] boolValue];
    return ret;
}

void bypassApplication(NSString *bundleID){
    int pid = [[%c(FBSSystemService) sharedService] pidForApplication:bundleID];
    if(!isEnableApplication(bundleID) || pid == -1){
        return;
    }
    NSDictionary* info = @{
        @"Pid" : [NSNumber numberWithInt:pid]
    };
    CFNotificationCenterPostNotification(CFNotificationCenterGetDistributedCenter(), (__bridge CFStringRef)@"jp.akusio.chrooter", NULL, (__bridge CFDictionaryRef)info, YES);
    kill(pid, SIGSTOP);
}

%group SpringBoardHook

%hook SpringBoard
- (void)applicationDidFinishLaunching:(id)arg1{
    %orig;
    // Automatically enabled on ldrestart and Re-Jailbreak
    easy_spawn((const char *[]){"/usr/bin/kernbypassd", NULL});
}
%end

%hook FBApplicationProcess
- (void)launchWithDelegate:(id)delegate{
    NSDictionary *env = self.executionContext.environment;
    %orig;
    if(env[@"_MSSafeMode"] || env[@"_SafeMode"])
        bypassApplication(self.executionContext.identity.embeddedApplicationIdentifier);
}
%end

%end // SpringBoardHook End

%ctor{
    if(IN_SPRINGBOARD)
        %init(SpringBoardHook);
    else
        bypassApplication([NSBundle mainBundle].bundleIdentifier);
}
