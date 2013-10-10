/*
 * NativeActionPickUp.cpp
 *
 *  Created on: 2013-2-5
 *      Author: 80054323
 */

#include "NativeActionPickUp.h"
#include <math.h>
#include <android/log.h>
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"NativeActionPickUp",__VA_ARGS__)

NativeActionPickUp::NativeActionPickUp() {
	// TODO Auto-generated constructor stub
	LOGD("NativeActionPickUp construct function is called!");
}

NativeActionPickUp::~NativeActionPickUp() {
	// TODO Auto-generated destructor stub
}

void NativeActionPickUp::nativeInit(){
#if 0 // canjie.zheng modified for distinguishing ActionPickUp by gyroscope
	GRAVITY = 9.81;
	InitFlagLimit = 1.0;
	PickUpLimit = 3.0;
	DIS_NEAR = 0;
	DIS_FAR = 1;
	disData = DIS_FAR;
	mInitFlag = false;
	mUpFlag = false;
#else
	//canjie.zheng add for distinguishing ActionPickUp by gyroscope 2013-03-18
	mTimeStamp = 0;
	NS2S = 1.0f / 1000000000.0f;
	EPSILON = 0.3f;
	anglex = 0;
	angley = 0;
	MAXVALUE = 0.32f;
	Init = true;
	DEBUG = true;
	TIMECOUNTLIMIT = 75;
	timeCount = TIMECOUNTLIMIT;
#endif //end
}

#if 0 //canjie.zheng modified for distinguishing ActionPickUp by gyroscope 2013-03-18
void NativeActionPickUp::nativeReceiveAccData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp){
	/*
	x = X;
	y = Y;
	z = Z;

	total = x * x + y * y + z * z;
	totalMin = (GRAVITY - InitFlagLimit) * (GRAVITY - InitFlagLimit);
	totalMax = (GRAVITY + InitFlagLimit) * (GRAVITY + InitFlagLimit);
	pickUpMin = (GRAVITY - PickUpLimit) * (GRAVITY - PickUpLimit);
	pickUpMax = (GRAVITY + PickUpLimit) * (GRAVITY + PickUpLimit);

	if (((total > totalMin) && (total < totalMax)) && !mInitFlag) {
		mInitFlag = true;
	}

	if (mInitFlag && !mUpFlag) {
		if ((total < pickUpMin) || (total > pickUpMax)) {
			mUpFlag = true;

		}
	}

	if (mUpFlag) {
		if (((total > totalMin) && (total < totalMax))
				&& (DIS_FAR == disData)) {
					jclass actionTurn = env->FindClass("com/oppo/kinectlib/ActionPickUp");
					jmethodID id = env->GetMethodID(actionTurn,"returnResult","()V");
					env->CallVoidMethod(thiz,id);
			LOGD("Return a PickUp Action!!!!");
			mInitFlag = false;
			mUpFlag = false;
		}
	}
	*/

}
void NativeActionPickUp::nativeReceiveDisData(JNIEnv* env,jobject thiz,jfloat distance,jfloat accuracy,jfloat timestamp){
	/*
	disData = distance;
	if (DIS_NEAR == disData) {
		mInitFlag = false;
		mUpFlag = false;
	}
	*/
}
#else
//canjie.zheng add 
void NativeActionPickUp::nativeReceiveGyrData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp){

	x = X;
	y = Y;
	z = Z;
	
#ifndef MTK_USER_BUILD
	if (DEBUG){
		LOGD("x = %f y = %f z = %f anglex=%f angley=%f",x,y,z,anglex, angley);
		LOGD("abs(x) = %f abs(y) = %f abs(z) = %f ",fabs(x),fabs(y),fabs(z));
		
	}
#endif

	if (fabs(x)<0.20001f && fabs(y)<0.20001f && fabs(z)<0.20001f) {
		Init = true;
		anglex = 0;
		angley = 0;
		if (timeCount > 0)
		{
			timeCount--;
		}//static  count it 
		if (DEBUG) 
			LOGD("----------Init state----------");
		
	}
	if (mTimeStamp == 0) {
		mTimeStamp = timestamp;
	} else {
		float dt = timestamp - mTimeStamp;
		mTimeStamp = timestamp;
		dt = dt * NS2S;
		if (dt > 0.7f) {
			return;
		}
		if (fabs(y) > EPSILON) {
			angley += y * dt; 
		}
		if (fabs(x) > EPSILON) {
			anglex += x * dt;
		}
		if (Init && fabs(anglex) > MAXVALUE || fabs(angley) > MAXVALUE) {
		#ifndef MTK_USER_BUILD
			if (DEBUG)
				LOGD("pick up timecount = %d ", timeCount);
		#endif
			if (timeCount == 0) 
			{
			jclass actionTurn = env->FindClass("com/oppo/kinectlib/ActionPickUp");
			jmethodID id = env->GetMethodID(actionTurn,"returnResult","()V");
			env->CallVoidMethod(thiz,id);
		#ifndef MTK_USER_BUILD
			if (DEBUG)
			{
				LOGD("Return a PickUp Action!!!!angelx=%f angley=%f ",anglex, angley);
				
			}
		#endif
			Init = false;
			anglex = 0;
			angley = 0;
			}
			timeCount = TIMECOUNTLIMIT;
		}

	}

}
//canjie.zheng end
#endif
