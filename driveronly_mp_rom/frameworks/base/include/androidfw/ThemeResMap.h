/* VENDOR_EDIT
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
// Theme Manager Map class.
//

#ifndef __LIBS_THEMERESMAP_H
#define __LIBS_THEMERESMAP_H

#include <androidfw/Asset.h>
#include <androidfw/AssetDir.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>
#include <utils/Vector.h>
#include <utils/String16.h>
#include <utils/ZipFileRO.h>
#include <utils/threads.h>

namespace android {
	//compatiable theme color
	static const int THEME_COLOR_START = 0x01060800;
	static const int THEME_COLOR_END = 0x01060b50;
	static const int THEME_COLOR_INDEX = 0x00030000;
	static const char DEFAULT_THEME_PATH[] = "/system/framework/framework-res.apk";
	static const int THEME_COLOR [] = {
		0x0106080d, //large_text_color
		0x0106080e, //large_text_color_select
		0x0106082a, //multilist_text_disabled_color
		0x0106082b, //multilist_text_disabled_color_select
		0x0106082c, //launcher_mainmenu_default_text_color
		0x0106082d, //candidate_color
		0x01060816, //title_text_color
		0x01060809, //listview_cacheColorHint
		0x0106080a, //listview_background_color
		0x0106080b, //listview_item_color
		0x01060800, //oppo_alert_dialog_button_text_color
		0x01060801, //oppo_alert_dialog_content_text_color
		0x01060802, //oppo_alert_dialog_title_text_color
		0x01060803, //oppo_alert_dialog_title_text_shadow_color
		0x01060804, //oppo_alert_dialog_button_text_shadow_color
		0x01060829, //oppo_dial_text_color
		//0x0106082e,	//ic_launcher_tohome_textcolor
	};
	static const int THEME_COLOR_LENGTH = sizeof(THEME_COLOR)/sizeof(int);
}

#endif /* __LIBS_THEMERESMAP_H */
