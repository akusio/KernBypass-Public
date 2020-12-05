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
        // kill changerootfs
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated-declarations"
        system("killall -9 changerootfs");
        #pragma clang diagnostic pop
    }
	return 0;
}
