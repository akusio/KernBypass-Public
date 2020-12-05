#include "../config.h"

int main(int argc, char **argv, char **envp) {
    @autoreleasepool {
        NSFileManager *manager = [NSFileManager defaultManager];
        if ([manager fileExistsAtPath:PREF_PATH]) {
            NSMutableDictionary *mutableDict = [[NSMutableDictionary alloc] initWithContentsOfFile:PREF_PATH];
            if ([mutableDict[@"autoEnabled"] boolValue]) {
                [mutableDict removeObjectForKey:@"autoEnabled"];
                [mutableDict writeToFile:PREF_PATH atomically:NO];
                printf("Disable kernbypassd\n");
            }
        }
        // Delete Check file
        if ([manager fileExistsAtPath:@kernbypassMem]) {
            [manager removeItemAtPath:@kernbypassMem error:nil];
            printf("Delete Check file\n");
        }
        // kill changerootfs
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
            system("killall changerootfs");
#pragma clang diagnostic pop
    }
	return 0;
}
