/*
 * NativeActionCartWheel.h
 *
 *  Created on: 2013-2-28
 *      Author: canjie.zheng
 */

#ifndef NATIVEAACRIONCARTWHEEL_H_
#define NATIVEAACRIONCARTWHEEL_H_
#include<stdio.h>
#include<jni.h>


class NativeActionCartWheel {
public:
	NativeActionCartWheel();
	virtual ~NativeActionCartWheel();
	void nativeReceiveAccData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp);
	void nativeReceiveGyrData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp);
	void init();
private:
    const float MININITX;
    const float MAXINITX; 
    const float MINENDX;
	const double NS2S;
	const float EPSILON;
	float sumAngleZ;

    float gravity[3];
	float motion[3];
	float mTimestamp;
	float angleZ;
	int direction;
	bool IsWheel;

	bool DEBUG;
};

#endif /* NATIVEAACRIONCARTWHEEL_H_ */
