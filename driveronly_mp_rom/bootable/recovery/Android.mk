LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

commands_recovery_local_path := $(LOCAL_PATH)

LOCAL_SRC_FILES := \
    recovery_ui.c \
    oppo_intent.c \
    bootloader.c \
    install.c \
    roots.c \
    verifier.c \
    recovery.c

LOCAL_SRC_FILES += \
    oppo/libs/png/png.c \
    oppo/libs/png/pngerror.c \
    oppo/libs/png/pnggccrd.c \
    oppo/libs/png/pngget.c \
    oppo/libs/png/pngmem.c \
    oppo/libs/png/pngpread.c \
    oppo/libs/png/pngread.c \
    oppo/libs/png/pngrio.c \
    oppo/libs/png/pngrtran.c \
    oppo/libs/png/pngrutil.c \
    oppo/libs/png/pngset.c \
    oppo/libs/png/pngtrans.c \
    oppo/libs/png/pngvcrd.c \
    oppo/libs/minutf8/minutf8.c \
    oppo/libs/freetype/autofit/autofit.c \
    oppo/libs/freetype/base/basepic.c \
    oppo/libs/freetype/base/ftapi.c \
    oppo/libs/freetype/base/ftbase.c \
    oppo/libs/freetype/base/ftbbox.c \
    oppo/libs/freetype/base/ftbitmap.c \
    oppo/libs/freetype/base/ftdbgmem.c \
    oppo/libs/freetype/base/ftdebug.c \
    oppo/libs/freetype/base/ftglyph.c \
    oppo/libs/freetype/base/ftinit.c \
    oppo/libs/freetype/base/ftpic.c \
    oppo/libs/freetype/base/ftstroke.c \
    oppo/libs/freetype/base/ftsynth.c \
    oppo/libs/freetype/base/ftsystem.c \
    oppo/libs/freetype/cff/cff.c \
    oppo/libs/freetype/pshinter/pshinter.c \
    oppo/libs/freetype/psnames/psnames.c \
    oppo/libs/freetype/raster/raster.c \
    oppo/libs/freetype/sfnt/sfnt.c \
    oppo/libs/freetype/smooth/smooth.c \
    oppo/libs/freetype/truetype/truetype.c \
    oppo/src/edify/lex.yy.c \
    oppo/src/edify/parser.c \
    oppo/src/edify/expr.c \
    oppo/src/libs/oppo_array.c \
    oppo/src/libs/oppo_freetype.c \
    oppo/src/libs/oppo_graph.c \
    oppo/src/libs/oppo_input.c \
    oppo/src/libs/oppo_languages.c \
    oppo/src/libs/oppo_libs.c \
    oppo/src/libs/oppo_png.c \
    oppo/src/controls/oppo_control_button.c \
    oppo/src/controls/oppo_control_menubox.c \
    oppo/src/controls/oppo_control_title.c \
    oppo/src/controls/oppo_controls.c \
    oppo/src/controls/oppo_control_threads.c \
    oppo/src/main/oppo_ui.c \
    oppo/src/main/common_ui.c \
    oppo/src/main/lang_ui.c \
    oppo/src/main/sd_file.c \
    oppo/src/main/sd_ui.c \
    oppo/src/main/power_ui.c \
    oppo/src/main/wipe_ui.c \
    oppo/src/main/menu_node.c \
    oppo/src/main/oppo_installer.c \
    oppo/src/main/oppo.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/oppo/include $(LOCAL_PATH)oppo/src
 
LOCAL_MODULE := recovery_old

LOCAL_FORCE_STATIC_EXECUTABLE := true

RECOVERY_API_VERSION := 3
MYDEFINE_CFLAGS :=  -D_GLIBCXX_DEBUG_PEDANTIC \
                  -DFT2_BUILD_LIBRARY=1 \
                  -DDARWIN_NO_CARBON \
                  -DVENDOR_EDIT
LOCAL_CFLAGS += -DRECOVERY_API_VERSION=$(RECOVERY_API_VERSION) 
LOCAL_CFLAGS += $(MYDEFINE_CFLAGS)
LOCAL_CFLAGS += -DUSE_EXT4



#VENDOR_EDIT
#######user difine macro start

#suport chinese display when browse sd card 
LOCAL_CFLAGS += -DSUPPORT_UTF8

#update package verify
#LOCAL_CFLAGS += -DVERIFY_UPDATE_PACKAGE

#for xiaomi 
#LOCAL_CFLAGS += -DXIAOMI_M2

#we can not detect virtual-key-up event for some phone, so we simulate
LOCAL_CFLAGS += -DSIMULATE_KEY_UP_EVENT

#for some 32bit and high resolution system, use 16bit can speed up display. eg,12025
#LOCAL_CFLAGS += -DFORCE_USE_RGB565

#some system's RGB offset is special when use 32bit display, eg, 12025 12001
#LOCAL_CFLAGS += -DRGB888_OFFSET_SPECIAL


ifeq ($(TARGET_PRODUCT), msm8960)
LOCAL_STATIC_LIBRARIES += libext_oppo_nvbk_lib
LOCAL_C_INCLUDES += vendor/qcom/proprietary/ext_oppo_nvbk_lib

LOCAL_CFLAGS += -DLINUXPC

#erase modem nv when major version modify for 12025 
LOCAL_CFLAGS += -DERASE_MODEM_NV_12025

#support NTFS filesystem
LOCAL_CFLAGS += -DNTFS_SUPPORT

endif

#######user difine macro end




ifeq ($(HAVE_SELINUX), true)
LOCAL_C_INCLUDES += external/libselinux/include
LOCAL_STATIC_LIBRARIES += libselinux
LOCAL_CFLAGS += -DHAVE_SELINUX
endif # HAVE_SELINUX

# This binary is in the recovery ramdisk, which is otherwise a copy of root.
# It gets copied there in config/Makefile.  LOCAL_MODULE_TAGS suppresses
# a (redundant) copy of the binary in /system/bin for user builds.
# TODO: Build the ramdisk image in a more principled way.

LOCAL_MODULE_TAGS := optional

#for android4.2
LOCAL_STATIC_LIBRARIES += libext4_utils_static

#for android4.1
#LOCAL_STATIC_LIBRARIES += libext4_utils

LOCAL_STATIC_LIBRARIES += \
    libz \
    libminzip \
    libunz \
    libmtdutils \
    libmincrypt \
    libedify \
    libcutils \
    libstdc++ \
    libc \
    libm \
    libcrecovery 

LOCAL_C_INCLUDES += system/extras/ext4_utils

include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := verifier_test.c verifier.c

LOCAL_MODULE := verifier_test

LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_MODULE_TAGS := tests

LOCAL_STATIC_LIBRARIES := libmincrypt libcutils libstdc++ libc

include $(BUILD_EXECUTABLE)

include $(commands_recovery_local_path)/minui/Android.mk
include $(commands_recovery_local_path)/minelf/Android.mk
include $(commands_recovery_local_path)/minzip/Android.mk
include $(commands_recovery_local_path)/minadbd/Android.mk
include $(commands_recovery_local_path)/mtdutils/Android.mk
include $(commands_recovery_local_path)/tools/Android.mk
include $(commands_recovery_local_path)/edify/Android.mk
include $(commands_recovery_local_path)/updater/Android.mk
include $(commands_recovery_local_path)/applypatch/Android.mk

#include $(commands_recovery_local_path)/libcrecovery/Android.mk

commands_recovery_local_path :=
