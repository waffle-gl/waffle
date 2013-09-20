LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng
LOCAL_MODULE:= gl_basic

LOCAL_CFLAGS:= \
        -DANDROID_STUB \
        -DWAFFLE_HAS_ANDROID \
        -std=c99 \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include/waffle/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../src/waffle/

LOCAL_SRC_FILES:= \
    gl_basic.c \

LOCAL_SHARED_LIBRARIES := libwaffle-1

include $(BUILD_EXECUTABLE)
