#define FAKEROOTDIR "/var/MobileSoftwareUpdate/mnt1"
//#define FAKEROOTDIR "/fakeroot"



#ifdef USE_DEV_FAKEVAR
#define FAKEVARDIR "/var/mobile/fakevar"
#define FINAL_FAKEVARDIR FAKEROOTDIR"/dev/fakevar"
#else
#define FAKEVAR_DMG "/var/mobile/test.dmg"
#define FINAL_FAKEVARDIR FAKEROOTDIR"/private/var"
#endif
