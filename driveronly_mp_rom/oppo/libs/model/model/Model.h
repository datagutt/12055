
#ifndef _Included_Model_H
#define _Included_Model_H
#ifdef __cplusplus
extern "C" {
#endif


#define Q_PI				3.14159265358979323846f

#define	MAX_ABSOLUTE_ERROR	0.000001f

#define ATAN2_45_DEGREE		1.20710675f

#define REGION_SCALE		1.1f

#define REGION_DELTA_SCALE	0.1f


#define true	1
#define false	0

#define KEY_MAX_DISTANCE					0xFFFF
#define KEY_MAX_VERTEX_COORD_VALUE			0xFFFF

#define KEY_FRAME_RIGHT_BOTTOM				0

#define TRIANGLE_VERTEX_INDEX_FIRST			0
#define TRIANGLE_VERTEX_INDEX_SECOND		1
#define TRIANGLE_VERTEX_INDEX_THIRD			2

#define TRIANGLE_VERTEX_NUM					3
#define VERTEX_VECTOR3F_DIMENSION			3
#define TRIANGLE_VERTEX_DATA_NUM			TRIANGLE_VERTEX_NUM * VERTEX_VECTOR3F_DIMENSION
#define VERTEX_TEX_COORD_DIMENSION			2
#define TRIANGLE_TEX_COOTD_DATA_NUM			TRIANGLE_VERTEX_NUM * VERTEX_TEX_COORD_DIMENSION
#define VERTEX_NORMAL_DIMENSION				3
#define TRIANGLE_NORMAL_DATA_NUM			TRIANGLE_VERTEX_NUM * VERTEX_NORMAL_DIMENSION
#define VERTEX_COLOR_DIMENSION				4
#define TRIANGLE_COLOR_DATA_NUM			TRIANGLE_VERTEX_NUM * VERTEX_COLOR_DIMENSION


#define TRIANGLE_VERTEX_REVERSE				2

#define VERTEX_X_AXIS_INDEX			0
#define VERTEX_Y_AXIS_INDEX			1
#define VERTEX_Z_AXIS_INDEX			2
#define VERTEX_W_AXIS_INDEX			3

#define MAX_SCRIPT_ARRAY_INDEX_OFFSET		1

#define COLOR_DATA_NUM				3
#define COLOR_RED_INDEX				0
#define COLOR_GREEN_INDEX			1
#define COLOR_BLUE_INDEX			2

#define LIGHT_DATA_DIMENSION		4

#define REGION_VERTEX_NUM			4
#define REGION_TRIANGLE_NUM			2
#define REGION_VERTEX_DATA_NUM		(REGION_TRIANGLE_NUM * TRIANGLE_VERTEX_NUM)

#define REGION_VERTEX_LEFT_TOP		0
#define REGION_VERTEX_RIGHT_TOP		1
#define REGION_VERTEX_RIGHT_BOTTOM	2
#define REGION_VERTEX_LEFT_BOTTOM	3

#define REGION_FRAME_INDEX			0
#define REGION_POSITION_DELTA		1000


#define SHADOW_BASE_COLOR			0.8f
#define SHADOW_BASE_ALPHA			1.0f
#define SHADOW_HEIGHT_RATE			0.5f

#define BUFFER						"Ljava/nio/Buffer;"
#define FBUFFER						"Ljava/nio/FloatBuffer;"
#define MSCONFIG					"Lcom/oppo/model/ms3d/MsModel$Config;"
#define MSBUFFER					"Lcom/oppo/model/ms3d/MsModel$RenderBuffer;"
#define MDCONFIG					"Lcom/oppo/model/md2/MdModel$Config;"
#define MDBUFFER					"Lcom/oppo/model/md2/MdModel$RenderBuffer;"

#define MAX(a, b)	(a) > (b) ? (a) : (b)
#define MIN(a, b)	(a) > (b) ? (b) : (a)
#define ABS(a)		(a) >= 0 ? (a) : -(a)

#define MC_FREE(ptr)	\
	{\
		if(ptr != NULL)\
		{\
			free(ptr);\
			ptr = NULL;\
		}\
	}

	

typedef unsigned char byte;

typedef unsigned short word;

typedef int bool;


typedef struct {
    float x;
    float y;
    float z;
} vector3f_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} quat4f_t;

typedef struct {
    float r;
    float g;
    float b;
    float a;
} color_t;

typedef struct {
	float m00;
	float m01;
	float m02;
	float m03;
	
	float m10;
	float m11;
	float m12;
	float m13;
	
	float m20;
	float m21;
	float m22;
	float m23;
	
	float m30;
	float m31;
	float m32;
	float m33;
} matrix4f_t;

typedef struct {
	vector3f_t origin;
	vector3f_t direction;
} ray_t;



#ifdef __cplusplus
}
#endif
#endif
