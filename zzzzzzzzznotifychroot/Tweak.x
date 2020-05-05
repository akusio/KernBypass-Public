#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import "Tweak.h"

#define PLIST_PATH @"/var/mobile/Library/Preferences/jp.akusio.kernbypass.plist"
#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)

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

%group SB

%hook FBApplicationProcess

-(void)launchWithDelegate:(id)delegate{
    NSDictionary *env = self.executionContext.environment;
    %orig;
    if(env[@"_MSSafeMode"] || env[@"_SafeMode"])
        bypassApplication(self.executionContext.identity.embeddedApplicationIdentifier);
}

%end

%end

%ctor{
    if(IN_SPRINGBOARD)
        %init(SB);
    else
        bypassApplication([NSBundle mainBundle].bundleIdentifier);
}