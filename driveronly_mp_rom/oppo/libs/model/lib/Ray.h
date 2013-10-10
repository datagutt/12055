
#include <model/Model.h>

#ifndef _Included_Ray_H
#define _Included_Ray_H
#ifdef __cplusplus
extern "C" {
#endif

void rayTransform(matrix4f_t matrix, ray_t out);

bool rayIntersectTriangle(ray_t ray, vector3f_t v0, vector3f_t v1, vector3f_t v2, quat4f_t* location);

bool rayIntersect(ray_t ray, vector3f_t v0, vector3f_t v1, vector3f_t v2, quat4f_t* location);

#ifdef __cplusplus
}
#endif
#endif
