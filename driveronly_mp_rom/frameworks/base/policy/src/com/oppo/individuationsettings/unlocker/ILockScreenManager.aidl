/************************************************************************************
** Copyright (C), 2000-2012, OPPO Mobile Comm Corp., Ltd
** All rights reserved.
** 
** VENDOR_EDIT
** 
** Description: - 
**      interface of lockscreen manager service
** 
** 
** --------------------------- Revision History: --------------------------------
** <author>		                      <data> 	<version >  <desc>
** ------------------------------------------------------------------------------
** Qihu.Liu@Pdt.DesktopApp.Keyguard  2012/05/26   1.0	    create file
** ------------------------------------------------------------------------------
** 
************************************************************************************/
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
