/************************************************************************************
** Copyright (C), 2000-2013, OPPO Mobile Comm Corp., Ltd
** All rights reserved.
** 
** VENDOR_EDIT
** 
** Description: - 
**      get hardware platform
** 
** 
** --------------------------- Revision History: --------------------------------
** <author>		                      <data> 	<version >  <desc>
** ------------------------------------------------------------------------------
** Zoufeng@Plf.Mediasrv.Player  2013/01/25   1.0	    create file
** ------------------------------------------------------------------------------
** 
************************************************************************************/

#ifndef HARDWARE_PLATFORM_H_
#define HARDWARE_PLATFORM_H_

namespace android {

class HardwarePlatform  {	
public:
int isQcomHardwarePlatform();
int isMtkHardwarePlatform();
static  HardwarePlatform* getInstance();
private:
	HardwarePlatform();	
	~HardwarePlatform();
	static HardwarePlatform* mHardwarePlatform;
	bool mIsQcom;
	bool mIsMtk;
};

}// namespace android

#endif
