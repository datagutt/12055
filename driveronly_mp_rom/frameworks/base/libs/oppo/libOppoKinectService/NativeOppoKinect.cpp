/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include <NativeActionTurn.h>
#include <NativeActionPickUp.h>
#include <NativeActionBringToEar.h>
#include <NativeActionStatic.h>
#include <NativeActionLean.h>
#include <NativeActionCartWheel.h>
#include <NativeActionSwing.h>
#include <com_oppo_kinectlib_ActionLean.h>
#include <com_oppo_kinectlib_ActionStatic.h>
#include <com_oppo_kinectlib_ActionBringToEar.h>
#include <com_oppo_kinectlib_ActionTurn.h>
#include <com_oppo_kinectlib_ActionRemind.h>
#include <com_oppo_kinectlib_ActionPickUp.h>
#include <com_oppo_kinectlib_ActionCartWheel.h>
#include <com_oppo_kinectlib_ActionSwing.h>

#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"NativeOppoKinect",__VA_ARGS__)

NativeActionBringToEar *gActionBringToEar = NULL;
NativeActionTurn *gActionTurn = NULL;
NativeActionPickUp *gActionPickUp = NULL;
NativeActionStatic *mActionStatic = NULL;
NativeActionLean *mActionLean = NULL;
char *GSENSOR_INTERRUPT_EN =
		"proc/gyro_gsensor/gesture_enable";
char *GSENSOR_INTERRUPT_TRUE =
		"proc/gyro_gsensor/gesture_true";
/*NativeActionRemind *gActionRemind = NULL;*/

//canjie.zheng@platform.srv add 2-28
NativeActionCartWheel *mActionCartWheel = NULL;
NativeActionSwing *mActionSwing = NULL;
//add end
//////////////////////////////////////ActionTurn///////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionTurn_nativeReceiveAccData(
		JNIEnv* env, jobject thiz, jfloat X, jfloat Y, jfloat Z,
		jfloat accuracy, jfloat timestamp) {
	if (gActionTurn) {
		gActionTurn->nativeReceiveAccData(env, thiz, X, Y, Z, accuracy,
				timestamp);
	}
}

JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionTurn_nativeInit(
		JNIEnv* env, jobject thiz) {
	if (!gActionTurn) {
		gActionTurn = new NativeActionTurn();
	}
	gActionTurn->init();
}
/////////////////////////////////////ActionPickUp//////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionPickUp_nativeInit(
		JNIEnv* env, jobject thiz) {
	if (!gActionPickUp) {
		gActionPickUp = new NativeActionPickUp();
	}
	gActionPickUp->nativeInit();
}
#if 0 //canjie.zheng modified for distinguishing ActionPickUp by gyroscope 2013-03-18
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionPickUp_nativeReceiveAccData(
		JNIEnv* env, jobject thiz, jfloat X, jfloat Y, jfloat Z,
		jfloat accuracy, jfloat timestamp) {
	gActionPickUp->nativeReceiveAccData(env, thiz, X, Y, Z, accuracy, timestamp);
}

JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionPickUp_nativeReceiveDisData(
		JNIEnv* env, jobject thiz, jfloat distance, jfloat accuracy,
		jfloat timestamp) {
	gActionPickUp->nativeReceiveDisData(env, thiz, distance, accuracy,
			timestamp);
}
#else
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionPickUp_nativeReceiveGyrData(
		JNIEnv* env, jobject thiz, jfloat X, jfloat Y, jfloat Z,
		jfloat accuracy, jfloat timestamp) {
	gActionPickUp->nativeReceiveGyrData(env, thiz, X, Y, Z, accuracy, timestamp);
}
#endif
/////////////////////////////////////ActionRemind//////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionRemind_nativeSetGsensorInterruptEn(
		JNIEnv* env, jobject thiz, jboolean enable) {
	int fp = open(GSENSOR_INTERRUPT_EN,O_RDWR);
	char str1[2] = "1";
	char str2[2] = "0";
	if (fp != -1) {
		if (enable) {
			write(fp, str1, strlen(str1));
		} else {
			write(fp,str2, strlen(str2));
		}
	}
	close(fp);
}

JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionRemind_nativeSetGsensorInterruptTrue(
		JNIEnv* env, jobject thiz, jboolean enable) {
		
	int fp = open(GSENSOR_INTERRUPT_TRUE,O_RDWR);
	char str1[2] = "1";
	char str2[2] = "0";
	
	if (fp != -1) {
		if (enable) {
			write(fp, str1, strlen(str1));
		
		} else {
			write(fp, str2, strlen(str2));
		}
	}
	close(fp);
}

JNIEXPORT jboolean JNICALL Java_com_oppo_kinectlib_ActionRemind_nativeGetGsensorInterruptTrue(
		JNIEnv* env, jobject thiz) {
	
	FILE *fp = fopen(GSENSOR_INTERRUPT_TRUE, "rt");
	char str[2]={0};

	if (fp != NULL) {

		fread(str, 1, 1, fp);
		str[1] = 0;
		if (!strcmp(str, "1")) {
			return true;
		} else {
			return false;
		}
	fclose(fp);
	}
	return false;
}
/////////////////////////////////////ActionBringToEar//////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionBringToEar_nativeReceiveAccData( JNIEnv* env,
                                                  jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp)
{
     if(!gActionBringToEar){
	 	gActionBringToEar = new NativeActionBringToEar();	 
    }
    gActionBringToEar->nativeReceiveAccData(env,thiz,X,Y,Z,accuracy,timestamp);
    
}


JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionBringToEar_nativeReceiveDisData( JNIEnv* env,
                                                  jobject thiz,jfloat distan,jfloat accuracy,jfloat timestamp)
{
     if(!gActionBringToEar){
	 	gActionBringToEar = new NativeActionBringToEar();	
    }
    gActionBringToEar->nativeReceiveDisData(env,thiz,distan,accuracy,timestamp);
    
}
#if 1 //canjie.zheng add for Gyroscope
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionBringToEar_nativeReceiveGyrData( JNIEnv* env,
                                                  jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp)
{
     if(!gActionBringToEar){
	 	gActionBringToEar = new NativeActionBringToEar();	 
    }
    gActionBringToEar->nativeReceiveGyrData(env,thiz,X,Y,Z,accuracy,timestamp);

}

#endif
/////////////////////////////////////ActionStatic//////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionStatic_nativeInit( JNIEnv* env,
                                                  jobject thiz ){
	if(!mActionStatic){
	    mActionStatic = new NativeActionStatic();
	 }
	mActionStatic->init();
}
#if 0 //ccanjie.zheng modified for distinguishing ActionStatic by gyroscope 2013-03-18
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionStatic_nativeReceiveAccData(JNIEnv* env, jobject thiz, jfloat x, jfloat y, jfloat z)
{
	if(mActionStatic){
    	mActionStatic->nativeReceiveAccData(env,thiz,x,y,z);
    }
}
#else
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionStatic_nativeReceiveGyrData(JNIEnv* env, jobject thiz, jfloat x, jfloat y, jfloat z)
{
	if(mActionStatic){
    	mActionStatic->nativeReceiveGyrData(env,thiz,x,y,z);
    }
}

#endif //canjie.zheng end
/////////////////////////////////////ActionLean//////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionLean_nativeInit( JNIEnv* env,
                                                  jobject thiz ){
	if(!mActionLean){
	    mActionLean = new NativeActionLean();
	 }
	mActionLean->init();
}

JNIEXPORT jboolean JNICALL Java_com_oppo_kinectlib_ActionLean_nativeSetparameter(JNIEnv* env, jobject object, jfloatArray parameter, jint length)
{
	jboolean result;
	int mlength = length;
	result = false;
	jsize theArrayLength = env->GetArrayLength(parameter);  
  jfloat* elems = env->GetFloatArrayElements(parameter,NULL);
     
	if(mActionLean){
    	result = mActionLean->nativeSetparameter((float *)elems, mlength);
    }
    
  return result;
}

JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionLean_nativeReceiveAccData(JNIEnv* env, jobject object, jfloat x, jfloat y, jfloat z)
{
	if(mActionLean){
    	mActionLean->nativeReceiveAccData(env,object,x,y,z);
    }
}

//add by canjie.zheng@platform.srv ActionCartWheel
/////////////////////////////////////ActionCartWheel//////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionCartWheel_nativeReceiveGyrData
  (JNIEnv *env, jobject object, jfloat x, jfloat y, jfloat z, jfloat accuracy, jfloat timestamp)
{
	if(mActionCartWheel){
		mActionCartWheel->nativeReceiveGyrData(env,object,x,y,z,accuracy,timestamp);
    }
}
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionCartWheel_nativeReceiveAccData
  (JNIEnv *env, jobject object, jfloat x, jfloat y, jfloat z, jfloat accuracy, jfloat timestamp)
{
	if(mActionCartWheel){
		mActionCartWheel->nativeReceiveAccData(env,object,x,y,z,accuracy,timestamp);
    }
}
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionCartWheel_nativeInit
  (JNIEnv *, jobject)
{
	if(!mActionCartWheel){
		mActionCartWheel = new NativeActionCartWheel();
	 }
	mActionCartWheel->init();
}
///////////////////////////////////////ActionSwing/////////////////////////////////////////////////
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionSwing_nativeReceiveGyrData
   (JNIEnv *env, jobject object, jfloat x, jfloat y, jfloat z, jfloat accuracy, jfloat timestamp)
{
	if(mActionSwing) 
	{
		mActionSwing->nativeReceiveGyrData(env,object,x,y,z,accuracy,timestamp);
	}
}
JNIEXPORT void JNICALL Java_com_oppo_kinectlib_ActionSwing_nativeInit
  (JNIEnv *, jobject)
{
	if(!mActionSwing)
	{
		mActionSwing = new NativeActionSwing();
	 }
	mActionSwing->init();

}
JNIEXPORT jboolean JNICALL Java_com_oppo_kinectlib_ActionSwing_nativeSetparameter
  (JNIEnv* env, jobject object, jfloatArray parameter, jint length)
{
	
	jboolean result;
	int mlength = length;
	result = false;
  	jsize theArrayLength = env->GetArrayLength(parameter);  
	jfloat* elems = env->GetFloatArrayElements(parameter,NULL);
	   
	if(mActionSwing)
	{
		result = mActionSwing->nativeSetparameter((float *)elems, mlength);
	}
	  
	return result;
}

