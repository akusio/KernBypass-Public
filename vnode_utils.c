//---------jelbrekLib------//
//Thanks to @Jakeashacks
//original code : https://github.com/jakeajames/jelbrekLib/blob/master/vnode_utils.h

#include "vnode_utils.h"

unsigned off_v_mount = 0xd8;             // vnode::v_mount
unsigned off_mnt_flag = 0x70;            // mount::mnt_flag

void print_vnode_usecount(uint64_t vnode_ptr){

    if(vnode_ptr == 0) return;

    uint32_t usecount = kernel_read32(vnode_ptr + off_vnode_usecount);

    uint32_t iocount = kernel_read32(vnode_ptr + off_vnode_iocount);

    printf("vp = 0x%llx, usecount = %d, iocount = %d\n", vnode_ptr, usecount, iocount);
}

void set_vnode_usecount(uint64_t vnode_ptr, uint32_t usecount, uint32_t iocount){
    if(vnode_ptr == 0) return;
    kernel_write32(vnode_ptr + off_vnode_usecount, usecount);
    kernel_write32(vnode_ptr + off_vnode_iocount, iocount);
}

uint64_t get_vnode_with_chdir(const char* path){

    int err = chdir(path);

    if(err) return 0;

    uint64_t proc = proc_of_pid(getpid());

    uint64_t filedesc = kernel_read64(proc + off_p_pfd);

    uint64_t vp = kernel_read64(filedesc + off_fd_cdir);

    uint32_t usecount = kernel_read32(vp + off_vnode_usecount);

    uint32_t iocount = kernel_read32(vp + off_vnode_iocount);

    kernel_write32(vp + off_vnode_usecount, usecount+1);
    kernel_write32(vp + off_vnode_iocount, iocount+1);

    chdir("/");
    return vp;

}

bool copy_file_in_memory(char *original, char *replacement, bool set_usecount) {

    uint64_t orig = get_vnode_with_chdir(original);
    uint64_t fake = get_vnode_with_chdir(replacement);

    if(orig == 0 || fake == 0){
        printf("hardlink error orig = %llu, fake = %llu\n", orig, fake);
        return false;
    }

    struct vnode rvp, fvp;
    kread_buf_tfp0(orig, &rvp, sizeof(struct vnode));
    kread_buf_tfp0(fake, &fvp, sizeof(struct vnode));

    fvp.v_usecount = rvp.v_usecount;
    fvp.v_kusecount = rvp.v_kusecount;
    //fvp.v_parent = rvp.v_parent; ?
    fvp.v_freelist = rvp.v_freelist;
    fvp.v_mntvnodes = rvp.v_mntvnodes;
    fvp.v_ncchildren = rvp.v_ncchildren;
    fvp.v_nclinks = rvp.v_nclinks;


    kwrite_buf_tfp0(orig, &fvp, sizeof(struct vnode));

    if (set_usecount) {
        set_vnode_usecount(orig, 0x2000, 0x2000);
        set_vnode_usecount(fake, 0x2000, 0x2000);
    }
    return true;

}
