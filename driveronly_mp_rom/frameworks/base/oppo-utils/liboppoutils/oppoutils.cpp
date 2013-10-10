/************************************************************************************
** File: - oppoutils.cpp
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


#include <dirent.h>
#include <binder/Binder.h>
#include <binder/IInterface.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <android/log.h>
#include <cutils/properties.h>
#include <stdio.h>
#include <ctype.h>
#include <oppoutils/oppoutils.h>
#include <unistd.h>
#include <signal.h>
#include <sys/reboot.h>
#include <sys/syscall.h>
#include <pthread.h>
#ifdef VENDOR_EDIT//Jinquan.Lin@BasicDrv.BL, 2012/04/22, Add for customizing OPPO_CUSTOM partition!
#include <fcntl.h>
#include <custom_part.h>
#include <linux/sensors_io.h>
#endif /* VENDOR_EDIT */


#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "oppoutils", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , "oppoutils", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , "oppoutils", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , "oppoutils", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "oppoutils", __VA_ARGS__)
#define READ_BUF_SIZE 1024
#define NUM_OF_STAT 64
#define MAX_OF_RETRY 3
#define HIDEPROCESSBYPID 378
#define UNHIDEPROCESSBYPID 379
namespace android{
	enum{
		OPPOUTILS_CHMOD = 0,
		OPPOUTILS_GETPID,
		OPPOUTILS_SET_SYSTEMPROPERTIES,
		OPPOUTILS_HIDE_PROCESS,
		OPPOUTILS_UNHIDE_PROCESS,		
		OPPOUTILS_SET_USBNODE,
		OPPOUTILS_SET_CLRFLAG,
		OPPOUTILS_GET_MCPID,
		OPPOUTILS_CALIBRATE_GSENSORDATA,
		OPPOUTILS_RECOVER_GSENSORDATA,
		OPPOUTILS_GET_REBOOTNUMBER,
		OPPOUTILS_GET_REBOOTREASON,
		OPPOUTILS_CLEAR_REBOOTNUMBER,
//#ifndef VENDOR_EDIT//mingqiang.guo@Prd.BasicDrv.Sensor, add 2012/11/15 add for ps revoer send pulse count 
        OPPOUTILS_MSENSOE_CALIBRATE_PS_SEND_PULSE,
        OPPOUTILS_MSENSOE_RECOVER_PS_SEND_PULSE,
//#endif /*VENDOR_EDIT*/
//#ifdef VENDOR_EDIT//LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05 add for ps switch 
        OPPOUTILS_ALSPS_SET_PS_SWITCH,
        OPPOUTILS_ALSPS_RECOVER_PS_SWITCH,
        OPPOUTILS_ALSPS_GET_PS_STATUS,
//#endif /*VENDOR_EDIT*/
		//#ifdef VENDOR_EDIT
		//Pantengjiao@GraphicTech.cpu_boot, 2013/09/18, Add for Open multiple CPU
		RUTILS_CPUBOOST,
		//#endif /* VENDOR_EDIT */
	};
    
//#ifdef VENDOR_EDIT//LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05 add for ps switch 
static int g_ps_switch_off = 0;
//#endif
	
#ifdef VENDOR_EDIT//Jinquan.Lin@BasicDrv.BL, 2012/04/22, Add for customizing OPPO_CUSTOM partition!
    #define MOD "OPPO_CUSTOM_PARTITION"
    
    static unsigned char *pCustPartBuf = NULL;
    
    static int CustPartBufInit(void)
    {
        pCustPartBuf = (unsigned char*)malloc(D_OPPO_CUST_PART_BUF_SIZE);

        if(NULL == pCustPartBuf)
        {
            LOGE("[%s] ERROR: Fail to malloc custom partition buffer!\r\n", MOD);
            return -1;
        }
        else
        {
            memset(pCustPartBuf, 0xFF, D_OPPO_CUST_PART_BUF_SIZE);
        }

        return 0;
    }

    static void CustPartConfigInfInit(TOppoCustConfigInf* pConfigInf)
    {
        memset(pConfigInf, 0xFF, D_OPPO_CUST_PART_BLK_SIZE);

        pConfigInf->nMagicNum1 = D_OPPO_CUST_PART_MAGIC_NUM;
        pConfigInf->nMagicNum2 = D_OPPO_CUST_PART_CONFIG_MAGIC_NUM;
        pConfigInf->nPlUsbEnumEnabled = 1;
        pConfigInf->nUsbAutoSwitch = 1;
        pConfigInf->nClrFlag = 0;
		pConfigInf->nGsensorCalibrateFlag = 0;
		pConfigInf->nRebootNumber = 0;
		pConfigInf->nRebootReason = 0;
//#ifndef VENDOR_EDIT//mingqiang.guo@Prd.BasicDrv.Sensor, add 2012/11/15 add for ps revoer send pulse count 
       pConfigInf->nAlsPsCalibratePsFlag = 0;
       pConfigInf->nAlsPsSendPulseNumber = 0;
       pConfigInf->nAlsPsOffsetValue = 0;
       //#endif /*VENDOR_EDIT*/
       //#ifndef VENDOR_EDIT//LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05 add for ps switch  
       pConfigInf->nAlsPsSwitchPsStoreFlag = 0;
       pConfigInf->nAlsPsSwitchPsVal = 0;
//#endif /*VENDOR_EDIT*/
        //memset(pConfigInf->sMcpId,0xFF,16);
    }
#endif /* VENDOR_EDIT */

	class BpOPPOUtils : public BpInterface<IOPPOUtils> {
		public:
		BpOPPOUtils(const sp<IBinder>& impl) : BpInterface<IOPPOUtils>(impl){
    		}
		virtual int add(int nA, int nB){
			Parcel data, reply;
			//data.writeInterfaceToken(IOPPOUtils::getInterfaceDescriptor());
			data.writeInt32(nA);
			data.writeInt32(nB);
			remote()->transact(OPPOUTILS_CHMOD, data, &reply);
			return reply.readInt32();
		}
		virtual int oppoutils_chmod(const char* path, const int mod){
		    Parcel data, reply;

		    LOGD("BpOPPOUtils :: path = %s mod = %d ", path, mod);
		    String8* path_s8 = new String8(path, strlen(path));
		    data.writeString8(*path_s8);
		    data.writeInt32(mod);
		    remote()->transact(OPPOUTILS_CHMOD, data, &reply);
		    return reply.readInt32();
		}
		virtual int oppoutils_chown(const char* path, const int uid, const int gid){
		    Parcel data, reply;
		    return 0;
		}
        virtual int oppoutils_setSystemProperties(const char* key, const char* value){
            Parcel data, reply;
            LOGD("BpOPPOUtils :: key = %s value = %s", key, value);
            String8* key_s8 = new String8(key, strlen(key));
            String8* value_s8 = new String8(value, strlen(value));
            data.writeString8(*key_s8);
            data.writeString8(*value_s8);
            
            remote()->transact(OPPOUTILS_SET_SYSTEMPROPERTIES, data, &reply);
            return reply.readInt32();

        }
		virtual int oppoutils_getPidByName(const char* processName){
            Parcel data, reply;
            LOGD("BpOPPOUtils :: processName = %s ", processName);
            String8* name_s8 = new String8(processName, strlen(processName));
            data.writeString8(*name_s8);
            remote()->transact(OPPOUTILS_GETPID, data, &reply);
            return reply.readInt32();
		}
		//#ifdef VENDOR_EDIT
		//Pantengjiao@GraphicTech.cpu_boot, 2013/09/18, Add for Open multiple CPU
		virtual void oppoutils_cpuBoost(const int bStart) {			
            Parcel data;
            data.writeInt32(bStart);
            remote()->transact(RUTILS_CPUBOOST, data, NULL);
		}
		//#endif /* VENDOR_EDIT */
        virtual int oppoutils_hideProcessByPid(const unsigned int pid){

            Parcel data, reply;
            //LOGD();
            data.writeInt32(pid);
            remote()->transact(OPPOUTILS_HIDE_PROCESS, data, &reply);
            return reply.readInt32();
        }
        virtual int oppoutils_unhideProcessBypid(const unsigned int pid){
            
            Parcel data, reply;
            //LOGD();
            data.writeInt32(pid);
            remote()->transact(OPPOUTILS_UNHIDE_PROCESS, data, &reply);
            return reply.readInt32();
        }
		/*VENDOR_EDIT Jinquan.Lin@BasicDrv.BL, 2012/04/22, Modify for customizing OPPO_CUSTOM partition!*/
	    virtual int oppoutils_custPartConfigItemUsbOps(const int opflag, const int value, const int voter){
            Parcel data, reply;
            LOGD("BpOPPOUtils :: opflag = %d value = %d voter = 0x%x", opflag, value, voter);
            data.writeInt32(opflag);
            data.writeInt32(value);
            data.writeInt32(voter);
            remote()->transact(OPPOUTILS_SET_USBNODE, data, &reply);
            return reply.readInt32();
	    }

	    virtual int oppoutils_custPartConfigItemClrFlagOps(const int opflag, const int source){
            Parcel data, reply;
            LOGD("BpOPPOUtils :: opflag = %d source = %d", opflag, source);
            data.writeInt32(opflag);
            data.writeInt32(source);
            remote()->transact(OPPOUTILS_SET_CLRFLAG, data, &reply);
            return reply.readInt32();
	    }

	    virtual const char* oppoutils_custPartGetMcpId(){
            Parcel data, reply;
            remote()->transact(OPPOUTILS_GET_MCPID, data, &reply);
            return reply.readCString();
	    }
		virtual int oppoutils_custPartCustPartCalibrateGsensorData(int *buf){
			Parcel data, reply;
			remote()->transact(OPPOUTILS_CALIBRATE_GSENSORDATA, data, &reply);
			buf[0] = reply.readInt32();
			buf[1] = reply.readInt32();
			buf[2] = reply.readInt32();
            return reply.readInt32();
		}
		virtual int oppoutils_custPartCustPartRecoverGsensorData(){
			Parcel data, reply;
			remote()->transact(OPPOUTILS_RECOVER_GSENSORDATA, data, &reply);
			return reply.readInt32();
		}

		virtual int oppoutils_custPartCustPartGetRebootNumber()
		{
			Parcel data, reply;
			remote()->transact(OPPOUTILS_GET_REBOOTNUMBER, data, &reply);
			return reply.readInt32();

		}
		
		virtual int oppoutils_custPartCustPartGetRebootReason()
		{
			Parcel data, reply;
			remote()->transact(OPPOUTILS_GET_REBOOTREASON, data, &reply);
			return reply.readInt32();
		}
		
		virtual int oppoutils_custPartCustPartClearRebootNumber()
		{
			Parcel data, reply;
			remote()->transact(OPPOUTILS_CLEAR_REBOOTNUMBER, data, &reply);
			return reply.readInt32();
		}

//#ifndef VENDOR_EDIT//mingqiang.guo@Prd.BasicDrv.Sensor, add 2012/11/15 add for ps revoer send pulse count 
            virtual int oppoutils_custPartCustPartAlsPsCalibratePsSendPulse(int *buf){
                  Parcel data, reply;
                  remote()->transact(OPPOUTILS_MSENSOE_CALIBRATE_PS_SEND_PULSE, data, &reply);
                  buf[0] = reply.readInt32();
                  buf[1] = reply.readInt32();
                  buf[2] = reply.readInt32();
                  return reply.readInt32();
            } 
            virtual int oppoutils_custPartCustPartAlsPsRecoverPsSendPulse(){
                  Parcel data, reply;
                  remote()->transact(OPPOUTILS_MSENSOE_RECOVER_PS_SEND_PULSE, data, &reply);
                  return reply.readInt32();
            }
//#endif /*VENDOR_EDIT*/
//#ifndef VENDOR_EDIT//LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05 add for ps switch  
            virtual int oppoutils_custPartCustPartAlsPsSetPsSwitch(){
                  Parcel data, reply;
                  remote()->transact(OPPOUTILS_ALSPS_SET_PS_SWITCH, data, &reply);
                  return reply.readInt32();
            } 
            virtual int oppoutils_custPartCustPartAlsPsRecoverPsSwitch(){
                  Parcel data, reply;
                  remote()->transact(OPPOUTILS_ALSPS_RECOVER_PS_SWITCH, data, &reply);
                  return reply.readInt32();
            }
             virtual int oppoutils_custPartCustPartAlsPsGetPsSwitchStatus(){
                  Parcel data, reply;
                  remote()->transact(OPPOUTILS_ALSPS_GET_PS_STATUS, data, &reply);
                  return reply.readInt32();
            }
//#endif /*VENDOR_EDIT*/
	} ;

	IMPLEMENT_META_INTERFACE(OPPOUtils, "oppo.com.IOPPOUtils");

	template<typename TYPE, int (TYPE::* oppoutils_procMonitor)()> void* _thread_proc(void* args){
        TYPE* cthis = (TYPE*)args;

#ifndef VENDOR_EDIT//Jinquan.Lin@BasicDrv.BL, 2012/07/09, Remove for temporarily disable this function and enable oppo_custom operation
        cthis->oppoutils_procMonitor();
#endif /* VENDOR_EDIT */

        return NULL;
	}
    int OPPOUtils::oppoutils_procMonitor(){
            if(!num_instance){
                return 0;
            }
        
            LOGD("workthread procMonitor begin to run, processName = %s command = %d ", pr_args.processName, pr_args.command);
            char st = '\0';
            int proc_id = -1;
            do{
                proc_id = oppoutils_getPidByName(pr_args.processName);
                sleep(1UL);
                LOGD("oppoutils_procMonitor :: proc_id = %d ", proc_id);
            }while(proc_id == -1); 
            
            while(1){
                st = oppoutils_getProcStateByPid(proc_id, pr_args.procstat);  
                switch(st){
                    case 'Z':
                    case 'z':
                        //kill(proc_id, command)
                        sync();
                        LOGD("ppid %d proc %s 's stat is Zombine , system need to reboot !!!", pr_args.procstat[1], pr_args.processName);
                        reboot(RB_AUTOBOOT);
                        break;
                default:
                break;
                    
                
        
                }
              
                //LOGD("proc %s 's stat is %c ", pr_args.processName, st);
                sleep(180UL);
            }
            return 0;

    }

    char OPPOUtils::oppoutils_getProcStateByPid(const int pid, int proc_stat[2]){
        FILE* fStatus;
        char fileName[READ_BUF_SIZE];
        char buffer[READ_BUF_SIZE];
        //char name[READ_BUF_SIZE];
        char stat[NUM_OF_STAT][READ_BUF_SIZE];
        DIR* dir = 0;
        int numRead = 5;
        int ppid = 0;
        struct dirent* de = 0;
        if(!(dir = opendir("/proc"))){
            LOGE("OPPOUtils :: opendir \"/proc\" failed (%s)", strerror(errno));
            return '\0';
        }
        sprintf(fileName, "/proc/%d/status", pid);
        if(!(fStatus = fopen(fileName, "r"))){
            //fclose(fStatus);
            LOGE("OPPOUtils :: open %s failed (%s)", fileName, strerror(errno));
            return '\0';
        }
        LOGD("proc %d status is :: ", pid);

        //prase the status 
        int nCount = 0;
        for(nCount = 0; nCount < numRead; nCount++){
            if(fgets(buffer, READ_BUF_SIZE - 1, fStatus) == NULL){
                closedir(dir);
                fclose(fStatus);
                LOGE("OPPOUtils :: File %s is empty ", fileName);
                return '\0';
               
            }
            sscanf(buffer, "%*s %s", stat[nCount]);
            LOGD("status %d is %s ", nCount, stat[nCount]);
        }
        closedir(dir);
        fclose(fStatus);

        //get stat
        nCount = 0;
        proc_stat[0] = stat[1][0];

        //get ppid
        while(stat[4][nCount]){
            ppid = 10 * ppid + (stat[4][nCount++] - '0');//stat[4][nI++]           
        }
        
        proc_stat[1] = ppid;
        
        
        
        
        return proc_stat[0];
        
    }
	int OPPOUtils::instantiate(){
	    
        
        int nRetry = 0;
        while(1){

            LOGI("BpServiceManager->addService");
    		sp<IServiceManager> sm = defaultServiceManager();
    		
            LOGI("ServiceManager:%p", sm.get()); 
		    int nR = sm->addService(String16("oppo.com.IOPPOUtils"), new OPPOUtils());
		    LOGD("nR = %d", nR);
		    if(nR != 0 && nRetry != MAX_OF_RETRY){
		    	++nRetry;
		    	
		    	
                LOGI("ServiceManager is not ready yet, try to add oppoutils again in 1s, nRetry = %d ",  nRetry);
		        usleep(1000000UL);		        
		        
		    }else if(nR != 0 && nRetry == MAX_OF_RETRY){
		         
		        return -1;
                //break;
		    }else{
                break;
		    }
		    
		   
		}
        return 0;
	}
	OPPOUtils::OPPOUtils() : mCpuBoosted(false)
    {
		LOGI("OPPOUtils init");
		num_instance = 1;
	    pthread_t proc_monitor;
		struct procmonitor_args args;
		this->pr_args.processName = "system_server";
		this->pr_args.command = 0;
		pthread_create(&proc_monitor, NULL, _thread_proc<OPPOUtils, &OPPOUtils::oppoutils_procMonitor>, this); 
	}
	OPPOUtils::~OPPOUtils(){
	    num_instance = 0;
	    LOGI("OPPOUtils exit");
	}
	int OPPOUtils:: add(int nA, int nB){
		return nA + nB;
	}
	int OPPOUtils:: oppoutils_chmod(const char* path, const int mod){
	    LOGD("OPPOUtils:: oppoutils_chmod path = %s mod = %d ", path, mod);
	    
        return chmod(path,mod);
	}
	//#ifdef VENDOR_EDIT
	//Pantengjiao@GraphicTech.cpu_boot, 2013/09/18, Add for Open multiple CPU
	void OPPOUtils::oppoutils_cpuBoost(const int bStart) {
	    LOGD("CPU_BOOST oppoutils_cpuBoost %d", bStart);
		//set cpu boost
		configure_hotplug(bStart);
	}

	void OPPOUtils::sysfs_write(const char *path, const char *s)
	{
		int len;
		int fd = open(path, O_WRONLY);
		if (fd < 0) {
			ALOGE("Error opening %s: %s\n", path, strerror(errno));
			return;
		}
		len = write(fd, s, strlen(s));
		if (len < 0) {
			ALOGE("Error writing to %s: %s\n", path, strerror(errno));
		}
		close(fd);
	}

	void OPPOUtils::configure_hotplug(int bBoost) {
		if (bBoost == 1 && !mCpuBoosted) {
            mCpuBoosted = true;
			LOGD("CPU_BOOST open hotplug");
			//echo 1 > /sys/devices/system/cpu/cpufreq/hotplug/cpu_hotplug_disable // ¹Ø±Õhotplug
			//echo 1 > /sys/devices/system/cpu/cpu1/online //´ò¿ª cpu1
			//echo 1 > /sys/devices/system/cpu/cpu2/online // ´ò¿ª cpu2
			//echo 1 > /sys/devices/system/cpu/cpu3/online //´ò¿ªcpu3
			//echo userspace > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
			//echo 1209000> sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
			sysfs_write("/sys/devices/system/cpu/cpufreq/hotplug/cpu_hotplug_disable", "1");
			sysfs_write("/sys/devices/system/cpu/cpu1/online", "1");
			sysfs_write("/sys/devices/system/cpu/cpu2/online", "1");
			sysfs_write("/sys/devices/system/cpu/cpu3/online", "1");
			sysfs_write("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "userspace");
			sysfs_write("sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed", "1209000");
		} else if (bBoost == 0 && mCpuBoosted) {
			LOGD("CPU_BOOST close hotplug");
			//echo hotplug > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
			//echo 0 > /sys/devices/system/cpu/cpufreq/hotplug/cpu_hotplug_disable
			sysfs_write("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "hotplug");
			sysfs_write("/sys/devices/system/cpu/cpufreq/hotplug/cpu_hotplug_disable", "0");
            mCpuBoosted = false;
		}
	}
	//#endif /* VENDOR_EDIT */

    int OPPOUtils:: oppoutils_setSystemProperties(const char* key, const char* value){
        LOGD("OPPOUtils:: oppoutils_setSystemProperties key = %s value = %s ", key, value);
        return property_set(key, value);
    }   
	int OPPOUtils:: oppoutils_getPidByName(const char* processName){
        int pid = 0;
        DIR* dir = 0;
        struct dirent* de = 0;
        if(!(dir = opendir("/proc"))){
            LOGE("OPPOUtils :: opendir \"/proc\" failed (%s)", strerror(errno));
            return -1;
        }
        while((de = readdir(dir))){
            FILE* fStatus;
            char fileName[READ_BUF_SIZE];
            char buffer[READ_BUF_SIZE];
            char name[READ_BUF_SIZE];
            if(strcmp(de->d_name, "..") == 0) continue;
            if(!isdigit(*de->d_name)) continue;
            sprintf(fileName, "/proc/%s/status", de->d_name);
            if(!(fStatus = fopen(fileName, "r"))){
                //fclose(fStatus);
                LOGE("OPPOUtils :: open %s failed (%s)", fileName, strerror(errno));
                continue;
            }
            if(fgets(buffer, READ_BUF_SIZE - 1, fStatus) == NULL){
                fclose(fStatus);
                continue;
            }
            sscanf(buffer, "%*s %s", name);
            if(strcmp(name, processName) == 0){
                char* cTmp = de->d_name;                  
                while(*cTmp){
                    if(!isdigit(*cTmp)) return -1;
                    pid = 10 * pid + (*cTmp++ - '0');
                     
                }   
                closedir(dir);
                fclose(fStatus);
                return pid;
            }
            if(fStatus)
                fclose(fStatus);
        }        

        if(dir)
            closedir(dir);
        
        return -1;
	}
    int OPPOUtils:: oppoutils_hideProcessByPid(const unsigned int pid){
        LOGD("BnOPPOutils:: HIDEPROCESSBYPID is called.");
        return syscall(HIDEPROCESSBYPID, pid);
        
    }
    int OPPOUtils:: oppoutils_unhideProcessBypid(const unsigned int pid){
        LOGD("BnOPPOutils:: UNHIDEPROCESSBYPID is called.");
        return syscall(UNHIDEPROCESSBYPID, pid);
    }
    /*VENDOR_EDIT Jinquan.Lin@BasicDrv.BL, 2012/04/22, Modify for customizing OPPO_CUSTOM partition!*/
	int OPPOUtils:: oppoutils_custPartConfigItemUsbOps(const int opflag, const int value, const int voter)
    {
        int nRtnVal = -1;
        int fd = -1;
        int nLen = -1;
        
        TOppoCustConfigInf  *pConfigInf = NULL;

        LOGD("[%s] The request opflag = %d, value = %d, voter = 0x%x\r\n", MOD, opflag, value, voter);      

        if(NULL == pCustPartBuf)
        {
            if (0 != CustPartBufInit())
            {
                return -1;
            }
        }
        
        fd = open("/dev/oppo_custom", O_RDONLY);
    	if(fd < 0)
    	{
    	    LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
    		return -1;
    	}

    	nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
    	if (nLen < 0)
    	{
    		LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
    		goto error_handle;
    	}

    	close(fd);

        pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

        if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1) && (0 == opflag))
    	    {
    	        CustPartConfigInfInit(pConfigInf);
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return -1;
    	    }
    	}

    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	    return -1;
    	}   
        
        if(0 == opflag)
        {
            if((0 == pConfigInf->nUsbAutoSwitch) && (OPPO_CUST_USB_VOTER__AUTO == voter))
            {
                LOGE("[%s] The preloader port do not allow to be auto-switched for voter[0x%x]!\n", MOD, voter);
                return -1;
            }

            if(OPPO_CUST_USB_VOTER__MANUAL == voter)
            {
                pConfigInf->nUsbAutoSwitch = 0;
            }
            
            pConfigInf->nPlUsbEnumEnabled = value;

            fd = open("/dev/oppo_custom", O_RDWR);
            if(fd < 0)
            {
                LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
                return -1;
            }

            nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
            if (nLen < 0)
            {
                LOGE("[%s] ERROR: Write OPPO_CUSTOM partition failed!\r\n", MOD);
                goto error_handle;
            }
            else
            {
                nRtnVal = 0;
            }

            close(fd);
        }
        else
        {
            nRtnVal = pConfigInf->nPlUsbEnumEnabled;
        }


        return nRtnVal;

    error_handle:
        close(fd);

        return -1;
    }

    int OPPOUtils:: oppoutils_custPartConfigItemClrFlagOps(const int opflag, const int source)
    {
        int nRtnVal = -1;
        int fd = -1;
        int nLen = -1;
        static int nTrigVoter = D_OPPO_CUST_PART_CLR_FLAG_SOURCE_MAX - 1;
        TOppoCustConfigInf  *pConfigInf = NULL;

        LOGD("[%s] The request opflag = %d, source = %d\r\n", MOD, opflag, source);  

        if(source >= D_OPPO_CUST_PART_CLR_FLAG_SOURCE_MAX)
        {
            LOGE("[%s] ERROR: The source to operate clear flag is too big\r\n", MOD);
    		return -1;
        }

        if((0 == opflag) && (0 < source))
        {
            LOGD("[%s] Record the triggered voter\r\n", MOD);

            nTrigVoter = source;
            
            return 0;
        }

        if(NULL == pCustPartBuf)
        {
            if (0 != CustPartBufInit())
            {
                return -1;
            }
        }
        
        fd = open("/dev/oppo_custom", O_RDONLY);
    	if(fd < 0)
    	{
    	    LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
    		return -1;
    	}

    	nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
    	if (nLen < 0)
    	{
    		LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
    		goto error_handle;
    	}

    	close(fd);

        pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

        if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1) && (0 == opflag))
    	    {
    	        CustPartConfigInfInit(pConfigInf);
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return -1;
    	    }
    	}

    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	    return -1;
    	}   
        
        if(0 == opflag)
        {
            unsigned char nCnt;
            int nOffset;

            nOffset = nTrigVoter;
            nCnt = ((pConfigInf->nClrFlag) >> (nOffset*8)) & 0xFF;
            nCnt += 1;
            pConfigInf->nClrFlag = ((pConfigInf->nClrFlag) & ~(0xFF << (nOffset*8))) | (nCnt << (nOffset*8));

            nOffset = source;
            nCnt = ((pConfigInf->nClrFlag) >> (nOffset*8)) & 0xFF;
            nCnt += 1;
            pConfigInf->nClrFlag = ((pConfigInf->nClrFlag) & ~(0xFF << (nOffset*8))) | (nCnt << (nOffset*8));

            fd = open("/dev/oppo_custom", O_RDWR);
            if(fd < 0)
            {
                LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
                return -1;
            }

            nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
            if (nLen < 0)
            {
                LOGE("[%s] ERROR: Write OPPO_CUSTOM partition failed!\r\n", MOD);
                goto error_handle;
            }
            else
            {
                nRtnVal = 0;
            }

            nTrigVoter = D_OPPO_CUST_PART_CLR_FLAG_SOURCE_MAX - 1;

            close(fd);
        }
        else
        {
            nRtnVal = ((pConfigInf->nClrFlag) >> (source*8)) & 0xFF;
        }


        return nRtnVal;

    error_handle:
        close(fd);

        return -1;
    }

    const char* OPPOUtils:: oppoutils_custPartGetMcpId()
    {
        int nRtnVal = -1;
        int fd = -1;
        int nLen = -1;
        int i = 0,j = 0;
        TOppoCustConfigInf  *pConfigInf = NULL;
        unsigned char Mcp_Id[][9] = {
        {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},//NONE
        {0x15,0x01,0x00,0x53,0x4A,0x53,0x30,0x30,0x4D},//KMSJS000KM_B308
        {0x90,0x01,0x4A,0x20,0x58,0x49,0x4E,0x59,0x48},//H9DP32A4JJACGR_KEM
        {0x15,0x01,0x00,0x4B,0x4A,0x53,0x30,0x30,0x4D},//KMKJS000VM_B309
        {0x15,0x01,0x00,0x4E,0x4A,0x53,0x30,0x30,0x4D},//KMNJS000ZM_B205
        {0x90,0x01,0x4A,0x20,0x58,0x49,0x4E,0x59,0x48},//H9TP32A4GDMCPR_KDM
			{0x15,0x01,0x00,0x4B,0x54,0x53,0x30,0x30,0x4D},//KMKJS000VM_B604
			{0x15,0x01,0x00,0x4B,0x33,0x55,0x30,0x30,0x4D},//KMK3U000VM_B410
        };
        if(NULL == pCustPartBuf)
        {
            if (0 != CustPartBufInit())
            {
                return "None";
            }
        }
        
        fd = open("/dev/oppo_custom", O_RDONLY);
    	if(fd < 0)
    	{
    	    LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
    		return "None";
    	}

    	nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
    	if (nLen < 0)
    	{
    		LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
    		return "None";
    	}

    	close(fd);

        pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

        if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1))
    	    {
    	        CustPartConfigInfInit(pConfigInf);
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return "None";
    	    }
    	}

    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	    return "None";
    	}   
        
       for(i = 0; i < sizeof(Mcp_Id)/9;i++)
       {
	         
	         LOGD("BpOPPOUtils :: mcpidlength= %d\n",sizeof(Mcp_Id)/9);
			   for(j = 0; j < 9;j++)
			{
					
					

				if(Mcp_Id[i][j] != pConfigInf->sMcpId[j])
				 break;
		    }
		 if(j == 9)
		    break;      
       }
       switch(i)
       {
            case MCP_NONE:
                return "mcp :None\n";
            case MCP_KMSJS000KM_B308:
                return "mcp :EMMC_KMSJS000KM_B308\n";
                break;
            case MCP_H9DP32A4JJACGR_KEM:
                return "mcp :EMMC_H9DP32A4JJACGR_KEM\n";
                break;
            case MCP_KMKJS000VM_B309:
                return "mcp :EMMC_KMKJS000VM_B309\n";
                break;
            case MCP_KMNJS000ZM_B205:
                return "mcp :EMMC_KMNJS000ZM_B205\n";
                break;
            case MCP_H9TP32A4GDMCPR_KDM:
                return "mcp :EMMC_H9TP32A4GDMCPR_KDM\n";
                break;
            case MCP_KMKJS000VM_B604:
                return "mcp :EMMC_KMKJS000VM_B604\n";
                break;
            case MCP_KMK3U000VM_B410:
                return "mcp :EMMC_KMK3U000VM_B410\n";
                break;
            default:
                return "mcp :unknown\n";
                break;
       }
        return "mcp :None\n";
    }
	
	int OPPOUtils:: oppoutils_custPartCustPartCalibrateGsensorData(int * buf)
	{
		int fd = -1;
        int nLen = -1;
        int i = 0;
		int buf_tmp[3] = {0,0,0};
        TOppoCustConfigInf  *pConfigInf = NULL;

		/***************Get calibrate data here and set date in buf_tmp*********************/

		fd = open("/dev/gsensor", O_RDWR);
		if(fd < 0)
		{
			close(fd);	
			LOGE("[%s] ERROR: open Gsensor file fail \n", MOD);
			return -1;
		}
		
		if(ioctl(fd,GSENSOR_IOCTL_USER_CALI,&buf_tmp) < 0)
		{
			close(fd);	
			LOGE("[%s] ERROR: IOCTL Gsensor file fail or get gsensor data fail \n", MOD);
			return -1;
		}
		
		close(fd);
		LOGE("Calibrate Gsensor x=%d y=%d z=%d \n", buf_tmp[0],buf_tmp[1],buf_tmp[2]);
		/************************check Magicnumber here**********************************/
		if(NULL == pCustPartBuf)
    {
        if (0 != CustPartBufInit())
        {
            return -1;
        }
    }
		
		fd = open("/dev/oppo_custom", O_RDONLY);
		
    	if(fd < 0)
    	{
    		close(fd);
    	    LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
    		return -1;
    	}

    	nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
    	if (nLen < 0)
    	{
    		close(fd);
    		LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
    		return -1;
    	}

    	close(fd);

        pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

        if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1))
    	    {
    	        CustPartConfigInfInit(pConfigInf);

				fd = open("/dev/oppo_custom", O_RDWR);
				if(fd < 0)
				{
					close(fd);
					LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
				close(fd);
				if(nLen < 0)
				{
					LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return -1;
    	    }
    	}
		
    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	    return -1;
    	} 
		
		/************************check Magicnumber end**********************************/

		/*********************backup calibrate date to custom partion*************************/

		fd = open("/dev/oppo_custom", O_RDWR);
		if(fd < 0)
		{
			close(fd);
			LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		pConfigInf->GsensorData[0] = buf_tmp[0];
		pConfigInf->GsensorData[1] = buf_tmp[1];
		pConfigInf->GsensorData[2] = buf_tmp[2];
		pConfigInf->nGsensorCalibrateFlag = 1;
		nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
		close(fd);
		if(nLen < 0)
		{
			LOGE("[%s] ERROR: write calibrate data to OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		buf[0] = buf_tmp[0];
		buf[1] = buf_tmp[1];
		buf[2] = buf_tmp[2];
		return 0;
	}

	int OPPOUtils::oppoutils_custPartCustPartRecoverGsensorData()
	{
		int fd = -1;
        int nLen = -1;
        int i = 0;
		int buf_tmp[3] = {0,0,0};
        TOppoCustConfigInf  *pConfigInf = NULL;

		/************************check Magicnumber here**********************************/
		#ifdef VENDOR_EDIT//mingqiang.guo@Prd.BasicDrv.Sensor, add 2012/8/3 for 
						  //read OPPO_CUSTOM partition device file fail
			if(NULL == pCustPartBuf)
	   		{
		        if (0 != CustPartBufInit())
		        {
		            return -1;
		        }
	    	}
		#endif /* VENDOR_EDIT*/
				
		fd = open("/dev/oppo_custom", O_RDONLY);
		
    	if(fd < 0)
    	{
    	    LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
    		return -1;
    	}

    	nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
    	if (nLen < 0)
    	{
    		LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
    		return -1;
    	}

    	close(fd);

        pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

       if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1))
    	    {
    	        CustPartConfigInfInit(pConfigInf);

				fd = open("/dev/oppo_custom", O_RDWR);
				if(fd < 0)
				{
					close(fd);
					LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
				close(fd);
				if(nLen < 0)
				{
					LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return -1;
    	    }
    	}
		
    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	   	return -1;
    	} 
		/************************check Magicnumber end**********************************/

		/************************check calibrate flag**************************************/
		if(pConfigInf->nGsensorCalibrateFlag == 1)
		{
			buf_tmp[0] =  pConfigInf->GsensorData[0];
			buf_tmp[1] =  pConfigInf->GsensorData[1];
			buf_tmp[2] =  pConfigInf->GsensorData[2];

			/**************set calibrate data to Gsensor here*******/

			
			fd = open("/dev/gsensor", O_RDWR);
			if(fd < 0)
			{
				close(fd);	
				LOGE("[%s] ERROR: open Gsensor file fail \n", MOD);
				return -1;
			}
			
			if(ioctl(fd,GSENSOR_IOCTL_SET_CALI,&buf_tmp) < 0)
			{
				close(fd);	
				LOGE("[%s] ERROR: IOCTL Gsensor file fail or set gsensor data fail \n", MOD);
				return -1;
			}

			close(fd);
			
			/**************set calibrate data to Gsensor end*******/
			
			LOGE("[%s] OPPO_CUSTOM partition Gsensor data %d %d %d\n", MOD,buf_tmp[0],buf_tmp[1],buf_tmp[2]);
		}else
		{
			LOGE("[%s] OPPO_CUSTOM partition Gsensor not Calibrated!\n", MOD);
			return -1;
		}
		return 0;
	}
	int OPPOUtils::oppoutils_custPartCustPartGetRebootNumber()
	{
	
		int fd = -1;
		int nLen = -1;
		int i = 0;
		int buf_tmp[3] = {0,0,0};
		TOppoCustConfigInf	*pConfigInf = NULL;
	
		if(NULL == pCustPartBuf)
		{
			if (0 != CustPartBufInit())
			{
				return -1;
			}
		}
			
		fd = open("/dev/oppo_custom", O_RDONLY);
		
		if(fd < 0)
		{
			LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		
		nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
		if (nLen < 0)
		{
			LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
			return -1;
		}
		
		close(fd);

		pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

       	if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1))
    	    {
    	        CustPartConfigInfInit(pConfigInf);

				fd = open("/dev/oppo_custom", O_RDWR);
				if(fd < 0)
				{
					close(fd);
					LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
				close(fd);
				if(nLen < 0)
				{
					LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return -1;
    	    }
    	}
		
    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	   	return -1;
    	} 

		return (int)pConfigInf->nRebootNumber;

	}

	int OPPOUtils::oppoutils_custPartCustPartGetRebootReason()
	{

				int fd = -1;
		int nLen = -1;
		int i = 0;
		int buf_tmp[3] = {0,0,0};
		TOppoCustConfigInf	*pConfigInf = NULL;
	
		if(NULL == pCustPartBuf)
		{
			if (0 != CustPartBufInit())
			{
				return -1;
			}
		}
			
		fd = open("/dev/oppo_custom", O_RDONLY);
		
		if(fd < 0)
		{
			LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		
		nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
		if (nLen < 0)
		{
			LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
			return -1;
		}
		
		close(fd);

		pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

       	if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1))
    	    {
    	        CustPartConfigInfInit(pConfigInf);

				fd = open("/dev/oppo_custom", O_RDWR);
				if(fd < 0)
				{
					close(fd);
					LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
				close(fd);
				if(nLen < 0)
				{
					LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return -1;
    	    }
    	}
		
    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	   	return -1;
    	} 
		/************************check Magicnumber end**********************************/

		return (int)pConfigInf->nRebootReason;
		
	}

	int OPPOUtils::oppoutils_custPartCustPartClearRebootNumber()
	{

		int fd = -1;
		int nLen = -1;
		int i = 0;
		int buf_tmp[3] = {0,0,0};
		TOppoCustConfigInf	*pConfigInf = NULL;
	
		/************************check Magicnumber here**********************************/
		if(NULL == pCustPartBuf)
		{
			if (0 != CustPartBufInit())
			{
				return -1;
			}
		}
			
		fd = open("/dev/oppo_custom", O_RDONLY);
		
		if(fd < 0)
		{
			LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		
		nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
		if (nLen < 0)
		{
			LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
			return -1;
		}
		
		close(fd);

		pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

       	if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1))
    	    {
    	        CustPartConfigInfInit(pConfigInf);

				fd = open("/dev/oppo_custom", O_RDWR);
				if(fd < 0)
				{
					close(fd);
					LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
				close(fd);
				if(nLen < 0)
				{
					LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return -1;
    	    }
    	}
		
    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	   	return -1;
    	} 
		/************************check Magicnumber end**********************************/
		pConfigInf->nRebootNumber = 0;
		fd = open("/dev/oppo_custom", O_RDWR);
		if(fd < 0)
		{
			close(fd);
			LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
		close(fd);
		if(nLen < 0)
		{
			LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		close(fd);	
		
		return 0;
		
	}
//#ifndef VENDOR_EDIT//mingqiang.guo@Prd.BasicDrv.Sensor, add 2012/11/15 add for ps revoer send pulse count 
       	int OPPOUtils::oppoutils_custPartCustPartAlsPsCalibratePsSendPulse (int *buf)
	{

		int fd = -1;
		int nLen = -1;
		int i = 0;
		int buf_tmp[3] = {0,0,0}; //0£ºoffset_value  1:ps_send_pulse 2:ps now value
		TOppoCustConfigInf	*pConfigInf = NULL;

                char strBuf[D_OPPO_CUST_PART_BUF_SIZE];
                
                //msensor calibrate ps send pulse 
                fd = open("/proc/sensor_debug/cali_pulse_count",O_RDONLY);
                if(fd < 0)
                {
                      LOGE("[%s] ERROR: open /proc/sensor_debug/cali_pulse_count\r\n", MOD);
                      close(fd);	
                      return -1;
                } 

                nLen = read(fd, strBuf, D_OPPO_CUST_PART_BUF_SIZE);
                if (nLen < 0)
                {
                      LOGE("[%s] ERROR: read /proc/sensor_debug/cali_pulse_count failed!\r\n", MOD);
                      close(fd);	
                      return -1;
                }
                LOGD("%s\n",strBuf);
                sscanf(strBuf,"%d:%d:%d",&buf_tmp[0],&buf_tmp[1],&buf_tmp[2]);
                LOGD("cali:  alsps offset_value = %d, ps_send_pulse=%d,ps_value=%d\n",buf_tmp[0],buf_tmp[1],buf_tmp[2]);
                //msensor calibrate end

		/************************check Magicnumber here**********************************/
		if(NULL == pCustPartBuf)
		{
			if (0 != CustPartBufInit()) { return -1;
			}
		}
			
		fd = open("/dev/oppo_custom", O_RDONLY);
		
		if(fd < 0)
		{
			LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		
		nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
		if (nLen < 0)
		{
			LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
			return -1;
		}
		
		close(fd);

		pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

       	if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1))
    	    {
    	        CustPartConfigInfInit(pConfigInf);

				fd = open("/dev/oppo_custom", O_RDWR);
				if(fd < 0)
				{
					close(fd);
					LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
				close(fd);
				if(nLen < 0)
				{
					LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return -1;
    	    }
    	}
		
    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	   	return -1;
    	} 
		/************************check Magicnumber end**********************************/
                pConfigInf->nAlsPsCalibratePsFlag = 1; 
                pConfigInf->nAlsPsSendPulseNumber = buf_tmp[0];
                pConfigInf->nAlsPsOffsetValue = buf_tmp[1];
		fd = open("/dev/oppo_custom", O_RDWR);
		if(fd < 0)
		{
			close(fd);
			LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
		close(fd);
		if(nLen < 0)
		{
			LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
			close(fd);
			return -1;
		}
		close(fd);	
                buf[0] = buf_tmp[0];
                buf[1] = buf_tmp[1];
                buf[2] = buf_tmp[2];

                return 0;
		
	}

      	int OPPOUtils::oppoutils_custPartCustPartAlsPsRecoverPsSendPulse()
	{

		int fd = -1;
		int nLen = -1;
		int i = 0;
		int buf_tmp[3] = {0,0,0};
		TOppoCustConfigInf	*pConfigInf = NULL;
                char strBuf[D_OPPO_CUST_PART_BUF_SIZE];

		/************************check Magicnumber here**********************************/
		if(NULL == pCustPartBuf)
		{
			if (0 != CustPartBufInit())
			{
				return -1;
			}
		}
			
		fd = open("/dev/oppo_custom", O_RDONLY);
		
		if(fd < 0)
		{
			LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		
		nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
		if (nLen < 0)
		{
			LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
			return -1;
		}
		
		close(fd);

		pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

       	if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1))
    	    {
    	        CustPartConfigInfInit(pConfigInf);

				fd = open("/dev/oppo_custom", O_RDWR);
				if(fd < 0)
				{
					close(fd);
					LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
				close(fd);
				if(nLen < 0)
				{
					LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return -1;
    	    }
    	}
		
    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	   	return -1;
        } 
        /************************check Magicnumber end**********************************/
        
        //msensor calibrate ps send pulse 
        if(pConfigInf->nAlsPsCalibratePsFlag)
        {
              fd = open("/proc/sensor_debug/cali_pulse_count",O_RDWR);
              if(fd < 0)
              {
                    LOGE("[%s] ERROR: open /proc/sensor_debug/cali_pulse_count\r\n", MOD);
                    close(fd);	
                    return -1;
               } 
              sprintf(strBuf,"8e:%x:",pConfigInf->nAlsPsSendPulseNumber);
                LOGD("recover: alsps offset_value = %d, ps_send_pulse=%d,ps_value=%d\n",buf_tmp[0],buf_tmp[1],buf_tmp[2]);
              nLen = write(fd, strBuf,strlen(strBuf)); 
              if (nLen < 0)
              {
                    LOGE("[%s] ERROR: read /proc/sensor_debug/cali_pulse_count failed!\r\n", MOD);
                    close(fd);	
                    return -1;
              }

              sprintf(strBuf,"9e:%x:",pConfigInf->nAlsPsOffsetValue);
              nLen = write(fd, strBuf,strlen(strBuf)); 
              if (nLen < 0)
              {
                    LOGE("[%s] ERROR: read /proc/sensor_debug/cali_pulse_count failed!\r\n", MOD);
                    close(fd);	
                    return -1;
              }
              close(fd);	
              //msensor calibrate end
        }
        return 0;

	}
//#endif /*VENDOR_EDIT*/

//#ifdef VENDOR_EDIT//LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05 add for ps switch 
       	int OPPOUtils::oppoutils_custPartCustPartAlsPsSetPsSwitch ()
	{

		int fd = -1;
		int nLen = -1;
              int status_tmp = 0;
		int i = 0;
		int buf_tmp[3] = {0,0,0};
		TOppoCustConfigInf	*pConfigInf = NULL;

                char strBuf[D_OPPO_CUST_PART_BUF_SIZE];
                int m_alsps_switch_ps_val;

                fd = open("/proc/sensor_debug/ps_switch",O_RDWR);
                if(fd < 0)
                {
                      LOGE("[%s] ERROR: open /proc/sensor_debug/ps_switch\r\n", MOD);
                      close(fd);	
                      return -1;
                } 
#if 1
                if (g_ps_switch_off == 0)
                {
    	                m_alsps_switch_ps_val = 1;
                }
                else if(g_ps_switch_off == 1)
                {
    	                m_alsps_switch_ps_val = 0;
                }
              sprintf(strBuf,"%d",m_alsps_switch_ps_val);
              LOGD(" alsps ps_switch set ps switch = %s\n",strBuf);
              nLen = write(fd, strBuf,strlen(strBuf)); 
              if (nLen < 0)
              {
                    LOGE("[%s] ERROR: read /proc/sensor_debug/ps_switch failed!\r\n", MOD);
                    close(fd);	
                    return -1;
              }
              g_ps_switch_off = m_alsps_switch_ps_val;
#else
                nLen = read(fd, strBuf, D_OPPO_CUST_PART_BUF_SIZE);
                if (nLen < 0)
                {
                      LOGE("[%s] ERROR: read /proc/sensor_debug/ps_switch failed!\r\n", MOD);
                      close(fd);	
                      return -1;
                }
                LOGD("%s\n",strBuf);
                g_ps_switch_off = m_alsps_switch_ps_val;// record the val
                sscanf(strBuf,"%d",&m_alsps_switch_ps_val);
                buf_tmp[0] = m_alsps_switch_ps_val;
                LOGD("alsps ps_switch =  %d\n",m_alsps_switch_ps_val);
#endif

		/************************check Magicnumber here**********************************/
		if(NULL == pCustPartBuf)
		{
			if (0 != CustPartBufInit()) { return -1;
			}
		}
			
		fd = open("/dev/oppo_custom", O_RDONLY);
		
		if(fd < 0)
		{
			LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		
		nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
		if (nLen < 0)
		{
			LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
			return -1;
		}
		
		close(fd);

		pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

       	if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1))
    	    {
    	        CustPartConfigInfInit(pConfigInf);

				fd = open("/dev/oppo_custom", O_RDWR);
				if(fd < 0)
				{
					close(fd);
					LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
				close(fd);
				if(nLen < 0)
				{
					LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return -1;
    	    }
    	}
		
    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	   	return -1;
    	} 
		/************************check Magicnumber end**********************************/
                pConfigInf->nAlsPsSwitchPsStoreFlag = 1; 
	        pConfigInf->nAlsPsSwitchPsVal = g_ps_switch_off;
		fd = open("/dev/oppo_custom", O_RDWR);
		if(fd < 0)
		{
			close(fd);
			LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
		close(fd);
		if(nLen < 0)
		{
			LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
			close(fd);
			return -1;
		}
		close(fd);	
		
		return (int)pConfigInf->nAlsPsSwitchPsVal;
		
	}

      	int OPPOUtils::oppoutils_custPartCustPartAlsPsRecoverPsSwitch()
	{

		int fd = -1;
		int nLen = -1;
		int i = 0;
		int buf_tmp[3] = {0,0,0};
		TOppoCustConfigInf	*pConfigInf = NULL;
                char strBuf[D_OPPO_CUST_PART_BUF_SIZE];

		/************************check Magicnumber here**********************************/
		if(NULL == pCustPartBuf)
		{
			if (0 != CustPartBufInit())
			{
				return -1;
			}
		}
			
		fd = open("/dev/oppo_custom", O_RDONLY);
		
		if(fd < 0)
		{
			LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
			return -1;
		}
		
		nLen = read(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
		if (nLen < 0)
		{
			LOGE("[%s] ERROR: read from OPPO_CUSTOM partition failed!\r\n", MOD);
			return -1;
		}
		
		close(fd);

		pConfigInf = (TOppoCustConfigInf*)(pCustPartBuf + OPPO_CUST_PART_ITEM__CONFIG * D_OPPO_CUST_PART_BLK_SIZE);

       	if(D_OPPO_CUST_PART_MAGIC_NUM != pConfigInf->nMagicNum1)
        {
    	    if((0x0 == pConfigInf->nMagicNum1 || 0xFFFFFFFF == pConfigInf->nMagicNum1))
    	    {
    	        CustPartConfigInfInit(pConfigInf);

				fd = open("/dev/oppo_custom", O_RDWR);
				if(fd < 0)
				{
					close(fd);
					LOGE("[%s] ERROR: open OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				nLen = write(fd, pCustPartBuf, D_OPPO_CUST_PART_BUF_SIZE);
				close(fd);
				if(nLen < 0)
				{
					LOGE("[%s] ERROR: write OPPO_CUSTOM partition device file fail\r\n", MOD);
					return -1;
				}
				
    	    }
    	    else
    	    {   
    	        LOGE("[%s] OPPO_CUSTOM partition is illegal!\n", MOD);
    	        return -1;
    	    }
    	}
		
    	if(D_OPPO_CUST_PART_CONFIG_MAGIC_NUM != pConfigInf->nMagicNum2)
    	{
    	    LOGE("[%s] OPPO_CUSTOM partition with error config magic number!\n", MOD);
    	   	return -1;
        } 
        /************************check Magicnumber end**********************************/
        
        //
        if(pConfigInf->nAlsPsSwitchPsStoreFlag)
        {              
              fd = open("/proc/sensor_debug/ps_switch",O_RDWR);
              if(fd < 0)
              {
                    LOGE("[%s] ERROR: open /proc/sensor_debug/ps_switch\r\n", MOD);
                    close(fd);	
                    return -1;
               } 
              sprintf(strBuf,"%d:",pConfigInf->nAlsPsSwitchPsVal);
              LOGD(" alsps  set ps switch = %s\n",strBuf);
              nLen = write(fd, strBuf,strlen(strBuf)); 
              if (nLen < 0)
              {
                    LOGE("[%s] ERROR: read /proc/sensor_debug/ps_switch failed!\r\n", MOD);
                    close(fd);	
                    return -1;
              }
              g_ps_switch_off = pConfigInf->nAlsPsSwitchPsVal;// record the val
              close(fd);	
              
        }
        return 0;

	}
      	int OPPOUtils:: oppoutils_custPartCustPartAlsPsGetPsSwitchStatus()
      	{
      	      /* add other code to avoid judging incorrect */
      	      return g_ps_switch_off;
      	}
        
//#endif /*VENDOR_EDIT*/

	status_t OPPOUtils::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags){
		LOGI("OPPOUtils::onTransact");
		switch(code){
		case OPPOUTILS_CHMOD:{
			LOGI("OPPOUtils::onTransact case OPPOUTILS_CHMOD");
			String8 path_s8 = data.readString8();
			const char* path = path_s8.string();		
			int mod = data.readInt32();
			LOGD("OPPOUtils:: path = %s mod = %d", path, mod);
			reply->writeInt32(oppoutils_chmod(path, mod));
			return NO_ERROR;
		} 
		case OPPOUTILS_GETPID:{
            LOGI("OPPOUtils::onTransact case OPPOUTILS_GETPID");
            String8 name_s8 = data.readString8();
            const char* name = name_s8.string();
            LOGD("OPPOUtils:: processName = %s ", name);
            reply->writeInt32(oppoutils_getPidByName(name));
            return NO_ERROR;
		}
		//#ifdef VENDOR_EDIT
		//Pantengjiao@GraphicTech.cpu_boost, 2013/09/18, Add for Open multiple CPU
		case RUTILS_CPUBOOST: {
            oppoutils_cpuBoost(data.readInt32());
            return NO_ERROR;
		}
		//#endif /* VENDOR_EDIT */
		case OPPOUTILS_SET_SYSTEMPROPERTIES:{
		    LOGI("OPPOUtils::onTransact case OPPOUTILS_SET_SYSTEMPROPERTIES");
		    String8 key_s8 = data.readString8();
		    const char* key = key_s8.string();
		    String8 value_s8 = data.readString8();
		    const char* value = value_s8.string();
		    LOGD("OPPOUtils:: key = %s val = %s", key, value);
		    reply->writeInt32(oppoutils_setSystemProperties(key_s8, value_s8));
		    return NO_ERROR;
		}
		case OPPOUTILS_HIDE_PROCESS:{
		    LOGI("OPPOUtils::onTransact case OPPOUTILS_HIDE_PROCESS");
		    int pid = data.readInt32();
		    LOGD("OPPOUtils:: pid = %d ", pid);
		    reply->writeInt32(oppoutils_hideProcessByPid(pid));
		    return NO_ERROR;
		}
		case OPPOUTILS_UNHIDE_PROCESS:{
            LOGI("OPPOUtils::onTransact case OPPOUTILS_UNHIDE_PROCESS");
            int pid = data.readInt32();
		    LOGD("OPPOUtils:: pid = %d ", pid);
		    reply->writeInt32(oppoutils_unhideProcessBypid(pid));
            return NO_ERROR;
		}
		/*VENDOR_EDIT Jinquan.Lin@BasicDrv.BL, 2012/04/22, Modify for customizing OPPO_CUSTOM partition!*/
		case OPPOUTILS_SET_USBNODE:{
            LOGI("OPPOUtils::onTransact case OPPOUTILS_SET_USBNODE");
            int opflag = data.readInt32();
            int value = data.readInt32();
            int voter = data.readInt32();
            LOGD("OPPOUtils:: opflag = %d value = %d voter = 0x%x", opflag, value, voter);
            reply->writeInt32(oppoutils_custPartConfigItemUsbOps(opflag, value, voter));
            return NO_ERROR;
		}
		case OPPOUTILS_SET_CLRFLAG:{
		    LOGI("OPPOUtils::onTransact case OPPOUTILS_SET_CLRFLAG");
            int opflag = data.readInt32();
            int source = data.readInt32();
            LOGD("OPPOUtils:: opflag = %d source = %d", opflag, source);
            reply->writeInt32(oppoutils_custPartConfigItemClrFlagOps(opflag, source));
            return NO_ERROR;
		}
		case OPPOUTILS_GET_MCPID:{
            reply->writeCString(oppoutils_custPartGetMcpId());
            return NO_ERROR;
		}
		case OPPOUTILS_CALIBRATE_GSENSORDATA:{
			//reply->writeCString(oppoutils_custPartGetMcpId());
			int buf[3] = {0,0,0};
			int ret = -1;
			ret = oppoutils_custPartCustPartCalibrateGsensorData(buf);
			reply->writeInt32(buf[0]);
			reply->writeInt32(buf[1]);
			reply->writeInt32(buf[2]);
			reply->writeInt32(ret);
            return NO_ERROR;
		}
		case OPPOUTILS_RECOVER_GSENSORDATA:{
			reply->writeInt32(oppoutils_custPartCustPartRecoverGsensorData());
			return NO_ERROR;
		}
		case OPPOUTILS_GET_REBOOTNUMBER:{
			reply->writeInt32(oppoutils_custPartCustPartGetRebootNumber());
			return NO_ERROR;
		}
		case OPPOUTILS_GET_REBOOTREASON:{
			reply->writeInt32(oppoutils_custPartCustPartGetRebootReason());
			return NO_ERROR;
		}
		case OPPOUTILS_CLEAR_REBOOTNUMBER:{
			reply->writeInt32(oppoutils_custPartCustPartClearRebootNumber());
			return NO_ERROR;
		}
//#ifndef VENDOR_EDIT//mingqiang.guo@Prd.BasicDrv.Sensor, add 2012/11/15 add for ps revoer send pulse count 
                case OPPOUTILS_MSENSOE_CALIBRATE_PS_SEND_PULSE:{
                          int buf[3] = {0,0,0};
                          int ret = -1;
                          ret = oppoutils_custPartCustPartAlsPsCalibratePsSendPulse(buf);
                          reply->writeInt32(buf[0]);
                          reply->writeInt32(buf[1]);
                          reply->writeInt32(buf[2]);
                          reply->writeInt32(ret);
			return NO_ERROR;
		}
		case OPPOUTILS_MSENSOE_RECOVER_PS_SEND_PULSE:{
			reply->writeInt32(oppoutils_custPartCustPartAlsPsRecoverPsSendPulse());
			return NO_ERROR;
		}
//#endif /*VENDOR_EDIT*/
//#ifndef VENDOR_EDIT//LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05 add for ps switch 
                case OPPOUTILS_ALSPS_SET_PS_SWITCH:{
			reply->writeInt32(oppoutils_custPartCustPartAlsPsSetPsSwitch());
			return NO_ERROR;
		}
		case OPPOUTILS_ALSPS_RECOVER_PS_SWITCH:{
			reply->writeInt32(oppoutils_custPartCustPartAlsPsRecoverPsSwitch());
			return NO_ERROR;
		}
		case OPPOUTILS_ALSPS_GET_PS_STATUS:{
			reply->writeInt32(oppoutils_custPartCustPartAlsPsGetPsSwitchStatus());
			return NO_ERROR;
		}
//#endif /*VENDOR_EDIT*/
		default:
			LOGI("OPPOUtils::onTransact default");
			return BBinder::onTransact(code, data, reply, flags);

		}
	}
};


