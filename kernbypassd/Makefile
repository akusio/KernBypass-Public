ARCHS = arm64 arm64e

TOOL_NAME = kernbypassd
$(TOOL_NAME)_FILES = kernbypassd.m
$(TOOL_NAME)_CFLAGS = -objc-arc
$(TOOL_NAME)_CODESIGN_FLAGS = -S../ent.plist

include $(THEOS)/makefiles/common.mk
include $(THEOS_MAKE_PATH)/tool.mk
