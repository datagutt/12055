/********************************************************************************
 ** Copyright (C), 2008-2013, OPPO Mobile Comm Corp., Ltd
 ** VENDOR_EDIT, All rights reserved.
 ** 
 ** File: - ILockScreenManager.aidl
 ** Description: 
 **     interface of lockscreen manager service
 **     
 ** Version: 1.0
 ** Date: 2012-05-26
 ** Author: Qihu.Liu@Prd.DesktopApp.Keyguard
 ** 
 ** ------------------------------- Revision History: ----------------------------
 ** <author>                           <data>       <version>   <desc>
 ** ------------------------------------------------------------------------------
 ** Qihu.Liu@Prd.DesktopApp.Keyguard   2012-05-26   1.0         Create this moudle
 ********************************************************************************/

package com.oppo.individuationsettings.unlocker;
 
interface ILockScreenManager {

	/* callbacks used by apk lockscreen */
    void onKeyguardShow();
    void onKeyguardHide();
	
	/* 2 methods provided for policy */
	void showApkLockscreen();
	void hideApkLockscreen();
	
	/* used to notify apk screen turned on/off */
	void notifyScreenTurnedOn();
	void notifyScreenTurnedOff();
}
