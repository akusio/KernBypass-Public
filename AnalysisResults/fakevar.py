import os

paths = [
"/var/lib"
"/var/mobile/Library/iGameGuardian",
"/var/db/stash",
"/var/mobile/Library/Flex3",
"/var/containers/Bundle/tweaksupport",
"/var/mobile/Library/Caches/com.saurik.Cydia",
"/var/containers/Bundle/iosbinpack64",
"/var/libexec",
"/var/mobile/Library/Caches/Snapshots/org.coolstar.SileoStore",
"/var/mobile/Library/Preferences/org.coolstar.SileoStore.plist",
"/var/mobile/Library/Preferences/xyz.willy.Zebra.plist",
]

paths = [c.strip('/').split('/') for c in paths]
pathdict = {}
for c in paths:
    cur = pathdict
    for node in c:
        if not node in cur:
            cur[node] = {}
        cur = cur[node]

def get_childs(curpath):
    curpath = curpath.strip('/').split('/')
    ret = pathdict
    for c in curpath:
        ret = ret[c]
    return ret

def process_dir(curpath):
    subdir, dirs, files = next(os.walk(curpath))
    childs = get_childs(curpath)
    try:
        os.makedirs("." + os.sep + curpath)
    except:
        pass

    if not childs:
        return
 
    for file in files:
        filepath = subdir + os.sep + file
        if not file in childs:
            print("Creating file: %s" % filepath)
            open("." + os.sep + filepath, 'w')
        else:
            print("Filtered: %s" % filepath)
    
    for dir in dirs:
        dirpath = subdir + os.sep + dir
        if not dir in childs:
            try:
                os.makedirs("." + os.sep + dirpath)
            except:
                pass
            print("Creating dir: %s" % dirpath)
        else:
            if childs[dir]:
                try:
                    os.makedirs("." + os.sep + dirpath)
                except:
                    pass
                process_dir(dirpath)
            else:
                print("Filtered: %s" % dirpath)


process_dir('/var')
