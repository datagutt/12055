/*
 * NativeActionPickUp.h
 *
 *  Created on: 2013-2-5
 *      Author: 80054323
 */

#ifndef NATIVEACTIONPICKUP_H_
#define NATIVEACTIONPICKUP_H_
#include<jni.h>
class NativeActionPickUp {
public:
	NativeActionPickUp();
	virtual ~NativeActionPickUp();
	void nativeInit();
#if 0 //canjie.zheng modified for distinguishing ActionPickUp by gyroscope
	void nativeReceiveAccData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp);
	void nativeReceiveDisData(JNIEnv* env,jobject thiz,jfloat distance,jfloat accuracy,jfloat timestamp);
#else
	void nativeReceiveGyrData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp);
#endif
private:
	float x, y, z;
#if 0 // canjie.zheng modified for distinguishing ActionPickUp by gyroscope
    double GRAVITY;
    double InitFlagLimit;
    double PickUpLimit;
    float DIS_NEAR;
    float DIS_FAR;
    float total;

    float disData;
    double totalMin, totalMax;
    double pickUpMin, pickUpMax;
    bool mInitFlag;
    bool mUpFlag;
#else
	//canjie.zheng add 
	float mTimeStamp;
	float NS2S;
	float EPSILON;
	float anglex;
	float angley;
	float MAXVALUE;
	float Init;
	int timeCount;
	int TIMECOUNTLIMIT;
	bool DEBUG;
#endif 	//add end
};

#endif /* NATIVEACTIONPICKUP_H_ */
