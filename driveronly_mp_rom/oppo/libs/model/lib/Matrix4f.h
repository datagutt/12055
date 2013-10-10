
#include <model/Model.h>

#ifndef _Included_Matrix4f_H
#define _Included_Matrix4f_H
#ifdef __cplusplus
extern "C" {
#endif

void matrix4fSetIdentity(matrix4f_t* matrix);

void matrix4fsetTranslationv(matrix4f_t* matrix, vector3f_t trans);

void matrix4fsetTranslationp(matrix4f_t* matrix, float x, float y, float z);

void matrix4fRotX(matrix4f_t* matrix, float angle);

void matrix4fRotY(matrix4f_t* matrix, float angle);

void matrix4fSet(matrix4f_t* matrix, matrix4f_t src);

void matrix4fMulWith(matrix4f_t* matrix, matrix4f_t m1);

void matrix4fMulResult(matrix4f_t* matrix, matrix4f_t m1, matrix4f_t m2);

void matrix4fSetQ(matrix4f_t* matrix, quat4f_t q1);

void matrix4fInvTransform(matrix4f_t* matrix, vector3f_t point, vector3f_t* pointOut);

void matrix4fInvTransformRotate(matrix4f_t* matrix, vector3f_t normal, vector3f_t* normalOut);

void matrix4fTransform(matrix4f_t* matrix, vector3f_t* point, vector3f_t* pointOut);

void matrix4fTranspose(matrix4f_t* matrix);

void matrix4fInvert(matrix4f_t* matrix);

void matrix4fInvertGeneral(matrix4f_t* matrix);

bool matrix4fLuDecomposition(double* matrix0, int* row_perm);

void matrix4fLuBacksubstitution(double* matrix1, int* row_perm, double* matrix2);

void matrix4fGluLookAt(vector3f_t eye, vector3f_t center, vector3f_t up, matrix4f_t* out);

void matrix4fGluPersective(float fovy, float aspect, float zNear, float zFar, matrix4f_t* out);

#ifdef __cplusplus
}
#endif
#endif
