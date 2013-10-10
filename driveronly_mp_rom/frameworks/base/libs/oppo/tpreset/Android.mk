LOCAL_PATH := $(call my-dir)

### shared library

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    share/oppo_tpreset_tpreset.cpp

LOCAL_SHARED_LIBRARIES := libcutils	
LOCAL_C_INCLUDES := \
    $(call include-path-for, system-core)/cutils
    
LOCAL_C_INCLUDES += $(JNI_H_INCLUDE)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libtpreset
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
