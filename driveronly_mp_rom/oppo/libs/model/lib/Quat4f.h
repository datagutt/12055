
#include <model/Model.h>

#ifndef _Included_Quat4f_H
#define _Included_Quat4f_H
#ifdef __cplusplus
extern "C" {
#endif

void quat4fSet4f(quat4f_t* quat, float x, float y, float z, float w);

void quat4fSet3f(quat4f_t* quat, float x, float y, float z);

void quat4fSetQ(quat4f_t* quat, quat4f_t q);

void quat4fSetV(quat4f_t* quat, vector3f_t v);

void quat4fAdd(quat4f_t* quat, float x, float y, float z);

#ifdef __cplusplus
}
#endif
#endif
