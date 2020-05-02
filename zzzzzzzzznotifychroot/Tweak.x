#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>

#define PLIST_PATH @"/var/mobile/Library/Preferences/jp.akusio.kernbypass.plist"

extern CFNotificationCenterRef CFNotificationCenterGetDistributedCenter(void);



BOOL isEnableApplication(){
    
    NSDictionary* pref = [NSDictionary dictionaryWithContentsOfFile:PLIST_PATH];
    
    NSString* bundleID = [[NSBundle mainBundle] bundleIdentifier];
    
    if(!pref || pref[bundleID] == nil){
        return NO;
    }
    
    BOOL ret = [pref[bundleID] boolValue];
    
    return ret;
    
}

%ctor{
    
    if(!isEnableApplication()){
        return;
    }
    
    NSDictionary* info = @{
                           @"Pid" : [NSNumber numberWithInt:getpid()]
                           };
    
    CFNotificationCenterPostNotification(CFNotificationCenterGetDistributedCenter(), (__bridge CFStringRef)@"jp.akusio.chrooter", NULL, (__bridge CFDictionaryRef)info, YES);
    
    kill(getpid(), SIGSTOP);
    
}
