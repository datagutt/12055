/*
 * NativeActionStatic.h
 *
 *  Created on: 2013-1-28
 *      Author: 80055923
 */

#ifndef NATIVEACTIONSTATIC_H_
#define NATIVEACTIONSTATIC_H_
#include<stdio.h>
#include<jni.h>

class NativeActionStatic {
public:
	NativeActionStatic();
	virtual ~NativeActionStatic();
#if 0 //canjie.zheng modified for distinguishing ActionStatic by gyroscope 2013-03-18
	void nativeReceiveAccData(JNIEnv* env, jobject thiz, jfloat x, jfloat y, jfloat z);
#else 
	void nativeReceiveGyrData(JNIEnv* env, jobject thiz, jfloat x, jfloat y, jfloat z);
#endif //canjie.zheng end
	void init();
private:

	int NOTSTATIC;
	int ISSTATIC;
	int COUNT, TIME;
	int mresult[2];
	int COUNTLIMIT;
#if 0 //canjie.zheng modified for distinguishing ActionStatic by gyroscope 2013-03-18

	float XYZLIMIT;
	float x, y, z, lastx, lasty, lastz;
	
#else
	float x, y, z;
	bool DEBUG;
	#endif
//end
};

#endif /* NATIVEACTIONSTATIC_H_ */
