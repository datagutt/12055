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
          find . -maxdepth 1 -name "*.apk" -and -not -name ".*"  -and -not -name "OppoBootReg.apk" -and -not -name "OppoOperationManual.apk"  -and -not -name "OppoAfterService.apk") \
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