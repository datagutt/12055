/*
 * NativeActionBringToEar.cpp
 *
 *  Created on: 2013-2-4
 *      Author: lemon
 */

#include "NativeActionBringToEar.h"
#include <math.h>
#include <pthread.h>
//#include <utils/threads.h>
#include <unistd.h>


#include <android/log.h>
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"NativeActionBringToEar",__VA_ARGS__)


static int SIZE=20;
static float data_x[20];
static float max_x=0;
static float min_x=0;
static float average_x = 0;
static float gap_x = 0;
static bool Isfirst=true;
static int mIndex=0;
static bool lastPhone_position = false;
static bool mBusy = false;
static bool mGesture_checked = false;


pthread_t thread;




NativeActionBringToEar::NativeActionBringToEar(){

	gravite[0] = 0;
	gravite[1] = 0;
	gravite[2] = 0;

	mTimeStamp = 0;
	NS2S = 1.0f / 1000000000.0f;
	EPSILON = 0.3f;
	anglez[SIZE] = 0;
	gyrIndex = 0;
	MAXVALUE = 0.33f;
	waitFor =false;
	waitForAngle = 0;
	sumAngle = 0;
	for (int i = 0; i < SIZE; ++i){
		anglez[i] = (float)0.0;
		motionZ[i] = (float)0.0; 
	}
}

NativeActionBringToEar::~NativeActionBringToEar() {
	
}

void NativeActionBringToEar:: nativeReceiveAccData(JNIEnv* env,
                                                  jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp) {
//add by canjie.zheng@platform.Srv 2013-03-26     
 	gravite[0] = gravite[0] * 0.9 + X *0.1;
	gravite[1] = gravite[1] * 0.9 + Y *0.1;
	gravite[2] = gravite[2] * 0.9 + Z *0.1;
//end
 	if(mGesture_checked){
	jclass ActionBringToEar = env->FindClass("com/oppo/kinectlib/ActionBringToEar");
	jmethodID id = env->GetMethodID(ActionBringToEar,"returnResult","()V");
	env->CallVoidMethod(thiz,id);
	LOGD("---lemon--call back is ok !!!!");
	mGesture_checked=false;		
	}
#if 0 //canjie.zheng add gyroscope
	data_x[mIndex++] = X;
	if(mIndex == SIZE){
		mIndex=0;
		Isfirst=false;
	}	
	if(fabs(Z)<7&& Y > 2.5 && fabs(X)>2 && fabs(X) < 8.5)
#else
	motionZ[mIndex] = Z - gravite[2];
	mIndex++;
	if(mIndex == SIZE){
		mIndex=0;
		Isfirst=false;
	}	
	if ((gravite[2] > -4.5 && gravite[1] > -1.5) 
		&& ((sumAngle < 0 && gravite[0] < -1.0) 
		|| (sumAngle > 0 && gravite[0] > 1.0 )))
#endif
	{
		lastPhone_position=true;

	}
	else
	{
		lastPhone_position=false;
	
	}
}

 void *NativeActionBringToEar::ThreadWrapper(void *me) 
 {
	 return (void *) static_cast<NativeActionBringToEar *>(me)->checkPhoneposition();
 }

 int  NativeActionBringToEar::checkPhoneposition() {

	 LOGD( "--lemon-0--the checkPhoneposition--sleep 150ms---");

	 usleep(150000);

	 LOGD( "--lemon-1--the checkPhoneposition--sleep 150ms---");
	if(lastPhone_position && waitForAngle < 0.12)
	{
		mGesture_checked=true;
		LOGD( "--lemon--checkPhoneposition-the lastPhone_position is true ---good job ! waitForAngle = %f" , waitForAngle);
		waitForAngle = 0;
		waitFor = false;
	}
	else
	{
		waitForAngle = 0;
		waitFor = false;
		LOGD( "--lemon---the lastPhone_position is false -- waitForAngle = %f", waitForAngle);
	}

	 return 0;
		
}


int NativeActionBringToEar:: nativeReceiveDisData(JNIEnv* env,
                                                  jobject thiz,jfloat distan,jfloat accuracy,jfloat timestamp) {
       if ( distan < 1 && !Isfirst ) {
	   	
	LOGD("--lemon---DIS the dlastP_position is true ---0");

#if 0 //add by canjie.zheng for gyroscope
	max_x=data_x[0];
	min_x=data_x[SIZE-1];
	for (int i = 0; i < SIZE; i++) {	
	if (data_x[i] > max_x) {
		max_x = data_x[i];
	}
	if (data_x[i] < min_x) {
		min_x = data_x[i];
	}
	average_x += fabs(data_x[i]) / SIZE;
	}
	gap_x = max_x - min_x;
	LOGD("---max=%f--------min=%f" ,max_x, min_x);
	LOGD("---gap_x=%f------average=%f",gap_x,average_x);
	 if(7.00001 < gap_x && gap_x < 32.5556 && average_x < 10.3333 && average_x > 0.222222)
#else
	sumAngle = 0;
	for (int i = 0; i < SIZE; ++i) {
		sumAngle += anglez[i];
	}
	LOGD("sumAngle = %f" ,sumAngle);
	if (fabs(sumAngle) > MAXVALUE && CheckTrackIsRight())
#endif
             {
				/* if (!mBusy) {
						mBusy = true;
						new Thread() {
							public void run() {
								checkPhoneposition();
								mBusy = false;
							}
						}.start();
					}*///this is java method
			waitFor = true;
			waitForAngle = 0;
			LOGD("--lemon---pthread_create -----");
			pthread_attr_t attr; 
			pthread_attr_init (&attr); 
			pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED); 
			pthread_create(&thread, &attr, ThreadWrapper,this);
			pthread_attr_destroy (&attr);
			//pthread_create(&thread, NULL, checkPhoneposition,NULL);
	
			//pthread_join(thread,NULL);
			//pthread_exit(NULL);
			LOGD("--lemon--pthread_join-1-----");
             }	
			 average_x = 0;
		} else {
			mGesture_checked = false;
			LOGD("--lemon---DIS the lastP_position is false ---1");
		}
	return 0;
	
}

void NativeActionBringToEar:: nativeReceiveGyrData(JNIEnv* env,
                                                  jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp) {
	if (mTimeStamp == 0) {
		mTimeStamp = timestamp;
	} else {
		float dt = timestamp - mTimeStamp;
		mTimeStamp = timestamp;
		dt = dt * NS2S;

		if (dt > 0.5 || fabs(Z) > 9) {
			LOGD("error Gyr data");	
		} else {
			
			anglez[gyrIndex++] = Z * dt;
			if (waitFor) {
				waitForAngle += fabs(Z * dt);
			}
		}
		if (gyrIndex == SIZE) {
			gyrIndex = 0;
			Isfirst=false;
		}
	
	}
}

bool NativeActionBringToEar::CheckTrackIsRight() {
		int countZ = 0;
		for (int i = 0; i < SIZE - 1; ++i) {
			if (fabs(motionZ[i] - motionZ[i + 1]) > 1){
				++countZ;
				
			}
			if (countZ > 10) {
				LOGD("count many countZ = %d", countZ);
				return false;
			}
			return true;
		}
		return false;
}
	

//end
