ifeq ($(strip $(MTK_PLATFORM)),MT6589)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    MtkI420ColorConverter.cpp

LOCAL_C_INCLUDES:= \
        $(TOP)/mediatek/frameworks/av/media/libstagefright/include/omx_core \
        $(TOP)/frameworks/native/include/media/editor \
        $(TOP)/mediatek/hardware/dpframework/inc

LOCAL_SHARED_LIBRARIES := libdpframework liblog

LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libI420colorconvert

include $(BUILD_SHARED_LIBRARY)

endif
