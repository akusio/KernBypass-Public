ARCHS = arm64
DEBUG = 0
GO_EASY_ON_ME = 1
PACKAGE_VERSION = $(THEOS_PACKAGE_BASE_VERSION)

ARCHS = arm64 arm64e
THEOS_DEVICE_IP = localhost -p 2222

TOOL_NAME = changerootfs preparerootfs
TARGET := iphone:clang:14.0:14.0

LIB_DIR := lib

preparerootfs_FILES = preparerootfs.m
preparerootfs_CFLAGS = $(CFLAGS) -fobjc-arc -Wno-error=unused-variable -Wno-error=unused-function -D USE_DEV_FAKEVAR

changerootfs_FILES = changerootfs.m
changerootfs_CFLAGS = $(CFLAGS) -fobjc-arc -Wno-error=unused-variable -Wno-error=unused-function

ifdef USE_JELBREK_LIB
	preparerootfs_LDFLAGS = $(LIB_DIR)/jelbrekLib.dylib
	changerootfs_LDFLAGS = $(LIB_DIR)/jelbrekLib.dylib
endif

SUBPROJECTS += zzzzzzzzznotifychroot
SUBPROJECTS += kernbypassd

include $(THEOS)/makefiles/common.mk
include $(THEOS_MAKE_PATH)/tool.mk
include $(THEOS_MAKE_PATH)/aggregate.mk


ifdef USE_JELBREK_LIB
before-package::
	$(THEOS)/toolchain/linux/iphone/bin/ldid -S./ent.plist $(THEOS_STAGING_DIR)/usr/lib/jelbrekLib.dylib
endif

before-package::
	mkdir -p $(THEOS_STAGING_DIR)/usr/lib/
	cp $(LIB_DIR)/jelbrekLib.dylib $(THEOS_STAGING_DIR)/usr/lib
	ldid -S./ent.plist $(THEOS_STAGING_DIR)/usr/bin/changerootfs
	ldid -S./ent.plist $(THEOS_STAGING_DIR)/usr/bin/preparerootfs
	sudo chown -R root:wheel $(THEOS_STAGING_DIR)
	sudo chmod -R 755 $(THEOS_STAGING_DIR)
	sudo chmod 6755 $(THEOS_STAGING_DIR)/usr/bin/kernbypassd
	sudo chmod 666 $(THEOS_STAGING_DIR)/DEBIAN/control

after-package::
	make clean
	sudo rm -rf .theos/_

after-install::
	install.exec "killall backboardd"
