/*
 * NativeActionSwing.h
 *
 *  Created on: 2013-3-18
 *      Author: 80049072
 */

#ifndef NativeActionSwing_H_
#define NativeActionSwing_H_
#include<stdio.h>
#include<jni.h>



class NativeActionSwing {
public:
	NativeActionSwing();
	virtual ~NativeActionSwing();
	void nativeReceiveGyrData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp);
	void init();
	bool nativeSetparameter(float parameter[], int length);
private:
    int refresh;
	int ACTION_SWING;
	float dynamics;
	float mx, my, mz;
	int freq;
	bool DEBUG;

};

#endif /* NativeActionSwing_H_ */

