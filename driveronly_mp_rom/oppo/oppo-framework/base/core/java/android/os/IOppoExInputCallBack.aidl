/********************************************************************************
 ** Copyright (C), 2008-2013, OPPO Mobile Comm Corp., Ltd
 ** VENDOR_EDIT, All rights reserved.
 ** 
 ** File: - IOppoExInputCallBack.aidl
 ** Description: 
 **     oppo ex service, three pointers move shot screen
 **     
 ** Version: 1.0
 ** Date: 2013-08-14
 ** Author: ZhangJun@Plf.Framework
 ** 
 ** ------------------------------- Revision History: ----------------------------
 ** <author>                        <data>       <version>   <desc>
 ** ------------------------------------------------------------------------------
 ** ZhangJun@Plf.Framework         2013-08-14   1.0         Create this moudle
 ********************************************************************************/

package android.os;

import android.view.InputEvent;

interface IOppoExInputCallBack {

	void onInputEvent(in InputEvent event);
}