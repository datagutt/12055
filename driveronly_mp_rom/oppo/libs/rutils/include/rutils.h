/************************************************************************************
** File: - rutils.h
** VENDOR_EDIT
** Copyright (C), 2008-2012, OPPO Mobile Comm Corp., Ltd
** 
** Description: 
**      Add this file for customizing OPPO_CUSTOM partition!
** 
** Version: 1.0
** Date created: 11/12/2012
** Author: ZhiYong.Lin@Plf.Framework 
** 
** --------------------------- Revision History: --------------------------------
** 	<author>	                    		<data>					<desc>
** 	ZhenHai.Long@Prd.SysSRV          	11/02/2012      Init
** 	ZhiYong.Lin@Plf.Framework 				11/12/2012      Port and Modify
************************************************************************************/

#ifndef IRUTILS_H_
#define IRUTILS_H_

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <sys/stat.h> 
#include <sys/types.h> 


//ZhiYong.Lin@Plf.Framework port and modify for ROM project at 2012/12/11
//Jinquan.Lin@BasicDrv.BL, 2012/04/22, Modify for customizing OPPO_CUSTOM partition!
namespace android
{
    static int num_instance = 0;
    struct procmonitor_args{

            char* processName;
            int command;
            int procstat[2];
    };
    
    template<typename TYPE, void (TYPE::* thread_run)()>void* _thread_proc(void*);
    	
		class IRUtils : public IInterface{
				public:
							DECLARE_META_INTERFACE(RUtils);
							virtual int add(int nA, int nB) = 0;
							virtual int rutils_chmod(const char* path, const int mod) = 0;
							//#ifdef VENDOR_EDIT
 							//liumei@Plf.Framework, 2013/04/02, add remount for feature on/off. 
							virtual int rutils_cmd(const char* cmd) = 0;
							//#endif /* VENDOR_EDIT */
							virtual int rutils_getPidByName(const char* processName) = 0;
					    virtual int rutils_setSystemProperties(const char* key, const char* value) = 0;
					    virtual int rutils_custPartConfigItemUsbOps(const int opflag, const int value, const int voter) = 0;
					    virtual int rutils_custPartConfigItemClrFlagOps(const int opflag, const int source) = 0; 
					    virtual const char* rutils_custPartGetMcpId() = 0; 
					    
					    //linzy modify for remove these interface
							/*
							virtual int rutils_custPartCustPartCalibrateGsensorData(int *buf) = 0; 
							virtual int rutils_custPartCustPartRecoverGsensorData() = 0;
							*/
							
							virtual int rutils_custPartCustPartGetRebootNumber() = 0;
							virtual int rutils_custPartCustPartGetRebootReason() = 0;
							virtual int rutils_custPartCustPartClearRebootNumber() = 0;
		};
	
	
		class BnRUtils : public BnInterface<IRUtils>{
				public: 
							virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags )
							{
								return BBinder :: onTransact(code, data, reply, flags);
							}
		};	
	
		class RUtils : public BnRUtils{
				public:
				    	struct procmonitor_args pr_args;   
							static int instantiate();
							
							RUtils();
					
							int rutils_procMonitor();
							
							virtual int add(int nA, int nB);
							virtual int rutils_chmod(const char* path, const int mod);
							//#ifdef VENDOR_EDIT
 							//liumei@Plf.Framework, 2013/04/02, add remount for feature on/off.
							virtual int rutils_cmd(const char* cmd);
							//#endif /* VENDOR_EDIT */
							virtual int rutils_getPidByName(const char* processName);
							virtual char rutils_getProcStateByPid(const int pid, int proc_stat[2]);
							virtual int rutils_setSystemProperties(const char* key, const char* value);
							virtual int rutils_custPartConfigItemUsbOps(const int opflag, const int value, const int voter);
							virtual int rutils_custPartConfigItemClrFlagOps(const int opflag, const int source);
							virtual const char* rutils_custPartGetMcpId();
							
							//linzy modify for remove these interface
							/*
							virtual int rutils_custPartCustPartCalibrateGsensorData(int *buf);
							virtual int rutils_custPartCustPartRecoverGsensorData();
							*/
							
							virtual int rutils_custPartCustPartGetRebootNumber();
							virtual int rutils_custPartCustPartGetRebootReason();
							virtual int rutils_custPartCustPartClearRebootNumber();
							
							virtual ~RUtils();
							virtual status_t onTransact(uint32_t, const Parcel&, Parcel*, uint32_t);
							//#ifdef VENDOR_EDIT
 							//liumei@Plf.Framework, 2013/04/10, add dump interface for feature on/off.
							//oppo.zyx:dump demo
							//method :override dump interface
							virtual status_t dump(int fd, const Vector<String16>& args);
							//#endif /* VENDOR_EDIT */
		};
		
} // end of namespace android


#endif /* IRUTILS_H_ */
