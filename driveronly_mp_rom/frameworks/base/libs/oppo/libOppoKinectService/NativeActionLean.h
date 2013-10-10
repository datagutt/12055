/*
 * NativeActionLean.h
 *
 *  Created on: 2013-1-28
 *      Author: 80055923
 */

#ifndef NATIVEACTIONLEAN_H_
#define NATIVEACTIONLEAN_H_
#include<stdio.h>
#include<jni.h>

class NativeActionLean {
public:
	NativeActionLean();
	virtual ~NativeActionLean();
	void nativeReceiveAccData(JNIEnv* env,jobject object,jfloat X,jfloat Y,jfloat Z);
	bool nativeSetparameter(float parameter[],int length);
	void init();
private:
	// the value of return mresult[1]
	int LEFT;
	int RIGHT;
	int DOWN;
	int UP;
	int LEFTF;
	int RIGHTF;
	int DOWNF;
	int UPF;

	int XYMINDEGREE; // degree of limit range
	int XYMAXDEGREE; // degree of fast limit range
	int ONESTEPCOUNT; // first step count time
	int XYCOUNTLIMIT; // x,y direct start judge time and step time
	int FASTTIMELIMIT; // fast step time

	int mresult[4];
	
	float x, y, z;
	float resetx, resety, resetz; // reset
	float degreex, degreey, resetdegreex, resetdegreey, diffx, diffy;
	int COUNTL, COUNTLF, COUNTR, COUNTRF, COUNTU, COUNTUF, COUNTD, COUNTDF;
	
	// judge if it is the first time
	bool ISFIRSTL;
	bool ISFIRSTLF;
	bool ISFIRSTR;
	bool ISFIRSTRF;
	bool ISFIRSTD;
	bool ISFIRSTDF;
	bool ISFIRSTU;
	bool ISFIRSTUF;
  bool ISINX;
	bool ISINY;
};

#endif /* NATIVEACTIONLEAN_H_ */
