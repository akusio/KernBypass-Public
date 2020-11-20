# KernBypass
kernel level jailbreak detection bypass

## Support Devices
- iOS12.0-14.x? (confirmed on iOS12.4 and above)
- A7-A13
- unc0ver or checkra1n

## Credits
- maphys by 0x7ff
- vnodebypass and iOS14 Support by @XsF1re
- jelbrekLib by @Jakeashacks
- Translated by sohsatoh
- iOS12 support by dora2-iOS


## WARNING
**This tweak is the kernel level. There is NO warranty. Run it at your own risk.**

## Getting Started
### Installation
1. Remove file `/var/mobile/Library/Preferences/jp.akusio.kernbypass.plist` if exist.
2. [download](https://github.com/XsF1re/KernBypass-Public/releases) and install the deb file.
### Setting up KernBypass
3. In terminal, run `su` and type your password.
4. Download the fakevar.zip from http://repo.misty.moe/apt/fakevar13.zip, and extract it to /var/mobile/fakevar (whatever method you use to extract, make sure there’s /var/mobile/fakevar/mobile)
5. Run `preparerootfs`
6. Run `changerootfs &` (don't forget "&").
7. Run `disown %1`
8. Done. The changerootfs is now a daemon.

### Selecting apps to bypass
1. After installing changerootfs, open Preferences > KernBypass, then select the applications to be enabled the bypass.

## Uninstall
1. Just uninstall from Cydia.
2. REBOOT!!!

## License
KernBypass is licensed under the [GPLv3](LICENSE).
