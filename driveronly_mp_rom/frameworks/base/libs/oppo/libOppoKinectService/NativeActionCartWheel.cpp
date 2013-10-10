/*
 * NativeActionCartWheel.cpp
 *
 *  Created on: 2013-1-28
 *      Author: 80049072
 */

#include "NativeActionCartWheel.h"
#include <math.h>

#include <android/log.h>
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"NativeActionCartWheel",__VA_ARGS__)

NativeActionCartWheel::NativeActionCartWheel():
MININITX((float) -3.355),MAXINITX((float) 3.355),MINENDX((float) 9.218),
NS2S(1.0f / 1000000000.0f),EPSILON((float) 1),sumAngleZ((float) (3.14 / 3)){
	DEBUG = true;
}

NativeActionCartWheel::~NativeActionCartWheel() {
	// TODO Auto-generated destructor stub
	
}
void NativeActionCartWheel::init() {

//    MININITX = (float) -3.355;
//    MAXINITX = (float) 3.355; /*cos(70)*9.81*/
//   MINENDX = (float) 9.218; /*cos(70)*/
//	NS2S = 1.0f / 1000000000.0f;
//	EPSILON = (float) 1;
//	sumAngleZ = (float) (3.14 / 3);
	for (int i = 0; i < 3; ++i){
	    gravity[i] = 0;
		motion[i] = 0;
	}
	mTimestamp = 0;
	angleZ = 0;
	direction = 0;
	IsWheel = false;
	
	mTimestamp = 0;
	angleZ = 0;
	direction = 0;
	IsWheel = false;
 	LOGD("NativeActionCartWheel:: Init");
}
void NativeActionCartWheel:: nativeReceiveAccData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp) {


	
	float values[3] = {X, Y, Z};

	for (int i = 0; i < 3; ++i) {
		gravity[i] = (float) (0.2 * values[i] + 0.8 * gravity[i]);
		motion[i] = values[i] - gravity[i];
	}


}

void NativeActionCartWheel:: nativeReceiveGyrData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp) {

		
	if (mTimestamp == 0) {
        mTimestamp = timestamp;
    } else {
     	float dt = (timestamp - mTimestamp) * NS2S;
		

       	mTimestamp = timestamp;
		if (fabs(Z) > EPSILON) {
			IsWheel = true;
			if (direction == 0) {
				direction = Z > 0 ? 1 : -1;
			}
			if (direction * Z > 0) {
				angleZ += Z * dt;
			} else { //direction has been changed
				angleZ = 0;
				direction *= -1;
				angleZ += Z * dt;
			}
		
		} else {
		
		#ifndef MTK_USER_BUILD
			if (DEBUG){
				LOGD("gry angleZ=%f",angleZ);
			}
		#endif

			if (IsWheel && fabs(angleZ) > sumAngleZ  && (gravity[0] > MINENDX || gravity[0] < -MINENDX) ) {

				jclass cartWheel = env->FindClass("com/oppo/kinectlib/ActionCartWheel");
				jmethodID id = env->GetMethodID(cartWheel,"returnResult","()V");
				env->CallVoidMethod(thiz,id);
				angleZ = 0;
				IsWheel = false;
			}


		}
    }
}
