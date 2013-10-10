#************************************************************************************
#** Copyright (C), 2000-2012, OPPO Mobile Comm Corp., Ltd
#** All rights reserved.
#** 
#** VENDOR_EDIT
#** 
#** Description: - 
#**      Java API for Dirac HD Effect
#** 
#** 
#** --------------------------- Revision History: --------------------------------
#** <author>		                      <data> 	<version >  <desc>
#** ------------------------------------------------------------------------------
#** guole@oppo.com	                     2011-10-26	  1.0       create file
#** guole@oppo.com	                     2011-10-26	  1.1       port it to 11071
#** Hongye.Jin@Pdt.Audio.AudioEffect&HAL  2012-06-19   1.2	    port it to 12021
#** ------------------------------------------------------------------------------
#**
#************************************************************************************/

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

#ifdef VENDOR_EDIT
#Guole@MultimediaSrv.AudioEffect, 2012.03.20, conditionally compile Dirac HD
#ifeq ($(strip $(OPPO_DIRAC_SUPPORT)), yes)
#endif

LOCAL_PREBUILT_LIBS := DiracLib.a
include $(BUILD_MULTI_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES:= \
	EffectDirac.cpp

LOCAL_CFLAGS+= -O2

LOCAL_SHARED_LIBRARIES := \
	libcutils

LOCAL_STATIC_LIBRARIES := \
        DiracLib

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/soundfx
LOCAL_MODULE:= libdirac

ifeq ($(TARGET_OS)-$(TARGET_SIMULATOR),linux-true)
LOCAL_LDLIBS += -ldl
endif

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES += libdl
endif

LOCAL_C_INCLUDES := \
	$(call include-path-for, graphics corecg)

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

#endif #ifeq ($(strip $(OPPO_DIRAC_SUPPORT)), yes)
