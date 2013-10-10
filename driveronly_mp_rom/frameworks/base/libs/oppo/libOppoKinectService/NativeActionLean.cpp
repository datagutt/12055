/*
 * NativeActionLean.cpp
 *
 *  Created on: 2013-1-28
 *      Author: 80055923
 */

#include "NativeActionLean.h"
#include <math.h>

#include <android/log.h>
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"NativeActionLean",__VA_ARGS__)

NativeActionLean::NativeActionLean(){

}

NativeActionLean::~NativeActionLean() {
	// TODO Auto-generated destructor stub
}
void NativeActionLean::init() {
	// the value of return mresult[1]
	LEFT = 0;
	RIGHT = 1;
	DOWN = 2;
	UP = 3;
	LEFTF = 4;
	RIGHTF = 5;
	DOWNF = 6;
	UPF = 7;

	XYMINDEGREE = 10; // degree of limit range
	XYMAXDEGREE = 25; // degree of fast limit range
	ONESTEPCOUNT = 17; // first step count time
	XYCOUNTLIMIT = 17; // x,y direct start judge time and step time
	FASTTIMELIMIT = 5; // fast step time
	
	mresult[0] = 2;
	mresult[1] = 0;
	// judge if it is the first time
	ISFIRSTL = false;
	ISFIRSTLF = false;
	ISFIRSTR = false;
	ISFIRSTRF = false;
	ISFIRSTD = false;
	ISFIRSTDF = false;
	ISFIRSTU = false;
	ISFIRSTUF = false;
  ISINX = false;
	ISINY = false;
	
	resetx = 0;
	resety = 0;
	resetz = 0;
}

float caculatedegree(float x, float z) 
	{
		float degree, degreex;
		if (x >= 9.81 || x <= -9.81) {
			degreex = 0;
		}else {
		degreex = acos(fabs(x) / 9.81) / 3.1415926 * 180;
	}
		if (x >= 0) {
			if (z >= 0) {
				degree = 90 - degreex;
			} else {
				degree = 90 + degreex;
			}
		} else {
			if (z >= 0) {
				degree = 270 + degreex;
			} else {
				degree = 270 - degreex;
			}
		}		
		return degree;
	}	
	
void NativeActionLean:: nativeReceiveAccData(JNIEnv* mEnv,jobject mObject,jfloat X,jfloat Y,jfloat Z) {
	jclass actionLean;
	jmethodID id;
	jintArray jresult;
	x = X;
	y = Y;
	z = Z;
	if ((resetx > -0.000001 && resetx < 0.000001) && (resety > -0.000001 && resety < 0.000001) 
		&& (resetz > -0.000001 && resetz < 0.000001)) {
			resetx = x;
			resety = y;
			resetz = z;
			
			resetdegreex = caculatedegree(resetx, resetz);
			resetdegreey = caculatedegree(resety, resetz);
			return;
		}
	
		degreex = caculatedegree(x, z);
		degreey = caculatedegree(y, z);
		diffx = degreex - resetdegreex;
		diffy = degreey - resetdegreey;
		//LOGD("NativeActionLean:: caculatedegree %f  %f",diffx,diffy);
		ISINX = (ISFIRSTL || ISFIRSTLF || ISFIRSTR || ISFIRSTRF);
		ISINY = (ISFIRSTD || ISFIRSTDF || ISFIRSTU || ISFIRSTUF);

		//left
		if ((diffx >= XYMINDEGREE && diffx < XYMAXDEGREE)
				|| (-diffx > 180 && (360 + diffx) >= XYMINDEGREE && (360 + diffx) < XYMAXDEGREE)) {		
			COUNTL++;
			if (ISFIRSTL == false && (COUNTL == ONESTEPCOUNT || ISINX)) {
				mresult[1] = LEFT;// left incline
				mresult[2] = (int) diffx;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
				COUNTL = 0;
				ISFIRSTL = true;
			} else if (ISFIRSTL == true && COUNTL == XYCOUNTLIMIT) {
				COUNTL = 0;
				mresult[1] = LEFT;// left incline
				mresult[2] = (int) diffx;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
			}
		} else {
			COUNTL = 0;
			ISFIRSTL = false;
		}
		//left fast
		if ((diffx >= XYMAXDEGREE && diffx <= 180)
				|| (-diffx > 180 && (360 + diffx) >= XYMAXDEGREE)) {
			COUNTLF++;
			if (ISFIRSTLF == false && (COUNTLF == ONESTEPCOUNT || ISINX)) {
				mresult[1] = LEFTF;
				mresult[2] = (int) diffx;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
				COUNTLF = 0;
				ISFIRSTLF = true;
			} else if (ISFIRSTLF == true && COUNTLF == FASTTIMELIMIT) {
				COUNTLF = 0;
				mresult[1] = LEFTF;
				mresult[2] = (int) diffx;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
			}
		} else {
			COUNTLF = 0;
			ISFIRSTLF = false;
		}
		//right
		if ((-diffx >= XYMINDEGREE && -diffx < XYMAXDEGREE)
				|| (diffx > 180 && (360 - diffx) >= XYMINDEGREE && (360 - diffx) < XYMAXDEGREE)) {
			COUNTR++;
			if (ISFIRSTR == false && (COUNTR == ONESTEPCOUNT || ISINX)) {
				mresult[1] = RIGHT;
				mresult[2] = (int) diffx;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
				COUNTR = 0;
				ISFIRSTR = true;
			} else if (ISFIRSTR == true && COUNTR == XYCOUNTLIMIT) {
				COUNTR = 0;
				mresult[1] = RIGHT;
				mresult[2] = (int) diffx;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
			}
		} else {
			COUNTR = 0;
			ISFIRSTR = false;
		}
		//right fast
		if ((-diffx >= XYMAXDEGREE && -diffx <= 180)
				|| (diffx > 180 && (360 - diffx) >= XYMAXDEGREE)) {
			COUNTRF++;
			if (ISFIRSTRF == false && (COUNTRF == ONESTEPCOUNT || ISINX)) {
				mresult[1] = RIGHTF;
				mresult[2] = (int) diffx;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
				COUNTRF = 0;
				ISFIRSTRF = true;
			} else if (ISFIRSTRF == true && COUNTRF == FASTTIMELIMIT) {
				COUNTRF = 0;
				mresult[1] = RIGHTF;
				mresult[2] = (int) diffx;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
			}
		} else {
			COUNTRF = 0;
			ISFIRSTRF = false;
		}
		//down
		if ((diffy >= XYMINDEGREE && diffy < XYMAXDEGREE)
				|| (-diffy > 180 && (360 + diffy) >= XYMINDEGREE && (360 + diffy) < XYMAXDEGREE)) {
			COUNTD++;
			if (ISFIRSTD == false && (COUNTD == ONESTEPCOUNT || ISINY)) {
				mresult[1] = DOWN;
				mresult[3] = (int) diffy;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
				COUNTD = 0;
				ISFIRSTD = true;
			} else if (ISFIRSTD == true && COUNTD == XYCOUNTLIMIT) {
				COUNTD = 0;
				mresult[1] = DOWN;
				mresult[3] = (int) diffy;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
			}
		} else {
			COUNTD = 0;
			ISFIRSTD = false;
		}
		//down fast
		if ((diffy >= XYMAXDEGREE && diffy <= 180)
				|| (-diffy > 180 && (360 + diffy) >= XYMAXDEGREE)) {
			COUNTDF++;
			if (ISFIRSTDF == false && (COUNTDF == ONESTEPCOUNT || ISINY)) {
				mresult[1] = DOWNF;
				mresult[3] = (int) diffy;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
				COUNTDF = 0;
				ISFIRSTDF = true;
			} else if (ISFIRSTDF == true && COUNTDF == FASTTIMELIMIT) {
				COUNTDF = 0;
				mresult[1] = DOWNF;
				mresult[3] = (int) diffy;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
			}
		} else {
			COUNTDF = 0;
			ISFIRSTDF = false;
		}
		//up
		if ((-diffy >= XYMINDEGREE && -diffy < XYMAXDEGREE)
				|| (diffy > 180 && (360 - diffy) >= XYMINDEGREE && (360 - diffy) < XYMAXDEGREE)) {
			COUNTU++;
			if (ISFIRSTU == false && (COUNTU == ONESTEPCOUNT || ISINY)) {
				mresult[1] = UP;
				mresult[3] = (int) diffy;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
				COUNTU = 0;
				ISFIRSTU = true;
			} else if (ISFIRSTU == true && COUNTU == XYCOUNTLIMIT) {
				COUNTU = 0;
				mresult[1] = UP;
				mresult[3] = (int) diffy;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
			}
		} else {
			COUNTU = 0;
			ISFIRSTU = false;
		}
		//up fast
		if ((-diffy >= XYMAXDEGREE && -diffy <= 180)
				|| (diffy > 180 && (360 - diffy) >= XYMAXDEGREE)) {
			COUNTUF++;
			if (ISFIRSTUF == false && (COUNTUF == ONESTEPCOUNT || ISINY)) {
				mresult[1] = UPF;
				mresult[3] = (int) diffy;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
				COUNTUF = 0;
				ISFIRSTUF = true;
			} else if (ISFIRSTUF == true && COUNTUF == FASTTIMELIMIT) {
				COUNTUF = 0;
				mresult[1] = UPF;
				mresult[3] = (int) diffy;
				jresult = mEnv->NewIntArray(4);
				actionLean = mEnv->FindClass("com/oppo/kinectlib/ActionLean");
				id = mEnv->GetMethodID(actionLean,"returnResult","([I)V");	              
				mEnv->SetIntArrayRegion(jresult, 0, 4, mresult);
				mEnv->CallVoidMethod(mObject,id,jresult);
			}
		} else {
			COUNTUF = 0;
			ISFIRSTUF = false;
		}
}

bool NativeActionLean:: nativeSetparameter(float parameter[], int length) {
	if (length <= 16 && length % 2 == 0) {
			for (int i = 0; i < length; i += 2) {
				if (parameter[i] == 0) {
					XYMINDEGREE = (int) parameter[i + 1];
				}
				if (parameter[i] == 1) {
					XYMAXDEGREE = (int) parameter[i + 1];
				}
				if (parameter[i] == 2) {
					XYCOUNTLIMIT = (int) (parameter[i + 1] * 50);
				}
				if (parameter[i] == 3) {
					FASTTIMELIMIT = (int) (parameter[i + 1] * 50);
				}
				if (parameter[i] == 4) {
					ONESTEPCOUNT = (int) (parameter[i + 1] * 50);
				}
				if (parameter[i] == 5) {
					if (parameter[i + 1] > 9999.999999 && parameter[i + 1] < 10000.000001) {
						resetx = 0;
					}else {
						resetx = parameter[i + 1];
					}
				}
				if (parameter[i] == 6) {
					if (parameter[i + 1] > 9999.999999 && parameter[i + 1] < 10000.000001) {
						resety = 0;
					}else {
						resety = parameter[i + 1];
					}			
				}
				if (parameter[i] == 7) {
					if (parameter[i + 1] > 9999.999999 && parameter[i + 1] < 10000.000001) {
						resetz = 0;
					}else {
						resetz = parameter[i + 1];
					}
				}
			}
			if(resetz < -0.000001 || resetz > 0.000001) {
				resetdegreex = caculatedegree(resetx, resetz);
				resetdegreey = caculatedegree(resety, resetz);
			}
			return true;
		} else {
			return false;
		}
}