LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	Camera.cpp \
	CameraParameters.cpp \
	ICamera.cpp \
	ICameraClient.cpp \
	ICameraService.cpp \
	ICameraRecordingProxy.cpp \
	ICameraRecordingProxyListener.cpp

#ifeq "yes" "$(strip $(MTK_CAMERA_BSP_SUPPORT))"
    LOCAL_SRC_FILES += $(call all-c-cpp-files-under, ../../../$(MTK_ROOT)/frameworks-ext/av/camera)
    LOCAL_C_INCLUDES += $(MTK_ROOT)/frameworks-ext/av/include
#endif

ifeq ($(strip $(MTK_USE_ANDROID_MM_DEFAULT_CODE)),yes)
LOCAL_CFLAGS += -DANDROID_DEFAULT_CODE
endif

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	libhardware \
	libui \
	libgui

$(shell cp $(LOCAL_PATH)/haarcascade_face.xml $(TARGET_OUT_ETC)/haarcascade_face.xml)	
$(shell cp $(LOCAL_PATH)/haarcascade_eye.xml $(TARGET_OUT_ETC)/haarcascade_eye.xml)
$(shell cp $(LOCAL_PATH)/libintel_jni.so $(TARGET_OUT_SHARED_LIBRARIES)/libintel_jni.so)

LOCAL_MODULE:= libcamera_client

include $(BUILD_SHARED_LIBRARY)
