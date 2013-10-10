LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=                       \
        GenericSource.cpp               \
        HTTPLiveSource.cpp              \
        NuPlayer.cpp                    \
        NuPlayerDecoder.cpp             \
        NuPlayerDriver.cpp              \
        NuPlayerRenderer.cpp            \
        NuPlayerStreamListener.cpp      \
        RTSPSource.cpp                  \
        StreamingSource.cpp             \
        mp4/MP4Source.cpp               \

LOCAL_C_INCLUDES := \
	$(TOP)/frameworks/av/media/libstagefright/httplive            \
	$(TOP)/frameworks/av/media/libstagefright/include             \
	$(TOP)/frameworks/av/media/libstagefright/mpeg2ts             \
	$(TOP)/frameworks/av/media/libstagefright/rtsp                \
	$(TOP)/frameworks/native/include/media/openmax

#ifdef VENDOR_EDIT
#@OppoHook.OPPO
#liangchagnwei@Plf.Mediasrv, add for dolby
ifdef DOLBY_DAP
    ifdef DOLBY_DAP_OPENSLES
        LOCAL_CFLAGS += -DDOLBY_DAP_OPENSLES
    endif
endif #DOLBY_END
#endif VENDOR_EDIT
LOCAL_MODULE:= libstagefright_nuplayer

LOCAL_MODULE_TAGS := eng

ifeq ($(strip $(MTK_USE_ANDROID_MM_DEFAULT_CODE)),yes)
LOCAL_CFLAGS += -DANDROID_DEFAULT_CODE
else
ifeq ($(strip $(MTK_RTSP_BITRATE_ADAPTATION_SUPPORT)),yes)
  LOCAL_CFLAGS += -DMTK_RTSP_BITRATE_ADAPTATION_SUPPORT
endif
endif

include $(BUILD_STATIC_LIBRARY)

