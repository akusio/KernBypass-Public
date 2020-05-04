ARCHS = arm64 arm64e
THEOS_DEVICE_IP=192.168.1.5

include $(THEOS)/makefiles/common.mk

TOOL_NAME = changerootfs

changerootfs_FILES = main.m

changerootfs_CFLAGS = -fobjc-arc -Wno-error=unused-variable -Wno-error=unused-function

include $(THEOS_MAKE_PATH)/tool.mk

before-package::
	ldid -S./ent.plist $(THEOS_STAGING_DIR)/usr/bin/changerootfs
SUBPROJECTS += notifychroot
include $(THEOS_MAKE_PATH)/aggregate.mk
