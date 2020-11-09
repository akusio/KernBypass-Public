#define FAKEROOTDIR "/var/MobileSoftwareUpdate/mnt1"

#ifdef USE_DEV_FAKEVAR
#define FAKEVARDIR "/var/mobile/fakevar"
#define FINAL_FAKEVARDIR FAKEROOTDIR"/dev/fakevar"
#else
#define FAKEVAR_DMG "/var/mobile/test.dmg"
#define FINAL_FAKEVARDIR FAKEROOTDIR"/private/var"
#endif

// Preferences
#define PREF_PATH @"/var/mobile/Library/Preferences/jp.akusio.kernbypass-unofficial.plist"
#define Notify_Preferences "jp.akusio.kernbypass.preferencechanged"
#define Notify_Alert "jp.akusio.kernbypass.alert"
#define Notify_Chrooter "jp.akusio.chrooter"

#define kernbypassMem "/tmp/kernbypassMem"
#define changerootfsMem "/tmp/changerootfsMem"
