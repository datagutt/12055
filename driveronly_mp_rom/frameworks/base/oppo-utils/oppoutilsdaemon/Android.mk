####################################################################################
## File: - Android.mk
## VENDOR_EDIT
## Copyright (C), 2008-2012, OPPO Mobile Comm Corp., Ltd
## 
## Description: 
##      Add this file for customizing OPPO_CUSTOM partition!
## 
## Version: 0.1
## Date created: 20:34:23,11/02/2012
## Author: ZhenHai.Long@Prd.SysSRV
## 
## --------------------------- Revision History: --------------------------------
## 	<author>	                    <data>			<desc>
## ZhenHai.Long@Prd.SysSRV                11/02/2012                     Init
## ZhiYong.Lin@Prd.SysSRV                29/05/2013                     port and modify
####################################################################################
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES:=oppoutilsdaemon.cpp 
LOCAL_SHARED_LIBRARIES:=libutils\
	libbinder\
	liboppoutils
LOCAL_LDLIBS:= -llog
LOCAL_MODULE:= oppoutilsdaemon
include $(BUILD_EXECUTABLE)