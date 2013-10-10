LOCAL_PATH:= $(call my-dir)

SINGLE_MODULE_TAGS := optional
SINGLE_CERTIFICATE := PRESIGNED
SINGLE_TARGET := 
#ifdef VENDOR_EDIT
#@OppoHook.OPPO
#yanwei@EXP.Multimedia.OppoGallery2, 2013-07-08, Modify for replacing cloud gallery with that of EXP's
ifeq ($(TARGET_BUILD_VERSION),US)
copy_from := $(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find . -maxdepth 1 -name "*.apk" -and -not -name ".*"  -and -not -name "OppoGallery2.apk") \
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