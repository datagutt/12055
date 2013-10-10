LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE       := coefs_48000.bin
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := coefs_48000.bin
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)/dolby_coef
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= \
	EffectDolby.cpp

LOCAL_CFLAGS+= -O2

LOCAL_SHARED_LIBRARIES := \
	libcutils

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/soundfx

LOCAL_MODULE:= libdolby

ifeq ($(TARGET_OS)-$(TARGET_SIMULATOR),linux-true)
LOCAL_LDLIBS += -ldl
endif

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES += libdl
endif

LOCAL_LDFLAGS := $(LOCAL_PATH)/mobiledolby.a

LOCAL_C_INCLUDES := \
	$(call include-path-for, graphics corecg)

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)