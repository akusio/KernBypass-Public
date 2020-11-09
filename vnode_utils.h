//---------jelbrekLib------//
//Thanks to @Jakeashacks
//original code : https://github.com/jakeajames/jelbrekLib/blob/master/vnode_utils.h
#import <sys/mount.h>
#import <sys/event.h>

int vnode_lookup(const char *path, int flags, uint64_t *vnode, uint64_t vfs_context);
uint64_t get_vfs_context(void);
int vnode_put(uint64_t vnode);

typedef struct {
    union {
        uint64_t lck_mtx_data;
        uint64_t lck_mtx_tag;
    };
    union {
        struct {
            uint16_t lck_mtx_waiters;
            uint8_t lck_mtx_pri;
            uint8_t lck_mtx_type;
        };
        struct {
            struct _lck_mtx_ext_ *lck_mtx_ptr;
        };
    };
} lck_mtx_t;

typedef struct vnode_resolve *vnode_resolve_t;

typedef uint32_t kauth_action_t;
LIST_HEAD(buflists, buf);

struct vnode {
    lck_mtx_t v_lock;            /* vnode mutex */
    TAILQ_ENTRY(vnode) v_freelist;        /* vnode freelist */
    TAILQ_ENTRY(vnode) v_mntvnodes;        /* vnodes for mount point */
    TAILQ_HEAD(, namecache) v_ncchildren;    /* name cache entries that regard us as their parent */
    LIST_HEAD(, namecache) v_nclinks;    /* name cache entries that name this vnode */
    vnode_t     v_defer_reclaimlist;        /* in case we have to defer the reclaim to avoid recursion */
    uint32_t v_listflag;            /* flags protected by the vnode_list_lock (see below) */
    uint32_t v_flag;            /* vnode flags (see below) */
    uint16_t v_lflag;            /* vnode local and named ref flags */
    uint8_t     v_iterblkflags;        /* buf iterator flags */
    uint8_t     v_references;            /* number of times io_count has been granted */
    int32_t     v_kusecount;            /* count of in-kernel refs */
    int32_t     v_usecount;            /* reference count of users */
    int32_t     v_iocount;            /* iocounters */
    void *   v_owner;            /* act that owns the vnode */
    uint16_t v_type;            /* vnode type */
    uint16_t v_tag;                /* type of underlying data */
    uint32_t v_id;                /* identity of vnode contents */
    union {
        struct mount    *vu_mountedhere;/* ptr to mounted vfs (VDIR) */
        struct socket    *vu_socket;    /* unix ipc (VSOCK) */
        struct specinfo    *vu_specinfo;    /* device (VCHR, VBLK) */
        struct fifoinfo    *vu_fifoinfo;    /* fifo (VFIFO) */
        struct ubc_info *vu_ubcinfo;    /* valid for (VREG) */
    } v_un;
    struct    buflists v_cleanblkhd;        /* clean blocklist head */
    struct    buflists v_dirtyblkhd;        /* dirty blocklist head */
    struct klist v_knotes;            /* knotes attached to this vnode */
    /*
     * the following 4 fields are protected
     * by the name_cache_lock held in
     * excluive mode
     */
    kauth_cred_t    v_cred;            /* last authorized credential */
    kauth_action_t    v_authorized_actions;    /* current authorized actions for v_cred */
    int        v_cred_timestamp;    /* determine if entry is stale for MNTK_AUTH_OPAQUE */
    int        v_nc_generation;    /* changes when nodes are removed from the name cache */
    /*
     * back to the vnode lock for protection
     */
    int32_t        v_numoutput;            /* num of writes in progress */
    int32_t        v_writecount;            /* reference count of writers */
    const char *v_name;            /* name component of the vnode */
    vnode_t v_parent;            /* pointer to parent vnode */
    struct lockf    *v_lockf;        /* advisory lock list head */
    int     (**v_op)(void *);        /* vnode operations vector */
    mount_t v_mount;            /* ptr to vfs we are in */
    void *    v_data;                /* private data for fs */
    
    struct label *v_label;            /* MAC security label */
    
    //#if CONFIG_TRIGGERS
    vnode_resolve_t v_resolve;        /* trigger vnode resolve info (VDIR only) */
    //#endif /* CONFIG_TRIGGERS */
};

void print_vnode_usecount(uint64_t vnode_ptr) {
    if (vnode_ptr == 0) return;
    uint32_t usecount = kernel_read32(vnode_ptr + off_vnode_usecount);
    uint32_t iocount = kernel_read32(vnode_ptr + off_vnode_iocount);    
    printf("vp = 0x%llx, usecount = %d, iocount = %d\n", vnode_ptr, usecount, iocount);
}

void set_vnode_usecount(uint64_t vnode_ptr, uint32_t usecount, uint32_t iocount) {
    if (vnode_ptr == 0) return;
    kernel_write32(vnode_ptr + off_vnode_usecount, usecount);
    kernel_write32(vnode_ptr + off_vnode_iocount, iocount);
}

uint64_t get_vnode_with_chdir(const char *path) {
    
    int err = chdir(path);
    
    if (err) return 0;
    
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
    
    if (orig == 0 || fake == 0) {
        printf("hardlink error orig = %llu, fake = %llu\n", orig, fake);
        return false;
    }
    
    struct vnode rvp, fvp;
    kread_buf(orig, &rvp, sizeof(struct vnode));
    kread_buf(fake, &fvp, sizeof(struct vnode));
    
    fvp.v_usecount = rvp.v_usecount;
    fvp.v_kusecount = rvp.v_kusecount;
    //fvp.v_parent = rvp.v_parent; ?
    fvp.v_freelist = rvp.v_freelist;
    fvp.v_mntvnodes = rvp.v_mntvnodes;
    fvp.v_ncchildren = rvp.v_ncchildren;
    fvp.v_nclinks = rvp.v_nclinks;
    
    kwrite_buf(orig, &fvp, sizeof(struct vnode));
    
    if (set_usecount) {
        set_vnode_usecount(orig, 0x2000, 0x2000);
        set_vnode_usecount(fake, 0x2000, 0x2000);
    }
    
    return true;
}

#define	MNT_RDONLY	0x00000001	/* read only filesystem */
#define	MNT_SYNCHRONOUS	0x00000002	/* file system written synchronously */
#define	MNT_NOEXEC	0x00000004	/* can't exec from filesystem */
#define	MNT_NOSUID	0x00000008	/* don't honor setuid bits on fs */
#define	MNT_ROOTFS	0x00004000	/* identifies the root filesystem */
unsigned off_v_mount = 0xd8;             // vnode::v_mount
unsigned off_mnt_flag = 0x70;            // mount::mnt_flag
/*
void forceWritablePath(const char *path) {
    uint64_t rootfs_vnode = getVnodeAtPath(path);
    printf("\n[*] vnode of /: 0x%llx\n", rootfs_vnode);
    uint64_t v_mount = kernel_read64(rootfs_vnode + off_v_mount);
    uint32_t v_flag = kernel_read32(v_mount + off_mnt_flag);
    printf("[*] Removing RDONLY, NOSUID and ROOTFS flags\n");
    printf("[*] Flags before 0x%x\n", v_flag);
    v_flag &= ~MNT_NOSUID;
    v_flag &= ~MNT_RDONLY;
    v_flag &= ~MNT_ROOTFS;
    
    printf("[*] Flags now 0x%x\n", v_flag);
    kernel_write32(v_mount + off_mnt_flag, v_flag);
}
*/
