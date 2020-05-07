DEBUG = 0
GO_EASY_ON_ME = 1
PACKAGE_VERSION = $(THEOS_PACKAGE_BASE_VERSION)

ARCHS = arm64 arm64e
THEOS_DEVICE_IP = localhost -p 2222

TOOL_NAME = changerootfs
changerootfs_FILES = main.m
changerootfs_CFLAGS = -objc-arc -Wno-error=unused-variable -Wno-error=unused-function

SUBPROJECTS += zzzzzzzzznotifychroot
SUBPROJECTS += kernbypassd

include $(THEOS)/makefiles/common.mk
include $(THEOS_MAKE_PATH)/tool.mk
include $(THEOS_MAKE_PATH)/aggregate.mk


before-package::
	ldid -S./ent.plist $(THEOS_STAGING_DIR)/usr/bin/changerootfs
	sudo chown -R root:wheel $(THEOS_STAGING_DIR)
	sudo chmod -R 755 $(THEOS_STAGING_DIR)
	sudo chmod 6755 $(THEOS_STAGING_DIR)/usr/bin/kernbypassd
	sudo chmod 666 $(THEOS_STAGING_DIR)/DEBIAN/control

after-package::
	make clean
	sudo rm -rf .theos/_

after-install::
	install.exec "killall backboardd"