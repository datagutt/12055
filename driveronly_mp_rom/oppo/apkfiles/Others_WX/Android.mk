#ifdef VENDOR_EDIT
#@OppoHook.OPPO
#LiHu@APP.WXKF add for WX ROM
ifeq ($(TARGET_BUILD_VERSION),US)
include $(call all-subdir-makefiles)
endif
#endif /* VENDOR_EDIT */