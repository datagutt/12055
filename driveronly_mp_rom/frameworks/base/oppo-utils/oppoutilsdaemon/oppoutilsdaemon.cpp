/************************************************************************************
** File: - rutilsdaemon.cpp
** VENDOR_EDIT
** Copyright (C), 2008-2012, OPPO Mobile Comm Corp., Ltd
** 
** Description: 
**      Add this file for customizing OPPO_CUSTOM partition!
** 
** Version: 0.1
** Date created: 20:34:23,11/02/2012
** Author: ZhenHai.Long@Prd.SysSRV
** 
** --------------------------- Revision History: --------------------------------
** 	<author>	                    <data>			<desc>
** ZhenHai.Long@Prd.SysSRV          11/02/2012      Init
** ZhiYong.Lin@Prd.SysSRV                29/05/2013                     port and modify
************************************************************************************/
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
#include <android/log.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <oppoutils/oppoutils.h>
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "OppoUtilsDaemon", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , "OppoUtilsDaemon", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , "OppoUtilsDaemon", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , "OppoUtilsDaemon", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "OppoUtilsDaemon", __VA_ARGS__)
using namespace android;
int main()
{
     
    
	sp<ProcessState> Proc(ProcessState::self());

	//sp<IServiceManager> sm = defaultServiceManager();

	//LOGI("SystemServiceManager:%p", sm.get());

    
	int nR = OPPOUtils::instantiate();
    if(nR == -1){
		LOGD("SystemServiceManager:%d", nR);
        return nR;
    }
	
	

	ProcessState::self()->startThreadPool();	

	IPCThreadState::self()->joinThreadPool();
	
}
