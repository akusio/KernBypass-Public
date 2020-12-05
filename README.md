# KernBypass (Unofficial)
kernel level jailbreak detection bypass

## Support Devices
- iOS12.0-14.2 (confirmed on iOS12.4 and above)
- A7-A13
- unc0ver or checkra1n or odysseyra1n or Odyssey

## Credits
- maphys by 0x7ff
- vnodebypass and iOS14 Support by @XsF1re
- jelbrekLib by @Jakeashacks
- Translated by sohsatoh
- iOS12 support by dora2-iOS
- Choicy Compatibility @level3tjg (Note: It doesn't work with Launch without Tweaks)
- fakevar version added @NyaMisty
- Preferences refresh, overall code optimization @ichitaso
- Icon design by @JannikCrack and @ichitaso


## WARNING
**This tweak is the kernel level. There is NO warranty. Run it at your own risk.**  
**Note: Not all applications are supported.**  
**(Please think that most don't work)**  

## Getting Started
### Installation
1. Added my private repo: https://cydia.ichitaso.com
2. ​Install from Cydia or other package manager
### Setting up KernBypass (Manual)
3. ​**If you were using a previous version, be sure to Reboot first.**
4. In terminal, run `su` and type your password.
5. Run `preparerootfs`
6. Run `changerootfs &` (don't forget "&").
7. Run `disown %1`
8. Done. The changerootfs is now a daemon.

### Setting up KernBypass (GUI)
1. Settings -> KernBypass -> Enable KernBypass

### Selecting apps to bypass
1. After installing changerootfs, open Preferences > KernBypass, then select the applications to be enabled the bypass.

## Uninstall
1. Just uninstall from Cydia or other package manager.
2. REBOOT!!!

## License
KernBypass is licensed under the [GPLv3](LICENSE).
