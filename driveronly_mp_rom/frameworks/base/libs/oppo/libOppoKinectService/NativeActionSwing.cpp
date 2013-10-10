/*
 * NativeActionSwing.cpp
 *
 *  Created on: 2013-3-18
 *      Author: 80049072
 */

#include "NativeActionSwing.h"
#include <math.h>

#include <android/log.h>
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"NativeActionSwing",__VA_ARGS__)

NativeActionSwing::NativeActionSwing()
{

}

NativeActionSwing::~NativeActionSwing() 
{
	// TODO Auto-generated destructor stub
	
}

void NativeActionSwing::init() 
{
    refresh = 0;
	ACTION_SWING = 8;
	dynamics = (float)4;
	freq = 23;
	DEBUG = true;
	
#ifndef MTK_USER_BUILD
	if (DEBUG) 
	{
		LOGD("init actionswing data");
	}
#endif
}

void NativeActionSwing:: nativeReceiveGyrData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp) 
{
	mx = X;
	my = Y;
	mz = Z;
#ifndef MTK_USER_BUILD
	if (DEBUG) 
	{
		LOGD("refresh = %d Y = %f  Z = %f", refresh, my, mz);
	}
#endif

	if (refresh == 0 || refresh-- ==  0) {
	#ifndef MTK_USER_BUILD
		if(DEBUG)
		{
			LOGD("in catching y = %f z = %f", my, mz);
		}
	#endif
		int result[2] = {0,0};
		if (my < -dynamics || mz > dynamics) {
			result[0] = ACTION_SWING;
			result[1] = 1;
		}
		if ( my > dynamics || mz < -dynamics) {
			result[0] = ACTION_SWING;
			result[1] = -1;
		
		}
		if (result[0] == ACTION_SWING){

			jintArray jresult;
			jclass actionSwing;
			jmethodID id;
			jresult = env->NewIntArray(2);
			actionSwing = env->FindClass("com/oppo/kinectlib/ActionSwing");
			id = env->GetMethodID(actionSwing,"returnResult","([I)V");
			env->SetIntArrayRegion(jresult, 0, 2, result);
			env->CallVoidMethod(thiz,id,jresult);
			result[0] = 0;
			refresh = freq;
		}
	}
}

bool NativeActionSwing:: nativeSetparameter(float parameter[], int length) 
{
#ifndef MTK_USER_BUILD
	if (DEBUG) 
	{
		LOGD("setparameter parameter[0]=%f, parameter[1]=%f, length=%d",parameter[0], parameter[1], length);
	}
#endif
	if (length == 2 && parameter[0] > 0 && parameter[1] > 0)
	{
		dynamics = parameter[0];
		freq = (int)parameter[1];
		return true;
	}
	return false;
}

