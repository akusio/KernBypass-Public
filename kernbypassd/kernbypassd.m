#include <spawn.h>
#import <dlfcn.h>
#import <sys/sysctl.h>
#include "../utils.h"
#include "../config.h"

static BOOL autoEnabled;

static void easy_spawn(const char *args[]) {
    pid_t pid;
    int status;
    posix_spawn(&pid, args[0], NULL, NULL, (char * const*)args, NULL);
    waitpid(pid, &status, WEXITED);
}

int main(int argc, char **argv, char **envp) {
    @autoreleasepool {
        // root
        setuid(0);
        setgid(0);
        if (getuid() != 0 && getgid() != 0) {
            printf("Require kernbypassd to be run as root!\n");
            return -1;
        }

        if (autoEnabled && access(kernbypassMem, F_OK) != 0) {
            NSFileManager *manager = [NSFileManager defaultManager];
            NSString *path = @FAKEROOTDIR;

            if (![manager fileExistsAtPath:path]) {
                printf("%s FOLDER NOT FOUND\n",FAKEROOTDIR);
                [manager createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:nil];
                // chmod permissions 755
                [manager setAttributes:@{NSFilePosixPermissions:@00755}
                          ofItemAtPath:path error:nil];
                // chown root:wheel
                NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
                                          @"root",NSFileOwnerAccountName,
                                          @"wheel",NSFileGroupOwnerAccountName,
                                          nil];

                [manager setAttributes:dict ofItemAtPath:path error:nil];
                if ([manager fileExistsAtPath:path]) {
                    printf("%s FOLDER CREATED SUCCESS\n",FAKEROOTDIR);
                } else {
                    printf("%s FOLDER CREATED FAILED\n",FAKEROOTDIR);
                    return 1;
                }
            }

            if ([manager fileExistsAtPath:path]) {
                // preparerootfs
                if (!is_empty(FAKEROOTDIR) && access(FAKEROOTDIR"/private/var/containers", F_OK) == 0) {
                    printf("error already mounted\n");
                } else {
                    printf("/usr/bin/preparerootfs\n");
                    easy_spawn((const char *[]){"/usr/bin/preparerootfs", NULL});
                    sleep(1);
                }
                // changerootfs
                printf("/usr/bin/changerootfs &\n");
                easy_spawn((const char *[]){"/usr/bin/changerootfs", "&", NULL});

                sleep(1);

                printf("disown %%1\n");
                easy_spawn((const char *[]){"disown", "%1", NULL});

                printf("RUNNING DAEMON\n");

                CFNotificationCenterPostNotification(CFNotificationCenterGetDarwinNotifyCenter(), CFSTR(Notify_Alert), NULL, NULL, YES);
            } else {
                printf("%s FOLDER CREATED FAILED\n",FAKEROOTDIR);
                return 1;
            }
        } else if (access(changerootfsMem, F_OK) == 0) {
            // kill changerootfs
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
            system("killall changerootfs");
#pragma clang diagnostic pop
            remove(kernbypassMem);
            remove(changerootfsMem);
            CFNotificationCenterPostNotification(CFNotificationCenterGetDarwinNotifyCenter(), CFSTR(Notify_Alert), NULL, NULL, YES);
        } else {
            printf("Settings -> KernBypass, turn on \"kernbypassd\"\n");
        }
    }
	return 0;
}

static void settingsChanged(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo) {
    NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:PREF_PATH];
    autoEnabled = (BOOL)[dict[@"autoEnabled"] ?: @NO boolValue];
}

__attribute__((constructor)) static void init() {
    @autoreleasepool {
        // Settings Notifications
        CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                        NULL,
                                        settingsChanged,
                                        CFSTR(Notify_Preferences),
                                        NULL,
                                        CFNotificationSuspensionBehaviorCoalesce);

        settingsChanged(NULL, NULL, NULL, NULL, NULL);
    }
}
