
#include <model/Model.h>

#ifndef _Included_MathLib_H
#define _Included_MathLib_H
#ifdef __cplusplus
extern "C" {
#endif

void ClearBounds(vector3f_t* mins, vector3f_t* maxs);

void AddPointToBounds(vector3f_t v, vector3f_t* mins, vector3f_t* maxs);

void AngleMatrix(vector3f_t angles, quat4f_t* matrix);

void R_ConcatTransforms(quat4f_t* in1, quat4f_t* in2, quat4f_t* out);

void VectorRotate(vector3f_t in1, quat4f_t* in2, vector3f_t* out);

float DotProduct(vector3f_t v, quat4f_t q);

void VectorIRotate(vector3f_t in1, quat4f_t* in2, vector3f_t* out);

void VectorTransform(vector3f_t in1, quat4f_t* in2, vector3f_t* out);

void VectorITransform(vector3f_t in1, quat4f_t* in2, vector3f_t* out);

void AngleQuaternion(vector3f_t angles, quat4f_t* quaternion);

void QuaternionMatrix(quat4f_t quaternion, quat4f_t* matrix);

void QuaternionSlerp(quat4f_t p, quat4f_t q, float t, quat4f_t* qt);

#ifdef __cplusplus
}
#endif
#endif
