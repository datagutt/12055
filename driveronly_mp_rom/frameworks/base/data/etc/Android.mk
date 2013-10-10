#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(my-dir)

########################
include $(CLEAR_VARS)

LOCAL_MODULE := platform.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
#
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

########################
#include $(CLEAR_VARS)

#LOCAL_MODULE := required_hardware.xml

#LOCAL_MODULE_TAGS := optional

#LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
#
#LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

#LOCAL_SRC_FILES := $(LOCAL_MODULE)

#include $(BUILD_PREBUILT)

#ifdef VENDOR_EDIT
#ZhongHaiping@Prd.MidWare.Theme,2012/12/28,for theme, upgrade android4.1
#ifeq ($(strip $(MTK_THEMEMANAGER_APP)),yes)
ifdef VENDOR_EDIT
########################
include $(CLEAR_VARS)

LOCAL_MODULE := thememap.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/theme
#
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/theme

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)
endif
#ifdef VENDOR_EDIT
#ZhongHaiping@Prd.MidWare.Theme,2012/12/28,for theme, upgrade android4.1
########################
include $(CLEAR_VARS)

LOCAL_MODULE := coloridmap.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/theme
#
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/theme

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)
#endif /* VENDOR_EDIT */

#ifdef VENDOR_EDIT
#WangLan@Plf.Framework add for nearme id
########################
include $(CLEAR_VARS)

LOCAL_MODULE := oppo_package.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
#
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)
#endif /* VENDOR_EDIT */

