// Funclib Model v1.0.0
// 
// This module contains functions for loading/rendering models.
// Use it in 3d games and 2d games where complex shapes are used.
// This module requires GRAPHICS and TEXTURE.
// This module works better when used in conjunction with MATH.
// 
// Contributor(s): Flatmush



#ifndef FLMODEL_H
#define FLMODEL_H

#include "flGlobal.h"
#include "flTexture.h"

#if FL_MODEL != 0

#ifdef __cplusplus
extern "C" {
#endif

#define MODEL_ANIM_TYPE_NONE 0
#define MODEL_ANIM_TYPE_MD2  1
#define MODEL_ANIM_TYPE_MD3  2

typedef struct {
     u8       mdlfVertCount;
     vect3f** mdlfVerts;
     Texture* mdlfTexture;
     vect2f** mdlfTexVerts;
     vect3f** mdlfNormals;
} Model3dStaticFace;
typedef Model3dStaticFace Mdl3dStatFace;

typedef struct {
     u32                mdlVertCount;
     u8                 mdlTextureCount;
     u32                mdlTexVertCount;
     u32                mdlNormalCount;
     u32                mdlFaceCount;
     vect3f*            mdlVerts;
     vect2f*            mdlTexVerts;
     vect3f*            mdlNormals;
     Texture**          mdlTextures;
     Model3dStaticFace* mdlFaces;
     bool               mdlOptimized;
} Model3dStatic;
typedef Model3dStatic Mdl3dStat;

typedef struct {
     u8    mdlType;
     void* mdlData;
} Model3dAnimated;
typedef Model3dAnimated Mdl3dAnim;

extern void             mdlInit();

extern Model3dStatic*   mdl3dStatLoad(char* inPath);
extern bool             mdl3dStatSave(char* inPath, Model3dStatic* inModel);
extern void             mdl3dStatDraw(Model3dStatic* inModel);
extern void             mdl3dStatOptimize(Model3dStatic* inModel);
extern void             mdl3dStatFree(Model3dStatic* inModel);

extern Model3dAnimated* mdl3dAnimLoad(char* inPath);
extern bool             mdl3dAnimSave(char* inPath, Model3dAnimated* inModel);
extern void             mdl3dAnimDraw(Model3dAnimated* inModel);
extern void             mdl3dAnimFree(Model3dAnimated* inModel);

#ifdef __cplusplus
}
#endif

#endif

#endif
