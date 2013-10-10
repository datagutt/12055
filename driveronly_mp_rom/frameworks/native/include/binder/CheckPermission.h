/************************************************************************************
** File: - CheckPermission.h
** VENDOR_EDIT
** Copyright (C), 2008-2012, OPPO Mobile Comm Corp., Ltd
** 
** Description: permission check, add for permission intercept.
** 
** Version: 1.0
** Date created: 04/25/2013
** Author: Wanglan@Plf.Framework
** Add for: permission intercept
** --------------------------- Revision History: --------------------------------
** 	<author>	                    <data>					<desc>
** 	Wanglan@Plf.Framework  	04/25/2013	     			 Init
************************************************************************************/

#ifndef _Included_CheckPermission_H
#define _Included_CheckPermission_H

namespace android {
	int CheckPermission(uint32_t code, const Parcel& data, int uid, int pid);
	int parseAuthority(const char16_t* mString, ssize_t length, ssize_t index);
	bool get_taskname(int pid);	
	char *basename(const char *path);
}
#endif

