ARCHS = arm64

TARGET = iphone:clang:9.2:9.2

include $(THEOS)/makefiles/common.mk

BUNDLE_NAME = KernBypassPrefs

KernBypassPrefs_FILES = AXRootListController.m
KernBypassPrefs_INSTALL_PATH = /Library/PreferenceBundles
KernBypassPrefs_FRAMEWORKS = UIKit
KernBypassPrefs_PRIVATE_FRAMEWORKS = Preferences
KernBypassPrefs_CFLAGS = -fobjc-arc

include $(THEOS_MAKE_PATH)/bundle.mk

internal-stage::
	$(ECHO_NOTHING)mkdir -p $(THEOS_STAGING_DIR)/Library/PreferenceLoader/Preferences$(ECHO_END)
	$(ECHO_NOTHING)cp entry.plist $(THEOS_STAGING_DIR)/Library/PreferenceLoader/Preferences/KernBypassPrefs.plist$(ECHO_END)
