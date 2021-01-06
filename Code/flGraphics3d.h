// Funclib Graphics3d v1.0.0
// 
// This module contains functions for dealing with simple 3d primatives.
// Use it to draw simple 3d shapes and for particles/effects.
// This module requires GRAPHICS to be compiled in, some functions only work
// when TEXTURES is also compiled in.
// 
// Contributor(s): Flatmush



#ifndef FLGRAPHICS3D_H
#define FLGRAPHICS3D_H

#include "flGlobal.h"

#if FL_GRAPHICS_3D != 0

#define SPHERE_MAP_CYLINDER_EQUAL 0
#define SPHERE_MAP_CYLINDER       1

#if FL_TEXTURE != 0
#include "flTexture.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void graphics3dDrawOrigin();
extern void graphics3dDrawCubeWireframe();
extern void graphics3dDrawCube();
extern void graphics3dDrawSphere(u32 inDetail);
extern void graphics3dDrawSphereWireframe(u32 inDetail);

extern void graphics3dDrawCubeWireframea(vect3f inLocation, vect3f inDimensions, vect3f inRotation, u32 inColor);
extern void graphics3dDrawCubea(vect3f inLocation, vect3f inDimensions, vect3f inRotation, u32 inColor);

#if FL_TEXTURE != 0
extern void graphics3dDrawCubeTextured(Texture* inTex);
extern void graphics3dDrawSphereTextured(u32 inDetail, u8 inMapType, Texture* inTex);
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
