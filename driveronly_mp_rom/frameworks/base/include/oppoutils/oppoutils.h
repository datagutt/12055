
#ifndef IOPPOUTILS_H_
#define IOPPOUTILS_H_

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <sys/stat.h> 
#include <sys/types.h> 

/*Jinquan.Lin@BasicDrv.BL, 2012/04/22, Modify for customizing OPPO_CUSTOM partition!*/
namespace android
{
    static int num_instance = 0;
    struct procmonitor_args{

            char* processName;
            int command;
            int procstat[2];
    };
    template<typename TYPE, void (TYPE::* thread_run)()>void* _thread_proc(void*);
	class IOPPOUtils : public IInterface{
	public:
		DECLARE_META_INTERFACE(OPPOUtils);
		virtual int add(int nA, int nB) = 0;
		virtual int oppoutils_chmod(const char* path, const int mod) = 0;
		virtual int oppoutils_getPidByName(const char* processName) = 0;
        virtual int oppoutils_setSystemProperties(const char* key, const char* value) = 0;
        virtual int oppoutils_hideProcessByPid(const unsigned int) = 0;
        virtual int oppoutils_unhideProcessBypid(const unsigned int) = 0;
        virtual int oppoutils_custPartConfigItemUsbOps(const int opflag, const int value, const int voter) = 0;
        virtual int oppoutils_custPartConfigItemClrFlagOps(const int opflag, const int source) = 0; 
        virtual const char* oppoutils_custPartGetMcpId() = 0; 
		virtual int oppoutils_custPartCustPartCalibrateGsensorData(int *buf) = 0; 
		virtual int oppoutils_custPartCustPartRecoverGsensorData() = 0;
		virtual int oppoutils_custPartCustPartGetRebootNumber() = 0;
		virtual int oppoutils_custPartCustPartGetRebootReason() = 0;
		virtual int oppoutils_custPartCustPartClearRebootNumber() = 0;
//#ifndef VENDOR_EDIT//mingqiang.guo@Prd.BasicDrv.Sensor, add 2012/11/15 add for ps revoer send pulse count 
		virtual int oppoutils_custPartCustPartAlsPsCalibratePsSendPulse(int *buf) = 0;
		virtual int oppoutils_custPartCustPartAlsPsRecoverPsSendPulse() = 0;
//#endif /*VENDOR_EDIT*/
//#ifndef VENDOR_EDIT////LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05 add for ps switch 
		virtual int oppoutils_custPartCustPartAlsPsSetPsSwitch() = 0;
		virtual int oppoutils_custPartCustPartAlsPsRecoverPsSwitch() = 0;
		virtual int oppoutils_custPartCustPartAlsPsGetPsSwitchStatus() = 0;
//#endif /*VENDOR_EDIT*/
		//#ifdef VENDOR_EDIT
		//Pantengjiao@GraphicTech.cpu_boot, 2013/09/18, Add for Open multiple CPU
		virtual void oppoutils_cpuBoost(const int bStart) = 0;
		//#endif /* VENDOR_EDIT */
	};
	class BnOPPOUtils : public BnInterface<IOPPOUtils>{
	public: 
		virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags ){
			return BBinder :: onTransact(code, data, reply, flags);
		}
			
	};	
	
	class OPPOUtils : public BnOPPOUtils{
	public:
    	struct procmonitor_args pr_args;   
		static int instantiate();
		OPPOUtils();

		int oppoutils_procMonitor();
		//#ifdef VENDOR_EDIT
		//Pantengjiao@GraphicTech.cpu_boot, 2013/09/18, Add for Open multiple CPU
        bool mCpuBoosted;
		void sysfs_write(const char *path, const char *s);
		void configure_hotplug(int bBoost);
		//#endif /* VENDOR_EDIT */
		virtual int add(int nA, int nB);
		virtual int oppoutils_chmod(const char* path, const int mod);
		virtual int oppoutils_getPidByName(const char* processName);
		virtual char oppoutils_getProcStateByPid(const int pid, int proc_stat[2]);
		virtual int oppoutils_setSystemProperties(const char* key, const char* value);
        virtual int oppoutils_hideProcessByPid(const unsigned int);
        virtual int oppoutils_unhideProcessBypid(const unsigned int);		
		virtual int oppoutils_custPartConfigItemUsbOps(const int opflag, const int value, const int voter);
		virtual int oppoutils_custPartConfigItemClrFlagOps(const int opflag, const int source);
		virtual const char* oppoutils_custPartGetMcpId();
		virtual int oppoutils_custPartCustPartCalibrateGsensorData(int *buf);
		virtual int oppoutils_custPartCustPartRecoverGsensorData();
		virtual int oppoutils_custPartCustPartGetRebootNumber();
		virtual int oppoutils_custPartCustPartGetRebootReason();
		virtual int oppoutils_custPartCustPartClearRebootNumber();
//#ifndef VENDOR_EDIT//mingqiang.guo@Prd.BasicDrv.Sensor, add 2012/11/15 add for ps revoer send pulse count 
		virtual int oppoutils_custPartCustPartAlsPsCalibratePsSendPulse(int *buf);
		virtual int oppoutils_custPartCustPartAlsPsRecoverPsSendPulse();
//#endif /*VENDOR_EDIT*/
//#ifndef VENDOR_EDIT////LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05 add for ps switch 
		virtual int oppoutils_custPartCustPartAlsPsSetPsSwitch() ;
		virtual int oppoutils_custPartCustPartAlsPsRecoverPsSwitch() ;
		virtual int oppoutils_custPartCustPartAlsPsGetPsSwitchStatus();
//#endif /*VENDOR_EDIT*/
		//#ifdef VENDOR_EDIT
		//Pantengjiao@GraphicTech.cpu_boot, 2013/09/18, Add for Open multiple CPU
		virtual void oppoutils_cpuBoost(const int bStart);
		//#endif /* VENDOR_EDIT */
		virtual ~OPPOUtils();
		virtual status_t onTransact(uint32_t, const Parcel&, Parcel*, uint32_t);	
	};
}


#endif /* IOPPOUTILS_H_ */
