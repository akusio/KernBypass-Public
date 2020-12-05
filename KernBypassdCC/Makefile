ARCHS = arm64 arm64e

TARGET = iphone:14.0:12.0

BUNDLE_NAME = KernBypassdCC
$(BUNDLE_NAME)_FILES = Switch.x
$(BUNDLE_NAME)_CFLAGS = -fobjc-arc -I$(THEOS_PROJECT_DIR)/headers
$(BUNDLE_NAME)_LDFLAGS = Frameworks/ControlCenterUIKit.tbd
$(BUNDLE_NAME)_FRAMEWORKS = UIKit
$(BUNDLE_NAME)_LIBRARIES = substrate
$(BUNDLE_NAME)_INSTALL_PATH = /Library/ControlCenter/Bundles

include $(THEOS)/makefiles/common.mk
include $(THEOS_MAKE_PATH)/bundle.mk
