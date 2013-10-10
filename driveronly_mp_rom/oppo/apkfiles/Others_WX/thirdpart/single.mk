$(shell target_path=$(TARGET_OUT)/reserve; \
    if [ ! -d $$target_path ];then \
        mkdir -p $$target_path; fi; \
    cp -f $(LOCAL_PATH)/$(SINGLE_TARGET) $$target_path)

include $(CLEAR_VARS)
LOCAL_MODULE       := $(SINGLE_TARGET)
LOCAL_MODULE_TAGS  := $(SINGLE_MODULE_TAGS)
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_PATH  :=$(TARGET_OUT_DATA)/app
LOCAL_CERTIFICATE  := $(SINGLE_CERTIFICATE)
LOCAL_SRC_FILES    := $(SINGLE_TARGET)
include $(BUILD_PREBUILT)