//
//                MilkShape 3D 1.4.0 File Format Specification
//
//                  This specifcation is written in C style.
//
// The data structures are defined in the order as they appear in the .ms3d file.
//

//
// max values
//

#include <model/Model.h>

#ifndef _Included_MS3DSPEC_H
#define _Included_MS3DSPEC_H
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_VERTICES    8192
#define MAX_TRIANGLES   16384
#define MAX_GROUPS      128
#define MAX_MATERIALS   128
#define MAX_JOINTS      128
#define MAX_KEYFRAMES   216     // increase when needed

#define MS3D_VERSION_MIN   3 
#define MS3D_VERSION_MAX   4

#define MS3D_SUBVERSION_ONE		1
#define MS3D_SUBVERSION_TWO		2

#define MS3D_SUB_JOINT_NUM		3
#define MS3D_SUB_JOINT_FIRST	0
#define MS3D_SUB_JOINT_SECOND	1
#define MS3D_SUB_JOINT_THIRD	2

#define SKELETON_DATA_NUM		4
#define SKELETON_INDEX_ONE			0
#define SKELETON_INDEX_TWO			1
#define SKELETON_INDEX_THREE		2
#define SKELETON_INDEX_FOUR			3

#define MS3D_KEY_FRAME_MIN		2

#define MS3D_PARENT_JOINT_INDEX_INVALID		-1

#define MS3D_MATERIAL_INDEX_INVALID			-1

#define MS3D_SKELETON_100_RATE				100.0f


//
// flags
//
#define SELECTED        1
#define HIDDEN          2
#define SELECTED2       4
#define DIRTY           8

#define HEADER_NAME_LEN				10
#define GROUP_NAME_LEN				32
#define MATERIAL_NAME_LEN			32
#define MATERIAL_TEXTURE_LEN		128
#define MATERIAL_ALPHAMAP_LEN		128
#define JOINT_NAME_LEN				32

//
// types
//

typedef struct texture_info {
	char					name[MATERIAL_TEXTURE_LEN+1];
	struct texture_info*	pnext;
} texture_info_t;

typedef struct {
    char    id[HEADER_NAME_LEN+1];                                     // always "MS3D000000"
    int     version;                                    // 3 or 4
} ms3d_header_t;

typedef struct {
    byte    	flags;                                      // SELECTED | SELECTED2 | HIDDEN
    vector3f_t	vertex;                                  //
    char		boneId;                                     // -1 = no bone
    byte		referenceCount;
    
    byte		boneIds[MS3D_SUB_JOINT_NUM];
    byte		weights[MS3D_SUB_JOINT_NUM];	
    int			extra;	
} ms3d_vertex_t;

typedef struct {
    word    	flags;                                      // SELECTED | SELECTED2 | HIDDEN
    word    	vertexIndices[TRIANGLE_VERTEX_NUM];                           //
    vector3f_t	vertexNormals[TRIANGLE_VERTEX_NUM];                        //
    float		S[TRIANGLE_VERTEX_NUM];
    float   	T[TRIANGLE_VERTEX_NUM];                                       //
    byte    	smoothingGroup;                             // 1 - 32
    byte    	groupIndex;                                 //
} ms3d_triangle_t;

typedef struct {
    byte            flags;                              // SELECTED | HIDDEN
    char            name[GROUP_NAME_LEN+1];                           //
    word            numtriangles;                       //
    word*           pTriangleIndices;      // the groups group the triangles
    char            materialIndex;                      // -1 = no material
    char*			comment;
    
    float*			pBufShadowVertex;
    float*			pBufTextureCoords;
    float*			pBufShadowColor;
} ms3d_group_t;

typedef struct {
    char            name[MATERIAL_NAME_LEN+1];                           //
    float           ambient[LIGHT_DATA_DIMENSION];                         //
    float           diffuse[LIGHT_DATA_DIMENSION];                         //
    float           specular[LIGHT_DATA_DIMENSION];                        //
    float           emissive[LIGHT_DATA_DIMENSION];                        //
    float           shininess;                          // 0.0f - 128.0f
    float           transparency;                       // 0.0f - 1.0f
    char            mode;                               // 0, 1, 2 is unused now
    char            texture[MATERIAL_TEXTURE_LEN+1];                       // texture.bmp
    char            alphamap[MATERIAL_ALPHAMAP_LEN+1];                      // alpha.bmp
    char*			comment;
} ms3d_material_t;

typedef struct {
    float           time;                               // time in seconds
    vector3f_t      key;                        // x, y, z angles
} ms3d_keyframe_t;

typedef struct {
	float tangentIn[MS3D_SUB_JOINT_NUM];
	float tangentOut[MS3D_SUB_JOINT_NUM];
} ms3d_tangent_t;

typedef struct {
    byte            	flags;                              // SELECTED | DIRTY
    char            	name[JOINT_NAME_LEN+1];                           //
    char            	parentName[JOINT_NAME_LEN+1];                     //
    vector3f_t      	rotation;                        // local reference matrix
    vector3f_t      	position;

    word            	numKeyFramesRot;                    //
    word            	numKeyFramesTrans;                  //

    ms3d_keyframe_t*	pKeyFramesRot;      // local animation matrices
    ms3d_keyframe_t*	pKeyFramesTrans;  // local animation matrices
    
    ms3d_tangent_t*		pTangent;
    
    int					parentIndex;
    char*				comment;
    float				color[COLOR_DATA_NUM];
    
	quat4f_t 			matLocalSkeleton[MS3D_SUB_JOINT_NUM];
	quat4f_t 			matGlobalSkeleton[MS3D_SUB_JOINT_NUM];

	quat4f_t			matLocal[MS3D_SUB_JOINT_NUM];
	quat4f_t			matGlobal[MS3D_SUB_JOINT_NUM];
} ms3d_joint_t;

typedef struct {
	
	ms3d_header_t 		header;
	
	word				nNumVertices;
	ms3d_vertex_t*		pVertex;
	
	word				nNumTriangles;
	ms3d_triangle_t*	pTriangle;
	
	word				nNumGroups;
	ms3d_group_t*		pGroup;
	
	word				nNumMaterials;
	ms3d_material_t*	pMaterial;
	
	float				fAnimationFPS;
	float				fCurrentTime;
	int					iTotalFrames;

	int					nNumJoints;
	ms3d_joint_t*		pJoint;
	
	char*				comment;
	float				jointSize;
	int					transparencyMode;
	float				alphaRef;
	
	texture_info_t*		pTextureList;
	
	vector3f_t*			pBufVertex;
	
	int					dirtFlag;
	bool				initBoundingBox;
	float				currentTime;
	vector3f_t			vMin;
	vector3f_t			vMax;
	vector3f_t			vCenter;
	float				radius;
	float				dynamicMinY;
	float				dynamicMaxY;

	float				baseShadowColor;
	float				baseShadowAlpha;
	
} ms3d_model_t;

#ifdef __cplusplus
}
#endif
#endif
