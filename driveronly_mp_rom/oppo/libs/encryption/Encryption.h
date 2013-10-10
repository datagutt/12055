/************************************************************************************
** File: - Encryption.h
** VENDOR_EDIT
** Copyright (C), 2008-2012, OPPO Mobile Comm Corp., Ltd
** 
** Description: 
**      Add this file for customizing OPPO_CUSTOM partition!
** 
** Version: 1.0
** Date created: 12/12/2012
** Author: ZhiYong.Lin@Plf.Framework
** 
** --------------------------- Revision History: --------------------------------
** 	<author>	                    <data>					<desc>
** 	ZhiYong.Lin@Plf.Framework     12/12/2012      Init
************************************************************************************/

#include "JNIHelp.h"
#include "jni.h"


#include <androidfw/AssetManager.h>
#include <androidfw/ResourceTypes.h>

#ifndef _Included_Encryption_H
#define _Included_Encryption_H


namespace android {
	#define DES_KEY "MXVle5k3"
	#define KEY_PATH "META-INF/key"
	#define PERSIST_KEY  "persist.sys.oppo.device.imei"
	#define PERSIST_KEY2 "persist.sys.oppo.device.imei2"
	#define IMEI_KEY_LENGTH 15

	extern int testaaa();
	extern int openXmlAssetInOppoEncryption(int cookie, const char* fileName, AssetManager* am);
	extern int encryptFileInOppoEncryption(const char* plainFileName, const char* encryptFileName);
	extern int decryptFileInOppoEncryption(const char* plainFileName, const char* encryptFileName);
	
	extern int decryptThemeFileInOppoEncryption(const char* keyFileName, const char* encryptThemeFileName, const char* decryptThemeFileName);
	
}

#endif
