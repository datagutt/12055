ifeq ($(OPPO_CMCC_TEST),yes)
else

LOCAL_PATH:= $(call my-dir)

SINGLE_MODULE_TAGS := optional
SINGLE_CERTIFICATE := PRESIGNED
SINGLE_TARGET := 

#ifdef VENDOR_EDIT
#@OppoHook.OPPO
#LiHu@EXP.APP modify for WX ROM do not need OppoBootReg.apk 2013.5.29
ifeq ($(TARGET_BUILD_VERSION),US)
copy_from := $(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find . -maxdepth 1 -name "*.apk" -and -not -name ".*"  -and -not -name "OppoBootReg.apk" -and -not -name "OppoDriveMode.apk" -and -not -name "IFlySpeechService.apk" -and -not -name "OppoPhoneActivation.apk" -and -not -name "OppoSpeechAssist.apk" -and -not -name "OppoSpeechService.apk" -and -not -name "OppoWeather.apk" -and -not -name "OppoWeatherLocationService.apk") \
 )
else
copy_from := $(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find . -maxdepth 1 -name "*.apk" -and -not -name ".*") \
 )
endif
#endif /* VENDOR_EDIT */

$(foreach SINGLE_TARGET, $(copy_from), $(eval include $(LOCAL_PATH)/single.mk))

include $(call all-makefiles-under,$(LOCAL_PATH))

endif