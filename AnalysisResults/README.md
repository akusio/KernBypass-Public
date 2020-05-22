The original KernBypass only protected the rootfs. However, MANY traces of jailbreak files are still existing in the /var.
So we must make a fake var.

0. I first created a fake var with a python shell script, see fakevar.py
1. Some of folders in var should be hided, so we should make a fake /var, and only link those needed folders into it. However if I simply link a folder instead of a mount point into fake root, the whole system will freeze and panic. Maybe I'm wrong, you can simply try it out yourself :)
2. I first tried to make use of the /dev, as the devfs allow us to create folder. It worked, however it seems that the devfs does not allow you to create files, so sadly this way is not that good. (See panic1.log)
3. Then I reliazed that iOS is able to mount a dmg. So I created a dmg and simply stored some empty folder in it. I mount it onto fakeroot's /private/var, and then link its subfolders.
    - I tried to use jelbrekLib, but it crashes on vfs_get_context(), so I decided to drop it.
    - After dropping the jelbrekLib, this method also works, but this time the system will panic if an application that was KernBypass'ed AND crashed. The panic, however, is caused by ReportCrash daemon, if you replace this daemon into another simple executable, the system won't panic. (See panic2.log)

4. So now, I release both branchs, and you can choose it as you wish.