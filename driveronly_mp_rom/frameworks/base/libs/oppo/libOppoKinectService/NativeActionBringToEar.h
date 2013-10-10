/*
 * NativeActionBringToEar.h
 *
 *  Created on: 2013-1-28
 *      Author: 80056741
 */

#ifndef NATIVEACTIONBRINGTOEAR_H_
#define NATIVEACTIONBRINGTOEAR_H_
#include<stdio.h>
#include<jni.h>

typedef struct {
   JNIEnv *env;
    jobject object;
} native_data_t;

class NativeActionBringToEar {
public:
	NativeActionBringToEar();
	virtual ~NativeActionBringToEar();
	void nativeReceiveAccData(JNIEnv* env,
                                                  jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp);
	int nativeReceiveDisData(JNIEnv* env,
                                                  jobject thiz,jfloat distance,jfloat accuracy,jfloat timestamp);
	//canjie.zheng add for gyroscope 2013-03-27
	void nativeReceiveGyrData(JNIEnv* env,
                                                  jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp);
	//end
private:
	 static void *ThreadWrapper(void *);
    	int checkPhoneposition(); 
	//canjie.zheng add

	float gravite[3];
	float mTimeStamp;
	float NS2S;
	float EPSILON;
	float anglez[20];
	float MAXVALUE;
	int gyrIndex;

	float motionZ[20];
	bool waitFor;
	float waitForAngle;
	float sumAngle;
	bool CheckTrackIsRight();
	//end
};

#endif /* NATIVEActionBringToEar_H_ */
