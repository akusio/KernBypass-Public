#include <dirent.h>

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

static void NSPrint(NSString *format, ...)
 {
    va_list args;
    
    va_start(args, format);
    NSString *string  = [[NSString alloc] initWithFormat:format arguments:args];
    va_end(args);
    
    fprintf(stdout, "%s\n", [string UTF8String]);
    
#if !__has_feature(objc_arc)
    [string release];
#endif
}
/*
int copy_dir(const char *from, const char *to) {
    NSError *copyError = nil;
    [[NSFileManager defaultManager] 
        copyItemAtPath:[NSString stringWithUTF8String:from]
        toPath:[NSString stringWithUTF8String:to] error:&copyError];
    
    if (copyError != nil) {
        NSPrint(@"%@", [copyError localizedDescription]);
        return 1;
    }
    return 0;
}*/


int copy_dir(const char *from, const char *to);

#include <sys/stat.h>

#define BUFFERSIZE 0x4000
#define COPYMORE 0644

int file_exists(const char* filename){
    struct stat buffer;
    int exist = stat(filename,&buffer);
    if(exist == 0)
        return 1;
    else // -1
        return 0;
}

void oops(const char *msg, char *s2) {
    fprintf(stderr, "%s", msg);
    perror(s2);
}

int copyFiles(char *source, char *destination)
{
    int in_fd, out_fd, n_chars;
    char buf[BUFFERSIZE];

    if ((in_fd = open(source, O_RDONLY)) == -1)
    {
        oops("Cannot open ", source);
        return 1;
    }

    if ((out_fd = creat(destination, COPYMORE)) == -1)
    {
        oops("Cannot creat ", destination);
        return 1;
    }

    while ((n_chars = read(in_fd, buf, BUFFERSIZE)) > 0)
    {
        if (write(out_fd, buf, n_chars) != n_chars)
        {
            oops("Write error to ", destination);
            return 1;
        }

        if (n_chars == -1)
        {
            oops("Read error from ", source);
            return 1;
        }
    }

    if (close(in_fd) == -1 || close(out_fd) == -1)
    {
        oops("Error closing files", "");
        return 1;
    }

    return 0;
}

int copy_dir(const char *name, const char *target)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return 1;
    
    mkdir(target, 0755);
    char path[1024];
    char targetpath[1024];
    int child = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
        snprintf(targetpath, sizeof(targetpath), "%s/%s", target, entry->d_name);
	    if (entry->d_type == DT_DIR)	
	        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                        continue;        
    	printf("Copying: %s...", path);
        child++;
        if (entry->d_type == DT_DIR)
        {
            if (mkdir(targetpath, 0755)) {
                perror("create folder");
            }
            if (!file_exists(targetpath)) {
                printf("Failed\n");
                continue;
            }
            printf("\n");
            copy_dir(path, targetpath);
        }
        else
        {
            if (copyFiles(path, targetpath) != 0) {
                printf("Failed\n");
            }
            printf("\n");
        }
    }
    closedir(dir);
    if (child == 0) {
        return 1;
    }
    return 0;
}
