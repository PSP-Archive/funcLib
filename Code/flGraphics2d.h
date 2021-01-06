// Funclib Graphics2d v1.0.0
// 
// This module contains functions for dealing with simple 2d graphics.
// Use it to draw in 2d games, or for huds/menus in 3d games.
// This module requires GRAPHICS to be compiled in, some functions only work
// when TEXTURES is also compiled in.
// 
// Contributor(s): Flatmush



#ifndef FLGRAPHICS2D_H
#define FLGRAPHICS2D_H

#include "flGlobal.h"

#if FL_GRAPHICS_2D != 0

#if FL_TEXTURE != 0
#include "flTexture.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void graphics2dDrawLineGrad(int inSourceX, int inSourceY, u32 inSourceColor, int inTargetX, int inTargetY, u32 inTargetColor);
extern void graphics2dDrawTriangleGrad(int inX0, int inY0, u32 inColor0, int inX1, int inY1, u32 inColor1, int inX2, int inY2, u32 inColor2);
extern void graphics2dDrawTriangleFilledGrad(int inX0, int inY0, u32 inColor0, int inX1, int inY1, u32 inColor1, int inX2, int inY2, u32 inColor2);
extern void graphics2dDrawQuadGrad(int inX0, int inY0, u32 inColor0, int inX1, int inY1, u32 inColor1, int inX2, int inY2, u32 inColor2, int inX3, int inY3, u32 inColor3);
extern void graphics2dDrawQuadFilledGrad(int inX0, int inY0, u32 inColor0, int inX1, int inY1, u32 inColor1, int inX2, int inY2, u32 inColor2, int inX3, int inY3, u32 inColor3);
extern void graphics2dDrawEllipseFilledGrad(int inX, int inY, int inWidth, int inHeight, u32 inInnerColor, u32 inOuterColor);

#define graphics2dDrawLineGradStruct(inLine, inSourceColor, inTargetColor)                 graphics2dDrawLineGrad(inLine.linePoints[0].x, inLine.linePoints[0].y, inSourceColor, inLine.linePoints[1].x, inLine.linePoints[1].y, inTargetColor)
#define graphics2dDrawTriangleGradStruct(inTriangle, inColor0, inColor1, inColor2)         graphics2dDrawTriangleGrad(inTriangle.triPoints[0].x, inTriangle.triPoints[0].y, inColor0, inTriangle.triPoints[1].x, inTriangle.triPoints[1].y, inColor1, inTriangle.triPoints[2].x, inTriangle.triPoints[2].y, inColor2)
#define graphics2dDrawTriangleFilledGradStruct(inTriangle, inColor0, inColor1, inColor2)   graphics2dDrawTriangleFilledGrad(inTriangle.triPoints[0].x, inTriangle.triPoints[0].y, inColor0, inTriangle.triPoints[1].x, inTriangle.triPoints[1].y, inColor1, inTriangle.triPoints[2].x, inTriangle.triPoints[2].y, inColor2)
#define graphics2dDrawQuadGradStruct(inQuad, inColor0, inColor1, inColor2, inColor3)       graphics2dDrawQuadGrad(inQuad.quadPoints[0].x, inQuad.quadPoints[0].y, inColor0, inQuad.quadPoints[1].x, inQuad.quadPoints[1].y, inColor1, inQuad.quadPoints[2].x, inQuad.quadPoints[2].y, inColor2, inQuad.quadPoints[3].x, inQuad.quadPoints[3].y, inColor3)
#define graphics2dDrawQuadDilledGradStruct(inQuad, inColor0, inColor1, inColor2, inColor3) graphics2dDrawQuadFiledGrad(inQuad.quadPoints[0].x, inQuad.quadPoints[0].y, inColor0, inQuad.quadPoints[1].x, inQuad.quadPoints[1].y, inColor1, inQuad.quadPoints[2].x, inQuad.quadPoints[2].y, inColor2, inQuad.quadPoints[3].x, inQuad.quadPoints[3].y, inColor3)
#define graphics2dDrawEllipseFilledGradStruct(inEllipse, inInnerColor, inOuterColor)       graphics2dDrawEllipseFilledGrad(inEllipse.ellipseCenter.x, inEllipse.ellipseCenter.y, inEllipse.ellipseSize.x, inEllipse.ellipseSize.yx, inInnerColor, inOuterColor)

extern void graphics2dDrawPoint(int inX, int inY, u32 inColor);
extern void graphics2dDrawLine(int inSourceX, int inSourceY, int inTargetX, int inTargetY, u32 inColor);
extern void graphics2dDrawTriangle(int inX0, int inY0, int inX1, int inY1, int inX2, int inY2, u32 inColor);
extern void graphics2dDrawTriangleFilled(int inX0, int inY0, int inX1, int inY1, int inX2, int inY2, u32 inColor);
extern void graphics2dDrawQuad(int inX0, int inY0, int inX1, int inY1, int inX2, int inY2, int inX3, int inY3, u32 inColor);
extern void graphics2dDrawQuadFilled(int inX0, int inY0, int inX1, int inY1, int inX2, int inY2, int inX3, int inY3, u32 inColor);
extern void graphics2dDrawRect(int inX, int inY, int inWidth, int inHeight, u32 inColor);
extern void graphics2dDrawRectFilled(int inX, int inY, int inWidth, int inHeight, u32 inColor);
extern void graphics2dDrawEllipse(int inX, int inY, int inWidth, int inHeight, u32 inColor);
extern void graphics2dDrawEllipseFilled(int inX, int inY, int inWidth, int inHeight, u32 inColor);

#if (FL_COLOR && FL_MATH)
extern void graphics2dDrawSpheroidShaded(int inX, int inY, int inWidth, int inHeight, u32 inColor, float inShadeAngle);//, bool inHShading);
#endif

#if FL_TEXTURE
extern void graphics2dDrawTexture(int inX, int inY, int inWidth, int inHeight, Texture* inTex);
extern void graphics2dDrawTextureSection(int inX, int inY, int inWidth, int inHeight, Texture* inTex, unsigned int inSrcX, unsigned int inSrcY, unsigned int inSrcWidth, unsigned int inSrcHeight);
#endif
     
#ifdef __cplusplus
}
#endif

#endif

#endif
