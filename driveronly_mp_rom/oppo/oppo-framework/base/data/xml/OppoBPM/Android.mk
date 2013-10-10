LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

$(shell mkdir -p $(TARGET_OUT)/bpm;)

include $(CLEAR_VARS)
LOCAL_MODULE       := brd.xml
LOCAL_MODULE_TAGS  := optional eng
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := brd.xml
LOCAL_MODULE_PATH  := $(TARGET_OUT)/bpm
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)
LOCAL_MODULE       := pkg.xml
LOCAL_MODULE_TAGS  := optional eng
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := pkg.xml
LOCAL_MODULE_PATH  := $(TARGET_OUT)/bpm
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := bpm.xml
LOCAL_MODULE_TAGS  := optional eng
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := bpm.xml
LOCAL_MODULE_PATH  := $(TARGET_OUT)/bpm
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := bpm_sts.xml
LOCAL_MODULE_TAGS  := optional eng
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := bpm_sts.xml
LOCAL_MODULE_PATH  := $(TARGET_OUT)/bpm
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := bad_apps.xml
LOCAL_MODULE_TAGS  := optional eng
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := bad_apps.xml
LOCAL_MODULE_PATH  := $(TARGET_OUT)/bpm
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := appwidgets.xml
LOCAL_MODULE_TAGS  := optional eng
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := appwidgets.xml
LOCAL_MODULE_PATH  := $(TARGET_OUT)/bpm
include $(BUILD_PREBUILT)