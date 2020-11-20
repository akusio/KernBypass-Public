#include "kernel.h"

//---------maphys and vnodebypass----------//
//Thanks to 0x7ff & @XsF1re
//original code : https://github.com/0x7ff/maphys/blob/master/maphys.c
//original code : https://github.com/XsF1re/vnodebypass/blob/master/main.m

uint32_t off_p_pid = 0;
uint32_t off_p_pfd = 0;
uint32_t off_fd_rdir = 0;
uint32_t off_fd_cdir = 0;
uint32_t off_vnode_iocount = 0;
uint32_t off_vnode_usecount = 0;

#define kCFCoreFoundationVersionNumber_iOS_12_0    (1535.12)
#define kCFCoreFoundationVersionNumber_iOS_13_0_b2 (1656)
#define kCFCoreFoundationVersionNumber_iOS_13_0_b1 (1652.20)
#define kCFCoreFoundationVersionNumber_iOS_14_0_b1 (1740)

int offset_init() {
  if(kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_14_0_b1){
      // ios 14
      off_p_pid = 0x68;
      off_p_pfd = 0xf8;
      off_fd_rdir = 0x40;
      off_fd_cdir = 0x38;
      off_vnode_iocount = 0x64;
      off_vnode_usecount = 0x60;
      return 0;
  }

    if(kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_13_0_b2){
        // ios 13
        off_p_pid = 0x68;
        off_p_pfd = 0x108;
        off_fd_rdir = 0x40;
        off_fd_cdir = 0x38;
        off_vnode_iocount = 0x64;
        off_vnode_usecount = 0x60;
        return 0;
    }

    if(kCFCoreFoundationVersionNumber == kCFCoreFoundationVersionNumber_iOS_13_0_b1){
        //ios 13b1
        return -1;
    }

    if(kCFCoreFoundationVersionNumber <= kCFCoreFoundationVersionNumber_iOS_13_0_b1
       && kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_12_0){
        //ios 12
        off_p_pid = 0x60;
        off_p_pfd = 0x100;
        off_fd_rdir = 0x40;
        off_fd_cdir = 0x38;
        off_vnode_iocount = 0x64;
        off_vnode_usecount = 0x60;
        return 0;
    }

    return -1;
}

//Use JelbrekLib
#ifdef USE_JELBREK_LIB
// shim

#include "jelbrekLib.h"

uint32_t kernel_read32(uint64_t where) {
    return KernelRead_32bits(where);
}

uint64_t kernel_read64(uint64_t where) {
    return KernelRead_64bits(where);
}

void kernel_write32(uint64_t where, uint32_t what) {
    KernelWrite_64bits(where, what);
}


void kernel_write64(uint64_t where, uint64_t what) {
    KernelWrite_64bits(where, what);
}


int init_kernel() {
    if(init_tfp0() != KERN_SUCCESS) {
        printf("get tfp0 failed!\n");
        return 1;
    }
    uint64_t kbase = get_kbase(&kslide);

    if(kbase == 0){
        printf("failed get_kbase\n");
        return 1;
    }

    int err = init_with_kbase(tfp0, kbase, NULL);
    if (err) {
        printf("init failed: %d\n", err);
        return 1;
    }

    err = offset_init();
    if (err) {
        printf("offset init failed: uint64_t proc_of_pid(pid_t pid) {

    uint64_t proc = kernel_read64(allproc);
    uint64_t current_pid = 0;

    while(proc){
        current_pid = kernel_read32(proc + off_p_pid);
        if (current_pid == pid) return proc;
        proc = kernel_read64(proc);
    }

    return 0;
}%d\n", err);
        return 1;
    }
    return 0;
}
//Not use jelbrekLib
#else
uint64_t proc_of_pid(pid_t pid) {

    uint64_t proc = kernel_read64(allproc);
    uint64_t current_pid = 0;

    while(proc){
        current_pid = kernel_read32(proc + off_p_pid);
        if (current_pid == pid) return proc;
        proc = kernel_read64(proc);
    }

    return 0;
}

//read kernel
uint32_t kernel_read32(uint64_t where) {
	uint32_t out;
	kread_buf_tfp0(where, &out, sizeof(uint32_t));
	return out;
}

uint64_t kernel_read64(uint64_t where) {
	uint64_t out;
	kread_buf_tfp0(where, &out, sizeof(uint64_t));
	return out;
}

//write kernel
void kernel_write32(uint64_t where, uint32_t what) {
	uint32_t _what = what;
	kwrite_buf_tfp0(where, &_what, sizeof(uint32_t));
}

void kernel_write64(uint64_t where, uint64_t what) {
	uint64_t _what = what;
	kwrite_buf_tfp0(where, &_what, sizeof(uint64_t));
}

int init_kernel() {
  printf("======= init_kernel =======\n");

  if(dimentio_init(0, NULL, NULL) != KERN_SUCCESS) {
    printf("failed dimentio_init!\n");
    return 1;
  }

  if(init_tfp0() != KERN_SUCCESS) {
    printf("failed init_tfp0!\n");
    return 1;
  }

  if(kbase == 0) {
    printf("failed get kbase\n");
    return 1;
  }

  kern_return_t err = offset_init();

  if (err) {
    printf("offset init failed: %d\n", err);
    return 1;
  }
  return 0;
}
#endif
