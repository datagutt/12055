/*
 * NativeActionStatic.cpp
 *
 *  Created on: 2013-1-28
 *      Author: 80055923
 */

#include "NativeActionStatic.h"
#include <math.h>

#include <android/log.h>
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"NativeActionStatic",__VA_ARGS__)

NativeActionStatic::NativeActionStatic(){

}


NativeActionStatic::~NativeActionStatic() {
	// TODO Auto-generated destructor stub
}
void NativeActionStatic::init() {
	NOTSTATIC = 0;
	ISSTATIC = 1;
	COUNTLIMIT = 50;
	mresult[0]	= 3;
	COUNT = 0;
	TIME = 0;
#if 0 // canjie.zheng modified for distinguishing ActionStatic by gyroscope 2013-03-18
	XYZLIMIT = 2;
	lastx = 0;
	lasty = 0;
	lastz = 0;
#else 
	DEBUG = true;
#endif 
//end
}

#if 0 // canjie.zheng add for distinguishing ActionStatic by gyroscope 2013-03-18
void NativeActionStatic:: nativeReceiveAccData(JNIEnv* env, jobject thiz,jfloat X,jfloat Y,jfloat Z) {
	x = X;
	y = Y;
	z = Z;
	
	if (lastx == 0 && lasty == 0 && lastz == 0) {
		lastx = x;
		lasty = y;
		lastz = z;
	} else {
		float DIFFX = fabs(lastx - x);
		float DIFFY = fabs(lasty - y);
		float DIFFZ = fabs(lastz - z);
		lastx = x;
		lasty = y;
		lastz = z;

		TIME++;
		if (DIFFX < XYZLIMIT && DIFFY < XYZLIMIT && DIFFZ < XYZLIMIT) {
			COUNT++;
		}
	}

	if (TIME == COUNTLIMIT) {
		if (COUNT == TIME) {
			mresult[1] = ISSTATIC;
		} else {
			mresult[1] = NOTSTATIC;
		}		
		COUNT = 0;
		TIME = 0;
		
		jintArray jresult = env->NewIntArray(2);
		env->SetIntArrayRegion(jresult, 0, 2, mresult);	
	    jclass actionStatic = env->FindClass("com/oppo/kinectlib/ActionStatic");
	    jmethodID id = env->GetMethodID(actionStatic,"returnResult","([I)V");
		env->CallVoidMethod(thiz,id,jresult);
	}
}

#else                     
void NativeActionStatic:: nativeReceiveGyrData(JNIEnv* env, jobject thiz,jfloat X,jfloat Y,jfloat Z) 
{
	x = X;
	y = Y;
	z = Z;
	++TIME;
	

	if (fabs(x)<0.20001f && fabs(y)<0.20001f && fabs(z)<0.20001f) 
	{
		++COUNT;

	}
	if (TIME == COUNTLIMIT) 
	{
		if (COUNT == TIME) 
		{
			mresult[1] = ISSTATIC;
		} else 
		{
			mresult[1] = NOTSTATIC;
		}		
		COUNT = 0;
		TIME = 0;
		
		jintArray jresult = env->NewIntArray(2);
		env->SetIntArrayRegion(jresult, 0, 2, mresult); 
		jclass actionStatic = env->FindClass("com/oppo/kinectlib/ActionStatic");
		jmethodID id = env->GetMethodID(actionStatic,"returnResult","([I)V");
		env->CallVoidMethod(thiz,id,jresult);
	}
}
#endif //canjie.zheng end
