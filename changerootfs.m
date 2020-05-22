#include <stdio.h>
#import <Foundation/Foundation.h>

#include "config.h"
#include "kernel.h"
#include "vnode_utils.h"

#include <dirent.h>

//#if 0
extern CFNotificationCenterRef CFNotificationCenterGetDistributedCenter(void);

bool change_rootvnode(uint64_t vp, pid_t pid){
    
    if(!vp) return false;
    
    printf("getting proc_t\n");
    uint64_t proc = proc_of_pid(pid);
    
    if(!proc) return false;
    
    printf("reading pfd\n");
    uint64_t filedesc = kernel_read64(proc + off_p_pfd);
    
    printf("writing fd_cdir\n");
    kernel_write64(filedesc + off_fd_cdir, vp);
    
    printf("writing fd_rdir\n");
    kernel_write64(filedesc + off_fd_rdir, vp);
    
    printf("setting up fd_flags\n");
    uint32_t fd_flags = kernel_read32(filedesc + 0x58);
    
    fd_flags |= 1; // FD_CHROOT = 1;
    
    kernel_write32(filedesc + 0x58, fd_flags);
    
    printf("finish\n");
    return true;
    
}

uint64_t rootvp;

void receive_notify_chrooter(CFNotificationCenterRef center,
                             void * observer,
                             CFStringRef name,
                             const void * object,
                             CFDictionaryRef userInfo){
    
    NSDictionary* info = (__bridge NSDictionary*)userInfo;
    
    NSLog(@"receive notify %@", info);
    
    pid_t pid = [info[@"Pid"] intValue];
    
    uint64_t rootvp = get_vnode_with_chdir(FAKEROOTDIR);
    set_vnode_usecount(rootvp, 0x2000, 0x2000);
    
    //change_rootvnode(FAKEROOTDIR, pid);
    change_rootvnode(rootvp, pid);
    
    //set_vnode_usecount(vnode_ref_by_chdir(FAKEROOTDIR), 0xf000);
    set_vnode_usecount(rootvp, 0x2000, 0x2000);
    
    usleep(100000);
    
    kill(pid, SIGCONT);
    
}



bool is_empty(const char* path){
    
    DIR* dir = opendir(path);
    struct dirent* ent;
    int count = 0;
    
    while ((ent = readdir(dir)) != NULL) {
        count++;
    }
    
    if(count == 2){
        return YES;
    }else{
        return NO;
    }
    
}


int main(int argc, char *argv[], char *envp[]) {
    
    int err = init_kernel();
    if (err) {
        return 1;
    }
    
    if(is_empty(FAKEROOTDIR) || access(FAKEROOTDIR"/private/var/containers", F_OK) != 0){
        printf("error fakeroot not mounted\n");
        return 1;
    }
    
    //uint64_t rootvp = getVnodeAtPath(FAKEROOTDIR);
    chdir("/");
    
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"
        
        
    CFNotificationCenterAddObserver(CFNotificationCenterGetDistributedCenter(), NULL, receive_notify_chrooter, (__bridge CFStringRef)@"jp.akusio.chrooter", NULL, CFNotificationSuspensionBehaviorDeliverImmediately);
        
        	
#pragma clang diagnostic pop
        
    printf("start changerootfs\n");
        
    CFRunLoopRun();
    
    return 1;
    

}
//#endif
//int main() {}
