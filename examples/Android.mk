LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng
LOCAL_MODULE:= gl_basic

LOCAL_CFLAGS:= \
        -DANDROID_STUB \
        -DWAFFLE_HAS_ANDROID \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../src/

LOCAL_SRC_FILES:= \
    gl_basic.c \

LOCAL_SHARED_LIBRARIES := libwaffle

include $(BUILD_EXECUTABLE)
