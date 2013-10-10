#vendorEdit
#lihu@EXP.SysFramework.GMS,2013/09/06,moved for GMS_4.2_r3 has given this so so delete it
ifneq ($(TARGET_BUILD_VERSION),US)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    android_renderscript_RenderScript.cpp

LOCAL_SHARED_LIBRARIES := \
        libandroid_runtime \
        libRSSupport \
        libcutils \
        libjnigraphics

LOCAL_STATIC_LIBRARIES :=

rs_generated_include_dir := $(call intermediates-dir-for,SHARED_LIBRARIES,libRSSupport,,)

LOCAL_C_INCLUDES += \
	$(JNI_H_INCLUDE) \
	frameworks/rs \
	$(rs_generated_include_dir)

LOCAL_CFLAGS +=

LOCAL_LDLIBS := -lpthread
LOCAL_ADDITIONAL_DEPENDENCIES := $(addprefix $(rs_generated_include_dir)/,rsgApiFuncDecl.h)
LOCAL_MODULE:= librsjni
LOCAL_ADDITIONAL_DEPENDENCIES += $(rs_generated_source)
LOCAL_MODULE_TAGS := optional
LOCAL_REQUIRED_MODULES := libRSSupport

include $(BUILD_SHARED_LIBRARY)
endif
#ENDvendorEdit