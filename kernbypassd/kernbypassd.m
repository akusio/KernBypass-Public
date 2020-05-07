#include <spawn.h>
#import <dlfcn.h>
#import <sys/sysctl.h>

#define FLAG_PLATFORMIZE (1 << 1)

static void easy_spawn(const char* args[]) {
    pid_t pid;
    int status;
    posix_spawn(&pid, args[0], NULL, NULL, (char* const*)args, NULL);
    waitpid(pid, &status, WEXITED);
}

void platformize_me() {
    void* handle = dlopen("/usr/lib/libjailbreak.dylib", RTLD_LAZY);
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
    void* handle = dlopen("/usr/lib/libjailbreak.dylib", RTLD_LAZY);
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
        if((chdir("/")) < 0) {
            exit(EXIT_FAILURE);
        }
        
        printf("/usr/bin/changerootfs &\n");
        easy_spawn((const char *[]){"/usr/bin/changerootfs", "&", NULL});
        
        sleep(3);
        
        printf("disown %%1\n");
        easy_spawn((const char *[]){"disown", "%1", NULL});
        
        printf("RUNNING DAEMON\n");
    }
	return 0;
}
