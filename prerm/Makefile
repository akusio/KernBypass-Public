ARCHS = arm64 arm64e
GO_EASY_ON_ME := 1

TOOL_NAME = prerm
$(TOOL_NAME)_FILES = prerm.m
$(TOOL_NAME)_CFLAGS = -objc-arc
$(TOOL_NAME)_CODESIGN_FLAGS = -S../ent.plist
$(TOOL_NAME)_INSTALL_PATH = /DEBIAN

include $(THEOS)/makefiles/common.mk
include $(THEOS_MAKE_PATH)/tool.mk
