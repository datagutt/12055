/******************************************************************************
 * Copyright 2008-2010 OPPO Mobile Comm Corp., Ltd, All rights reserved.
 * FileName:android_oppoutils_OppoUtils.cpp
 * ModuleName:
 * Author:longzh@oppo.com
 * Creat Data: 2011-02-13
 * Description:
		jni interface of tp for rezero
 * History
 * version       time         author     description
	1.0		  2011-02-13  longzh@oppo.com   create
 ******************************************************************************/
#define LOG_TAG "jniOPPOUtils"
#define RES_SUCESS 0
#define RES_ERROR -1
#include <jni.h>
#include <nativehelper/JNIHelp.h>
#include <binder/Binder.h>
#include <binder/Parcel.h>
#include <binder/IInterface.h>
#include <binder/IServiceManager.h>
#include <oppoutils/oppoutils.h> 
#include <utils/Log.h>
#define MOD "OPPO_CUSTOM_PARTITION"
#define OPPOUTILS_SERVICENAME "oppo.com.IOPPOUtils"
namespace android{
    static sp<IOPPOUtils> getOPPOUtilsService(const char* serviceName){
        sp<IServiceManager>sm = defaultServiceManager();
        if(sm == NULL){
            ALOGE("android_oppoutils_OppoUtils :: ServiceManager is ready ?" );
            return NULL;
        }
        sp<IBinder> binder = sm->getService(String16(serviceName));
        if(binder == NULL){
            ALOGE("android_oppoutils_OppoUtils :: %s is ready ?", serviceName );
            return NULL;
        }
        sp<IOPPOUtils> ir = interface_cast<IOPPOUtils>(binder);
        return ir;
    }
    static int jstring2char(JNIEnv * env, char** cstr, jstring jstr){
        jclass clsstring = env->FindClass("java/lang/String");
        jstring strencode = env->NewStringUTF("utf-8");
        jmethodID methodID = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
        jbyteArray bArr = (jbyteArray)env->CallObjectMethod(jstr, methodID, strencode);
        jsize bArrSize = env->GetArrayLength(bArr);        
        ALOGE("android_oppoutils_OppoUtils :: bArrSize = %d ", bArrSize);  
        jbyte* pbArr = env->GetByteArrayElements(bArr, JNI_FALSE);
        if(bArrSize > 0){
            *cstr = (char*)malloc(bArrSize + 1);
            memset(*cstr, '\0', bArrSize + 1);
            memcpy(*cstr, pbArr, bArrSize);
            ALOGE("android_oppoutils_OppoUtils :: cstr = %s ", *cstr);
        }
        env->ReleaseByteArrayElements(bArr, pbArr, 0);
        return RES_SUCESS;
    }
    static int android_oppoutils_OppoUtils_NativeGetPidByName(JNIEnv * env, jobject obj, jstring processName){
        char* cName = NULL;
        jstring2char(env, &cName, processName);        
        sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);
        if(ir == NULL){
            ALOGE("android_oppoutils_OppoUtils :: Something wrong in getting service %s .", OPPOUTILS_SERVICENAME );
            return RES_ERROR;
        }
        return ir->oppoutils_getPidByName(cName);
    }
	//#ifdef VENDOR_EDIT
	//Pantengjiao@GraphicTech.cpu_boot, 2013/09/18, Add for Open multiple CPU
	static void android_oppoutils_OppoUtils_NativeCpuBoost(JNIEnv * env, jobject obj, jboolean bStart) {
        sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);
        if(ir == NULL){
            ALOGE("android_oppoutils_OppoUtils :: Something wrong in getting service %s .", OPPOUTILS_SERVICENAME );
        }
        ir->oppoutils_cpuBoost(bStart);
	}
	//#endif /* VENDOR_EDIT */
    static int android_oppoutils_OppoUtils_NativeChmod(JNIEnv * env, jobject obj, jstring path, jint mode){
        char* cPath = NULL;
        jstring2char(env, &cPath, path);
        ALOGE("android_oppoutils_OppoUtils :: cPath = %s mode = %d ", cPath, mode);
        sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);
        if(ir == NULL){
            ALOGE("android_oppoutils_OppoUtils :: Something wrong in getting service %s .", OPPOUTILS_SERVICENAME );
            return RES_ERROR;
        }
        ir->oppoutils_chmod(cPath, mode);
        return RES_SUCESS;
    }
    static int android_oppoutils_OppoUtils_NativeSetSystemProperties(JNIEnv * env, jobject obj, jstring key, jstring val){
        char* cKey = NULL;
        jstring2char(env, &cKey, key);
        char* cVal = NULL;
        jstring2char(env, &cVal, val);
        ALOGE("android_oppoutils_OppoUtils :: cKey = %s cVal = %s ", cKey, cVal);
        sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);
        if(ir == NULL){
            ALOGE("android_oppoutils_OppoUtils :: Something wrong in getting service %s .", OPPOUTILS_SERVICENAME );
            return RES_ERROR;
        }
        ir->oppoutils_setSystemProperties(cKey, cVal);
        return RES_SUCESS;
        


    }
    static int android_oppoutils_OppoUtils_NativeHideProcessByPid(JNIEnv * env, jobject obj, jint pid){
        sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);
        if(ir == NULL){
            ALOGE("android_oppoutils_OppoUtils :: Something wrong in getting service %s .", OPPOUTILS_SERVICENAME );
            return RES_ERROR;
        }
        return ir->oppoutils_hideProcessByPid(pid);
         
    }
    static int android_oppoutils_OppoUtils_NativeUnHideProcessByPid(JNIEnv * env, jobject obj, jint pid){
        sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);
        if(ir == NULL){
            ALOGE("android_oppoutils_OppoUtils :: Something wrong in getting service %s .", OPPOUTILS_SERVICENAME );
            return RES_ERROR;
        }
        return ir->oppoutils_unhideProcessBypid(pid);
         

    }
#ifdef VENDOR_EDIT//Jinquan.Lin@BasicDrv.BL, 2012/05/17, Add for customizing OPPO_CUSTOM partition!
    static int android_oppoutils_OppoUtils_NativeCustPartConfigItemUsbOps(JNIEnv *env, jobject clazz, jboolean bRead, jint nValue, int nVoter){
        int nRtnVal = -1;

        sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

        if(ir == NULL)
        {
            ALOGE("[%s] android_oppoutils_OppoUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
            return -1;
        }

        nRtnVal = ir->oppoutils_custPartConfigItemUsbOps(bRead, nValue, nVoter);

        return nRtnVal;
    }

    static int android_oppoutils_OppoUtils_NativeCustPartConfigItemClrFlagOps(JNIEnv *env, jobject clazz, jboolean bRead, jint nSource)
    {
        int nRtnVal = -1;

        sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

        if(ir == NULL)
        {
            ALOGE("[%s] android_oppoutils_OppoUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
            return -1;
        }

        nRtnVal = ir->oppoutils_custPartConfigItemClrFlagOps(bRead, nSource);

        return nRtnVal;
    }

    static jstring android_oppoutils_OppoUtils_NativeCustPartGetMcpId(JNIEnv *env, jobject clazz)
    {
        //String8 nRtnVal = "";

        sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

        if(ir == NULL)
        {
            ALOGE("[%s] android_oppoutils_OppoUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
            return env->NewStringUTF("None");
        }

        return env->NewStringUTF(ir->oppoutils_custPartGetMcpId());

    }
#endif /* VENDOR_EDIT */

	//Yixue.Ge@ProDrv.BL add for set and get Gsensor data
	
	static jintArray android_oppoutils_OppoUtils_NativeCustPartCalibrateGsensorData(JNIEnv *env, jobject clazz)
	{
		int nRtnVal = -1;
		int data_get[3] = {0,0,0};
		jintArray data_return = env->NewIntArray(3);
		sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

		if(ir == NULL)
		{
			ALOGE("[%s] android_oppoutils_OppoUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
			return data_return;
		}
		nRtnVal = ir->oppoutils_custPartCustPartCalibrateGsensorData(data_get);
		env->SetIntArrayRegion(data_return,0,3,data_get);

		return data_return;
	}

	static int android_oppoutils_OppoUtils_NativeCustPartRecoverGsensorData(JNIEnv *env, jobject clazz)
	{
		int nRtnVal = -1;
		sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

		if(ir == NULL)
		{
			ALOGE("[%s] android_oppoutils_OppoUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
			return -1;
		}
		nRtnVal = ir->oppoutils_custPartCustPartRecoverGsensorData();

		return nRtnVal;
	}

	static int android_oppoutils_OppoUtils_NativeCustPartGetRebootNumber(JNIEnv *env, jobject clazz)
	{
		int nRtnVal = -1;
		sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

		if(ir == NULL)
		{
			ALOGE("[%s] android_oppoutils_OppoUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
			return -1;
		}
		nRtnVal = ir->oppoutils_custPartCustPartGetRebootNumber();

		return nRtnVal;
	}

	static int android_oppoutils_OppoUtils_NativeCustPartGetRebootReason(JNIEnv *env, jobject clazz)
	{
		int nRtnVal = -1;
		sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

		if(ir == NULL)
		{
			ALOGE("[%s] android_oppoutils_OppoUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
			return -1;
		}
		nRtnVal = ir->oppoutils_custPartCustPartGetRebootReason();

		return nRtnVal;
	}

	static int android_oppoutils_OppoUtils_NativeCustPartClearRebootNumber(JNIEnv *env, jobject clazz)
	{
		int nRtnVal = -1;
		sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

		if(ir == NULL)
		{
			ALOGE("[%s] android_oppoutils_OppoUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
			return -1;
		}
		nRtnVal = ir->oppoutils_custPartCustPartClearRebootNumber();

		return nRtnVal;
	}
//#ifndef VENDOR_EDIT//mingqiang.guo@Prd.BasicDrv.Sensor, add 2012/11/15 add for ps revoer send pulse count 
	static jintArray android_oppoutils_OppoUtils_NativeCustPartAlsPsCalibratePsSendPulse(JNIEnv *env, jobject clazz)
	{
		int nRtnVal = -1;
		int data_get[3] = {0,0,0};
		jintArray data_return = env->NewIntArray(3);
		sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

		if(ir == NULL)
		{
			ALOGE("[%s] android_oppoutils_OPPOUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
			return data_return;
		}
		nRtnVal = ir->oppoutils_custPartCustPartAlsPsCalibratePsSendPulse(data_get);
		env->SetIntArrayRegion(data_return,0,3,data_get);
		return data_return;
	}

	static int android_oppoutils_OppoUtils_NativeCustPartAlsPsRecoverPsSendPulse(JNIEnv *env, jobject clazz)
	{
		int nRtnVal = -1;
		sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

		if(ir == NULL)
		{
			ALOGE("[%s] android_oppoutils_OPPOUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
			return -1;
		}
		nRtnVal = ir->oppoutils_custPartCustPartAlsPsRecoverPsSendPulse();

		return nRtnVal;
	}
//#endif /*VENDOR_EDIT*/
//#ifdef VENDOR_EDIT//LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05  for ps switch 
	static int android_oppoutils_OppoUtils_NativeCustPartAlsPsSetPsSwitch(JNIEnv *env, jobject clazz)
	{
		int nRtnVal = -1;
		sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

		if(ir == NULL)
		{
			ALOGE("[%s] android_oppoutils_OPPOUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
			return -1;
		}
		nRtnVal = ir->oppoutils_custPartCustPartAlsPsSetPsSwitch();

		return nRtnVal;
	}

	static int android_oppoutils_OppoUtils_NativeCustPartAlsPsRecoverPsSwitch(JNIEnv *env, jobject clazz)
	{
		int nRtnVal = -1;
		sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

		if(ir == NULL)
		{
			ALOGE("[%s] android_oppoutils_OPPOUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
			return -1;
		}
		nRtnVal = ir->oppoutils_custPartCustPartAlsPsRecoverPsSwitch();

		return nRtnVal;
	}
 	static int android_oppoutils_OppoUtils_NativeCustPartAlsPsGetPsSwitchStatus(JNIEnv *env, jobject clazz)
	{
		int nRtnVal = -1;
		sp<IOPPOUtils> ir = getOPPOUtilsService(OPPOUTILS_SERVICENAME);

		if(ir == NULL)
		{
			ALOGE("[%s] android_oppoutils_OPPOUtils :: Something wrong in getting service %s .", MOD, OPPOUTILS_SERVICENAME );
			return -1;
		}
		nRtnVal = ir->oppoutils_custPartCustPartAlsPsGetPsSwitchStatus();

		return nRtnVal;
	}   
//#endif /*VENDOR_EDIT*/
    static JNINativeMethod method_table[] = {
        { "NativeChmod", "(Ljava/lang/String;I)I",(void *)android_oppoutils_OppoUtils_NativeChmod },
        { "NativeGetPidByName", "(Ljava/lang/String;)I", (void *)android_oppoutils_OppoUtils_NativeGetPidByName},   
        { "NativeSetSystemProperties", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)android_oppoutils_OppoUtils_NativeSetSystemProperties},
        { "NativeHideProcessByPid", "(I)I", (void *)android_oppoutils_OppoUtils_NativeHideProcessByPid},
        { "NativeUnHideProcessByPid", "(I)I", (void *)android_oppoutils_OppoUtils_NativeUnHideProcessByPid},
        { "NativeCustPartConfigItemUsbOps", "(ZII)I", (void*)android_oppoutils_OppoUtils_NativeCustPartConfigItemUsbOps},
        { "NativeCustPartConfigItemClrFlagOps", "(ZI)I", (void*)android_oppoutils_OppoUtils_NativeCustPartConfigItemClrFlagOps},
        { "NativeCustPartCustPartGetMcpId", "()Ljava/lang/String;", (void*)android_oppoutils_OppoUtils_NativeCustPartGetMcpId},
        { "NativeCustPartCustPartCalibrateGsensorData", "()[I", (void*)android_oppoutils_OppoUtils_NativeCustPartCalibrateGsensorData},
        { "NativeCustPartCustPartRecoverGsensorData", "()I", (void*)android_oppoutils_OppoUtils_NativeCustPartRecoverGsensorData},
        { "NativeCustPartCustPartGetRebootNumber", "()I", (void*)android_oppoutils_OppoUtils_NativeCustPartGetRebootNumber},
        { "NativeCustPartCustPartGetRebootReason", "()I", (void*)android_oppoutils_OppoUtils_NativeCustPartGetRebootReason}, 
        { "NativeCustPartCustPartClearRebootNumber", "()I", (void*)android_oppoutils_OppoUtils_NativeCustPartClearRebootNumber}, 
//#ifndef VENDOR_EDIT//mingqiang.guo@Prd.BasicDrv.Sensor, add 2012/11/15 add for ps revoer send pulse count 
        { "NativeCustPartCustPartAlsPsCalibratePsSendPulse", "()[I", (void*)android_oppoutils_OppoUtils_NativeCustPartAlsPsCalibratePsSendPulse}, 
        { "NativeCustPartCustPartAlsPsRecoverPsSendPulse", "()I", (void*)android_oppoutils_OppoUtils_NativeCustPartAlsPsRecoverPsSendPulse},    
//#endif /*VENDOR_EDIT*/
//#ifdef VENDOR_EDIT//LiuPing@Prd.BasicDrv.Sensor, add 2012/12/05  for ps switch 
        { "NativeCustPartCustPartAlsPsSetPsSwitch", "()I", (void*)android_oppoutils_OppoUtils_NativeCustPartAlsPsSetPsSwitch}, 
        { "NativeCustPartCustPartAlsPsRecoverPsSwitch", "()I", (void*)android_oppoutils_OppoUtils_NativeCustPartAlsPsRecoverPsSwitch},
         { "NativeCustPartCustPartAlsPsGetPsSwitchStatus", "()I", (void*)android_oppoutils_OppoUtils_NativeCustPartAlsPsGetPsSwitchStatus},           
//#endif /*VENDOR_EDIT*/
		//#ifdef VENDOR_EDIT
		//Pantengjiao@GraphicTech.cpu_boot, 2013/09/18, Add for Open multiple CPU
        { "NativeCpuBoost", "(Z)V", (void*)android_oppoutils_OppoUtils_NativeCpuBoost},
		//#endif /* VENDOR_EDIT */
    };
    int register_android_oppoutils_OppoUtils(JNIEnv *env)
    {
        return jniRegisterNativeMethods(env, "android/oppoutils/OppoUtils",
                method_table, NELEM(method_table));
    }

};

