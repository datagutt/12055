/*
 * NativeActionTurn.h
 *
 *  Created on: 2013-1-28
 *      Author: 80054323
 */

#ifndef NATIVEACTIONTURN_H_
#define NATIVEACTIONTURN_H_
#include<stdio.h>
#include<jni.h>


class NativeActionTurn {
public:
	NativeActionTurn();
	virtual ~NativeActionTurn();
	void nativeReceiveAccData(JNIEnv* env,jobject thiz,jfloat X,jfloat Y,jfloat Z,jfloat accuracy,jfloat timestamp);
	void init();
private:
	double GRAVITY;
        int DEGREE_PI;
	int ZDEGREE; // turn slice degree
	int XYDEGREE; // do not change it
	int ZPERSISTENTNUM; // for Z persistent num

	bool mZPositive;
	bool mInitFlag;
	bool mZInitCondition;
	bool mZInitPositive;
	bool mXCondition;
	bool mYCondition;
	bool mZCondition;
	double mZInitData;
	int mZConditionNum;
	double mXAdjust;
	double mYAdjust;

	double mZinitLimit;
	double mZMoveLimit;
	double mXLimit;
	double mYLimit;
};

#endif /* NATIVEACTIONTURN_H_ */
