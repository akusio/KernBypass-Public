#import <Foundation/Foundation.h>
#include <stdio.h>
#include <stdint.h>
#include <dirent.h>

#include "config.h"
#include "kernel.h"
#include "vnode_utils.h"
#include "utils.h"

extern CFNotificationCenterRef CFNotificationCenterGetDistributedCenter(void);

bool change_rootvnode(uint64_t vp, pid_t pid) {
    
    if (!vp) return false;
    //printf("vp:%"PRIx64"\n",vp);

    uint64_t proc = proc_of_pid(pid);
    //printf("getting proc_t:%"PRIx64"\n",proc);

    if (!proc) return false;

    uint64_t filedesc = kernel_read64(proc + off_p_pfd);
    //printf("reading pfd:%"PRIx64"\n",filedesc);

    kernel_write64(filedesc + off_fd_cdir, vp);
    //printf("writing fd_cdir:%"PRIx64"\n",(filedesc + off_fd_cdir));

    kernel_write64(filedesc + off_fd_rdir, vp);
    //printf("writing fd_rdir:%"PRIx64"\n",(filedesc + off_fd_rdir));

    uint32_t fd_flags = kernel_read32(filedesc + 0x58);
    //printf("setting up fd_flags:%"PRIx64"\n",filedesc + 0x58);

    fd_flags |= 1; // FD_CHROOT = 1;
    
    kernel_write32(filedesc + 0x58, fd_flags);
    //printf("finish fd_flags:%"PRIx32"\n",fd_flags);
    return true;
    
}

void receive_notify_chrooter(CFNotificationCenterRef center,
                             void * observer,
                             CFStringRef name,
                             const void * object,
                             CFDictionaryRef userInfo) {

    NSDictionary *info = (__bridge NSDictionary*)userInfo;
    
    NSLog(@"receive notify %@", info);
    
    pid_t pid = [info[@"Pid"] intValue];
    
    uint64_t rootvp = get_vnode_with_chdir(FAKEROOTDIR);

    change_rootvnode(rootvp, pid);

    set_vnode_usecount(rootvp, 0x2000, 0x2000);
    
    usleep(100000);
    
    kill(pid, SIGCONT);
}

int main(int argc, char *argv[], char *envp[]) {
    
    int err = init_kernel();
    if (err) {
        printf("error init_kernel\n");
        return 1;
    }
    
    if (is_empty(FAKEROOTDIR) || access(FAKEROOTDIR"/private/var/containers", F_OK) != 0) {
        printf("error fakeroot not mounted\n");
        return 1;
    }

    CFNotificationCenterAddObserver(CFNotificationCenterGetDistributedCenter(),
                                    NULL,
                                    receive_notify_chrooter,
                                    CFSTR(Notify_Chrooter),
                                    NULL,
                                    CFNotificationSuspensionBehaviorDeliverImmediately);

    printf("start changerootfs\n");

    CFRunLoopRun();

    return 1;
}
