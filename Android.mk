LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

waffle_top := $(LOCAL_PATH)

waffle_major_version := 1
waffle_minor_version := 0
waffle_patch_version := 1

$(waffle_top)/include/waffle_version.h: \
    $(waffle_top)/Android.mk \
    $(waffle_top)/include/waffle/waffle_version.h.in
	@echo "target  Gen: libwaffle <= $(waffle_top)/include/waffle/waffle_version.h"
	@sed -e 's/@waffle_major_version@/$(waffle_major_version)/' \
	     -e 's/@waffle_minor_version@/$(waffle_minor_version)/' \
	     -e 's/@waffle_patch_version@/$(waffle_patch_version)/' \
	     $(waffle_top)/include/waffle/waffle_version.h.in \
	     > $(waffle_top)/include/waffle/waffle_version.h

LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := libwaffle

LOCAL_CFLAGS := \
    -DANDROID_STUB \
    -DWAFFLE_HAS_ANDROID \

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include/waffle \
    $(LOCAL_PATH)/src/ \
    $(LOCAL_PATH)/src/waffle/core/ \

LOCAL_SRC_FILES := \
    src/waffle/core/wcore_tinfo.c \
    src/waffle/core/wcore_config_attrs.c \
    src/waffle/core/wcore_error.c \
    src/waffle/core/wcore_util.c \
    src/waffle/core/wcore_display.c \
    src/waffle/api/api_priv.c \
    src/waffle/api/waffle_attrib_list.c \
    src/waffle/api/waffle_config.c \
    src/waffle/api/waffle_context.c \
    src/waffle/api/waffle_display.c \
    src/waffle/api/waffle_enum.c \
    src/waffle/api/waffle_error.c \
    src/waffle/api/waffle_gl_misc.c \
    src/waffle/api/waffle_init.c \
    src/waffle/api/waffle_window.c \
    src/waffle/api/waffle_dl.c \
    src/waffle/linux/linux_dl.c \
    src/waffle/linux/linux_platform.c \
    src/waffle/android/droid_priv_egl.c \
    src/waffle/android/droid_platform.c \
    src/waffle/android/droid_display.c \
    src/waffle/android/droid_config.c \
    src/waffle/android/droid_context.c \
    src/waffle/android/droid_window.c \
    src/waffle/android/droid_surfaceflingerlink.cpp \
    src/waffle/egl/egl_no_native.c \

LOCAL_SHARED_LIBRARIES := \
    libEGL \
    libdl \
    libGLESv2 \
    libutils \
    libgui \

LOCAL_GENERATED_SOURCES := \
    $(LOCAL_PATH)/include/waffle_version.h

include $(BUILD_SHARED_LIBRARY)
