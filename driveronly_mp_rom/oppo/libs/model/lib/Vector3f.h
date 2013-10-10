
#include <model/Model.h>

#ifndef _Included_Vector3f_H
#define _Included_Vector3f_H
#ifdef __cplusplus
extern "C" {
#endif

void vector3fSet(vector3f_t* src, vector3f_t* dest);

void vector3fAdd(vector3f_t* src, vector3f_t* dest);

void vector3fSub(vector3f_t* src, vector3f_t* dest);

void vector3fSubResult(vector3f_t* dest, vector3f_t* v1, vector3f_t* v2);

float vector3fDot(vector3f_t* v, vector3f_t* v1);

void vector3fCross(vector3f_t* v, vector3f_t* v1, vector3f_t *v2);

void vector3fNormalize(vector3f_t* v);

float vector3fDistance(vector3f_t v0, vector3f_t v1);

#ifdef __cplusplus
}
#endif
#endif
