
#include <model/Model.h>

#ifndef _Included_MD2SPEC_H
#define _Included_MD2SPEC_H
#ifdef __cplusplus
extern "C" {
#endif

#define MD2_MAGIC					844121161
#define MD2_VERSION					8

#define HEADER_LEN					68
#define SKIN_NAME_LEN				64
#define FRAME_NAME_LEN				16

#define NUBVER_NORMALS				162


//
// types
//

typedef struct {
    int		magic;
    int		version;
	int		skinWidth;
	int		skinHeight;
	int		frameSize;
	int		numSkins;
	int		numVertices;
	int		numTexCoords;
	int		numTriangles;
	int		numGlCommands;
	int		numFrames;
	int		offSkins;
	int		offTexCoords;
	int		offTriangles;
	int		offFrames;
	int		offGlCommands;
	int		offEnd;
} md2_header_t;

typedef struct {
	char	name[SKIN_NAME_LEN+1];
} md2_skin_t;

typedef struct {
	float		s;
	float		t;
} md2_texcoord_t;

typedef struct {
    vector3f_t	position;
    vector3f_t	normal;
} md2_vertex_t;

typedef struct {
	int			aVertexIndices[TRIANGLE_VERTEX_NUM];
	int			aTexCoordIndices[TRIANGLE_VERTEX_NUM];
} md2_triangle_t;

typedef struct {
	vector3f_t*		pVertex;
	vector3f_t*		pNormal;
	vector3f_t		vMin;
	vector3f_t		vMax;
} md2_mesh_t;

typedef struct {
    char            name[FRAME_NAME_LEN+1];
    md2_mesh_t		mesh;
} md2_frame_t;

typedef struct {
	
	md2_header_t 		header;
	
	md2_skin_t*			pSkin;
	
	md2_texcoord_t*		pTexCoord;
	
	md2_triangle_t*		pTriangle;
	
	md2_frame_t*		pFrame;
	
    float*				pBufTextureCoords;
    float*				pBufShadowColor;
    
    vector3f_t*			pBufVertex;
    vector3f_t*			pBufNormal;
    
    float				width;
    float				height;
    float				depth;
    
    bool				needScale;
    float				scale;
    float				screenWidth;
    float				screenHeight;
    
	int					dirtFlag;
	bool				initBoundingBox;
	float				currentFrame;
	
	vector3f_t			vMin;
	vector3f_t			vMax;
	vector3f_t			vCenter;
	float				radius;
	
	int					keyVertex;
	
	bool				regionInit;
	int					regionVertex[REGION_VERTEX_NUM];
	
	float				shadowHeight;
	float				shadowColor;
	float				shadowAlpha;
	
} md2_model_t;

#ifdef __cplusplus
}
#endif
#endif
