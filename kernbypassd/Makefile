DEBUG = 0
ARCHS = arm64 arm64e

TOOL_NAME = kernbypassd
kernbypassd_FILES = kernbypassd.m
kernbypassd_CFLAGS = -objc-arc
kernbypassd_CODESIGN_FLAGS = -S../ent.plist

include $(THEOS)/makefiles/common.mk
include $(THEOS_MAKE_PATH)/tool.mk
