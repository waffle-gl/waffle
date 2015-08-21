LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

waffle_top := $(LOCAL_PATH)

#
# !!! The version must be updated in tandem with the CMakeLists !!!
#
waffle_major_version := 1
waffle_minor_version := 5
waffle_patch_version := 2
waffle_api_version := 0x0105

waffle_android_major_version := $(word 1, $(subst ., , $(PLATFORM_VERSION)))
waffle_android_minor_version := $(word 2, $(subst ., , $(PLATFORM_VERSION)))
waffle_android_version := $(waffle_android_major_version).$(waffle_android_minor_version)

$(waffle_top)/include/waffle/waffle_version.h: \
    $(waffle_top)/Android.mk \
    $(waffle_top)/include/waffle/waffle_version.h.in
	@echo "target  Gen: libwaffle <= $(waffle_top)/include/waffle/waffle_version.h"
	@sed -e 's/@waffle_major_version@/$(waffle_major_version)/' \
	     -e 's/@waffle_minor_version@/$(waffle_minor_version)/' \
	     -e 's/@waffle_patch_version@/$(waffle_patch_version)/' \
	     $(waffle_top)/include/waffle/waffle_version.h.in \
	     > $(waffle_top)/include/waffle/waffle_version.h

LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := libwaffle-$(waffle_major_version)

LOCAL_CFLAGS := \
    -DANDROID_STUB \
    -DWAFFLE_API_VERSION=$(waffle_api_version) \
    -DWAFFLE_API_EXPERIMENTAL \
    -DWAFFLE_HAS_ANDROID \
    -DWAFFLE_ANDROID_MAJOR_VERSION=$(waffle_android_major_version) \
    -DWAFFLE_ANDROID_MINOR_VERSION=$(waffle_android_minor_version) \
    -Wno-pointer-arith

ifeq ($(shell echo "$(waffle_android_version) >= 4.4" | bc),1)
LOCAL_CONLYFLAGS := -std=c99
else
LOCAL_CFLAGS += -std=c99
endif

LOCAL_CFLAGS += -fvisibility=hidden

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/include/waffle \
    $(LOCAL_PATH)/src/ \
    $(LOCAL_PATH)/src/waffle/api/ \
    $(LOCAL_PATH)/src/waffle/core/ \
    $(LOCAL_PATH)/src/waffle/egl/ \
    $(LOCAL_PATH)/src/waffle/linux/ \
    $(LOCAL_PATH)/src/waffle/droid/ \
    $(LOCAL_PATH)/third_party/khronos/ \
    $(LOCAL_PATH)/third_party/threads/

LOCAL_SRC_FILES := \
    src/waffle/core/wcore_tinfo.c \
    src/waffle/core/wcore_config_attrs.c \
    src/waffle/core/wcore_error.c \
    src/waffle/core/wcore_util.c \
    src/waffle/core/wcore_display.c \
    src/waffle/core/wcore_attrib_list.c \
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
    src/waffle/egl/wegl_config.c \
    src/waffle/egl/wegl_context.c \
    src/waffle/egl/wegl_display.c \
    src/waffle/egl/wegl_platform.c \
    src/waffle/egl/wegl_util.c \
    src/waffle/egl/wegl_window.c \
    src/waffle/android/droid_platform.c \
    src/waffle/android/droid_display.c \
    src/waffle/android/droid_window.c \
    src/waffle/android/droid_surfaceflingerlink.cpp \
    third_party/threads/threads_posix.c

LOCAL_SHARED_LIBRARIES := \
    libdl \
    libutils \
    libgui

LOCAL_GENERATED_SOURCES := \
    $(LOCAL_PATH)/include/waffle/waffle_version.h

LOCAL_COPY_HEADERS := \
    include/waffle/waffle.h \
    include/waffle/waffle_gbm.h \
    include/waffle/waffle_glx.h \
    include/waffle/waffle_version.h \
    include/waffle/waffle_wayland.h \
    include/waffle/waffle_x11_egl.h

LOCAL_COPY_HEADERS_TO := waffle-$(waffle_major_version)

include $(BUILD_SHARED_LIBRARY)

SUBDIRS := \
    examples \
    src/utils

mkfiles := $(patsubst %,$(waffle_top)/%/Android.mk,$(SUBDIRS))
include $(mkfiles)
