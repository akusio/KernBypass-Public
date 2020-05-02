ARCHS = arm64 arm64e
include $(THEOS)/makefiles/common.mk

TWEAK_NAME = zzzzzzzzzNotifyChroot

zzzzzzzzzNotifyChroot_FILES = Tweak.x
zzzzzzzzzNotifyChroot_CFLAGS = -fobjc-arc

include $(THEOS_MAKE_PATH)/tweak.mk
SUBPROJECTS += kernbypassprefs
include $(THEOS_MAKE_PATH)/aggregate.mk
