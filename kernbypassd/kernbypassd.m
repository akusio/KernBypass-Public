#include <spawn.h>
#import <dlfcn.h>
#import <sys/sysctl.h>
#include "../config.h"

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


#define FLAG_PLATFORMIZE (1 << 1)

void platformize_me() {
    void *handle = dlopen("/usr/lib/libjailbreak.dylib", RTLD_LAZY);
    if (!handle) return;
    // Reset errors
    dlerror();
    typedef void (*fix_entitle_prt_t)(pid_t pid, uint32_t what);
    fix_entitle_prt_t ptr = (fix_entitle_prt_t)dlsym(handle, "jb_oneshot_entitle_now");

    const char *dlsym_error = dlerror();
    if (dlsym_error) return;

    ptr(getpid(), FLAG_PLATFORMIZE);
}

void patch_setuid() {
    void *handle = dlopen("/usr/lib/libjailbreak.dylib", RTLD_LAZY);
    if (!handle) return;
    // Reset errors
    dlerror();
    typedef void (*fix_setuid_prt_t)(pid_t pid);
    fix_setuid_prt_t ptr = (fix_setuid_prt_t)dlsym(handle, "jb_oneshot_fix_setuid_now");

    const char *dlsym_error = dlerror();
    if (dlsym_error) return;

    ptr(getpid());
}

int main(int argc, char **argv, char **envp) {
    @autoreleasepool {
        patch_setuid();
        platformize_me();
        setuid(0);
        if ((chdir("/")) < 0) {
            exit(EXIT_FAILURE);
        }

        if (isProcessRunning(@"changerootfs") == NO) {
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
                if (access(FAKEROOTDIR"/private/var/containers", F_OK) == 0) {
                    printf("preparerootfs already done\n");
                } else {
                    printf("/usr/bin/preparerootfs\n");
                    easy_spawn((const char *[]){"/usr/bin/preparerootfs", NULL});
                    sleep(3);
                }
                // changerootfs
                printf("/usr/bin/changerootfs &\n");
                easy_spawn((const char *[]){"/usr/bin/changerootfs", "&", NULL});

                printf("RUNNING DAEMON\n");

                CFNotificationCenterPostNotification(CFNotificationCenterGetDarwinNotifyCenter(), CFSTR(Notify_Alert), NULL, NULL, YES);
            } else {
                printf("%s FOLDER CREATED FAILED\n",FAKEROOTDIR);
                return 1;
            }
        } else if (access(changerootfsMem, F_OK) == 0) {
            // kill changerootfs
            printf("kill changerootfs\n");
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wdeprecated-declarations"
            system("killall -9 changerootfs");
            #pragma clang diagnostic pop
            remove(changerootfsMem);
            CFNotificationCenterPostNotification(CFNotificationCenterGetDarwinNotifyCenter(), CFSTR(Notify_Alert), NULL, NULL, YES);
        } else {
            printf("changerootfs already running\n");
        }
    }
    return 0;
}
