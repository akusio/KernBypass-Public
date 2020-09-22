ARCHS = arm64

include $(THEOS)/makefiles/common.mk

TOOL_NAME = changerootfs preparerootfs
TARGET := iphone:clang:10.3:5.1

LIB_DIR := lib

preparerootfs_FILES = preparerootfs.m
preparerootfs_CFLAGS = $(CFLAGS) -fobjc-arc -Wno-error=unused-variable -Wno-error=unused-function -D USE_DEV_FAKEVAR

changerootfs_FILES = changerootfs.m
changerootfs_CFLAGS = $(CFLAGS) -fobjc-arc -Wno-error=unused-variable -Wno-error=unused-function

ifdef USE_JELBREK_LIB
	preparerootfs_LDFLAGS = $(LIB_DIR)/jelbrekLib.dylib
	changerootfs_LDFLAGS = $(LIB_DIR)/jelbrekLib.dylib
endif

include $(THEOS_MAKE_PATH)/tool.mk

ifdef USE_JELBREK_LIB
before-package::
	$(THEOS)/toolchain/linux/iphone/bin/ldid -S./ent.plist $(THEOS_STAGING_DIR)/usr/lib/jelbrekLib.dylib
endif

before-package::
	mkdir -p $(THEOS_STAGING_DIR)/usr/lib/
	cp $(LIB_DIR)/jelbrekLib.dylib $(THEOS_STAGING_DIR)/usr/lib
	/usr/bin/ldid -S./ent.plist $(THEOS_STAGING_DIR)/usr/bin/changerootfs
	/usr/bin/ldid -S./ent.plist $(THEOS_STAGING_DIR)/usr/bin/preparerootfs	

SUBPROJECTS += zzzzzzzzznotifychroot
include $(THEOS_MAKE_PATH)/aggregate.mk
