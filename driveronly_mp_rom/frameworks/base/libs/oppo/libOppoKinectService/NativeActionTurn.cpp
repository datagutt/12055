/*
 * NativeActionTurn.cpp
 *
 *  Created on: 2013-1-28
 *      Author: 80054323
 */

#include "NativeActionTurn.h"
#include <math.h>

#include <android/log.h>
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"NativeActionTurn",__VA_ARGS__)

NativeActionTurn::NativeActionTurn(){

}

NativeActionTurn::~NativeActionTurn() {
	// TODO Auto-generated destructor stub
	
}
void NativeActionTurn::init() {
         GRAVITY = 9.81;
         DEGREE_PI = 180;
	 ZDEGREE = 150; // turn slice degree
	 XYDEGREE = 45; // do not change it
	 ZPERSISTENTNUM = 8; // for Z persistent num

	 mZPositive = false;
	 mInitFlag = false;
	 mZInitCondition = false;
	 mZInitPositive = false;
	 mXCondition = false;
	 mYCondition = false;
	 mZCondition = false;
	 mZInitData = 0.0;
	 mZConditionNum = 0;
	 mXAdjust = 4;
	 mYAdjust = 4;

	 mZinitLimit = 0.0;
	 mZMoveLimit = 0.0;
	 mXLimit = 0.0;
	 mYLimit = 0.0;



	double degreeDbl;
	degreeDbl = cos((DEGREE_PI - ZDEGREE) * 3.14159 / DEGREE_PI);
	mZinitLimit = degreeDbl * GRAVITY;
	mZMoveLimit = degreeDbl * GRAVITY + GRAVITY - 1.5;
	mXLimit = sin(XYDEGREE * 3.14159 / DEGREE_PI) * GRAVITY - mXAdjust;
	mYLimit = sin(XYDEGREE * 3.14159 / DEGREE_PI) * GRAVITY - mYAdjust;
 	LOGD("NativeActionTurn:: Init");
}
void NativeActionTurn:: nativeReceiveAccData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp) {
   if (Z > 0) {
		mZPositive = true;
	} else {
		mZPositive = false;
	}

	if (fabs(Z) > mZinitLimit && !mInitFlag) { // 8

		mZInitCondition = true;
		mZInitData = Z;
		mZInitPositive = mZPositive;
		mInitFlag = true;
		mXCondition = false;
		mYCondition = false;
	}

	if (fabs(X) > mXLimit) { // 7
		mXCondition = true;
	}

	if (fabs(Y) > mYLimit) { // 7
		mYCondition = true;
	}

	if ((fabs(mZInitData) + fabs(Z) > mZMoveLimit) // 16
			&& (mZPositive != mZInitPositive)) {
		mZConditionNum++;
		if (ZPERSISTENTNUM == mZConditionNum) {
			mZCondition = true;
		}
	} else {
		mZConditionNum = 0;
	}

	if (mZInitCondition && (mXCondition || mYCondition) && mZCondition) {
		LOGD("Hello NativeActionTurn It is successful!");
		mInitFlag = false;
		mZInitCondition = false;
		mXCondition = false;
		mYCondition = false;
		mZCondition = false;
		jclass actionTurn = env->FindClass("com/oppo/kinectlib/ActionTurn");
		jmethodID id = env->GetMethodID(actionTurn,"returnResult","()V");
		env->CallVoidMethod(thiz,id);
	}
}
