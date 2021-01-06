#include "flGlobal.h"
#if FL_GRAPHICS_2D != 0
#include <pspgu.h>
#include <pspgum.h>

#if FL_MATH == 0
#include <math.h>
#endif

#if FL_INCLUDE_ALL_C == 0
#include "flGraphics2d.h"
#include "flGraphics.h"
#include "flMemory.h"

#if FL_COLOR != 0
#include "flColor.h"
#endif

#if FL_MATH != 0
#include "flMath.h"
#endif
#endif

void graphics2dDrawLineGrad(int inSourceX, int inSourceY, u32 inSourceColor, int inTargetX, int inTargetY, u32 inTargetColor) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);

     vertCVs* tempVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) << 1);

     tempVerts[0].vertColor = inSourceColor;
     tempVerts[0].vertX = inSourceX;
     tempVerts[0].vertY = inSourceY;
     tempVerts[0].vertZ = 0;

     tempVerts[1].vertColor = inTargetColor;
     tempVerts[1].vertX = inTargetX;
     tempVerts[1].vertY = inTargetY;
     tempVerts[1].vertZ = 0;

     sceGuDrawArray(GU_LINES, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);
}

void graphics2dDrawTriangleGrad(int inX0, int inY0, u32 inColor0, int inX1, int inY1, u32 inColor1, int inX2, int inY2, u32 inColor2) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);

     vertCVs* tempVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) << 2);

     tempVerts[0].vertColor = inColor0;
     tempVerts[0].vertX = inX0;
     tempVerts[0].vertY = inY0;
     tempVerts[0].vertZ = 0;

     tempVerts[1].vertColor = inColor1;
     tempVerts[1].vertX = inX1;
     tempVerts[1].vertY = inY1;
     tempVerts[1].vertZ = 0;

     tempVerts[2].vertColor = inColor2;
     tempVerts[2].vertX = inX2;
     tempVerts[2].vertY = inY2;
     tempVerts[2].vertZ = 0;
     
     tempVerts[3].vertColor = inColor0;
     tempVerts[3].vertX = inX0;
     tempVerts[3].vertY = inY0;
     tempVerts[3].vertZ = 0;

     sceGuDrawArray(GU_LINE_STRIP, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 4, 0, tempVerts);
     
     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);
}

void graphics2dDrawTriangleFilledGrad(int inX0, int inY0, u32 inColor0, int inX1, int inY1, u32 inColor1, int inX2, int inY2, u32 inColor2) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);
     sceGuDisable(GU_CULL_FACE);

     vertCVs* tempVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) * 3);

     tempVerts[0].vertColor = inColor0;
     tempVerts[0].vertX = inX0;
     tempVerts[0].vertY = inY0;
     tempVerts[0].vertZ = 0;

     tempVerts[1].vertColor = inColor1;
     tempVerts[1].vertX = inX1;
     tempVerts[1].vertY = inY1;
     tempVerts[1].vertZ = 0;

     tempVerts[2].vertColor = inColor2;
     tempVerts[2].vertX = inX2;
     tempVerts[2].vertY = inY2;
     tempVerts[2].vertZ = 0;

     sceGuDrawArray(GU_TRIANGLES, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 3, 0, tempVerts);
     
     sceGuEnable(GU_CULL_FACE);
     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);
}

void graphics2dDrawQuadGrad(int inX0, int inY0, u32 inColor0, int inX1, int inY1, u32 inColor1, int inX2, int inY2, u32 inColor2, int inX3, int inY3, u32 inColor3) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);

     vertCVs* tempVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) * 5);

     tempVerts[0].vertColor = inColor0;
     tempVerts[0].vertX = inX0;
     tempVerts[0].vertY = inY0;
     tempVerts[0].vertZ = 0;

     tempVerts[1].vertColor = inColor1;
     tempVerts[1].vertX = inX1;
     tempVerts[1].vertY = inY1;
     tempVerts[1].vertZ = 0;

     tempVerts[2].vertColor = inColor2;
     tempVerts[2].vertX = inX2;
     tempVerts[2].vertY = inY2;
     tempVerts[2].vertZ = 0;
     
     tempVerts[3].vertColor = inColor3;
     tempVerts[3].vertX = inX3;
     tempVerts[3].vertY = inY3;
     tempVerts[3].vertZ = 0;
     
     tempVerts[4].vertColor = inColor0;
     tempVerts[4].vertX = inX0;
     tempVerts[4].vertY = inY0;
     tempVerts[4].vertZ = 0;

     sceGuDrawArray(GU_LINE_STRIP, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 5, 0, tempVerts);
     
     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);
}

void graphics2dDrawQuadFilledGrad(int inX0, int inY0, u32 inColor0, int inX1, int inY1, u32 inColor1, int inX2, int inY2, u32 inColor2, int inX3, int inY3, u32 inColor3) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);
     sceGuDisable(GU_CULL_FACE);

     vertCVs* tempVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) << 2);

     tempVerts[0].vertColor = inColor0;
     tempVerts[0].vertX = inX0;
     tempVerts[0].vertY = inY0;
     tempVerts[0].vertZ = 0;

     tempVerts[1].vertColor = inColor1;
     tempVerts[1].vertX = inX1;
     tempVerts[1].vertY = inY1;
     tempVerts[1].vertZ = 0;

     tempVerts[2].vertColor = inColor2;
     tempVerts[2].vertX = inX2;
     tempVerts[2].vertY = inY2;
     tempVerts[2].vertZ = 0;
     
     tempVerts[3].vertColor = inColor3;
     tempVerts[3].vertX = inX3;
     tempVerts[3].vertY = inY3;
     tempVerts[3].vertZ = 0;

     sceGuDrawArray(GU_TRIANGLE_FAN, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 4, 0, tempVerts);
     
     sceGuEnable(GU_CULL_FACE);
     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);
}

void graphics2dDrawEllipseFilledGrad(int inX, int inY, int inWidth, int inHeight, u32 inInnerColor, u32 inOuterColor) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);
     sceGuDisable(GU_CULL_FACE);

     inWidth = pos(inWidth);
     inHeight = pos(inHeight);
     
     float tempAvgRad = sqrtf((float)((inWidth * inWidth) + (inHeight * inHeight)));
     u32 tempPoints = max(FL_GRAPHICS_2D_ELLIPSE_MINLINES, (int)(tempAvgRad * FL_GRAPHICS_2D_ELLIPSE_EDGESPERRAD));

     vertCVs* tempVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) * (tempPoints + 2));

     inWidth >>= 1;
     inHeight >>= 1;
     inX += inWidth;
     inY += inHeight;

     tempVerts[0].vertColor = inInnerColor;
     tempVerts[0].vertX = inX;
     tempVerts[0].vertY = inY;
     tempVerts[0].vertZ = 0;

     u32 i;
     float tempAngle = 0;
     for(i = 0; i <= tempPoints; i++) {
          tempAngle = i * ((2 * MATH_PI) / tempPoints);
          tempVerts[i + 1].vertColor = inOuterColor;
          #if FL_MATH != 0
          tempVerts[i + 1].vertX = inX + (inWidth * mathSinf(tempAngle));
          tempVerts[i + 1].vertY = inY + (inHeight * mathCosf(tempAngle));
          #else
          tempVerts[i + 1].vertX = inX + (inWidth * sinf(tempAngle));
          tempVerts[i + 1].vertY = inY + (inHeight * cosf(tempAngle));
          #endif
          tempVerts[i + 1].vertZ = 0;
     }

     sceGuDrawArray(GU_TRIANGLE_FAN, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, (tempPoints + 2), 0, tempVerts);

     sceGuEnable(GU_CULL_FACE);
     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);
}











void graphics2dDrawPoint(int inX, int inY, u32 inColor) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);

     sceGuColor(inColor);

     vertVs* tempVerts = (vertVs*)sceGuGetMemory(sizeof(vertVs));

     tempVerts[0].vertX = inX;
     tempVerts[0].vertY = inY;
     tempVerts[0].vertZ = 0;

     sceGuDrawArray(GU_POINTS, GU_VERTEX_16BIT | GU_TRANSFORM_2D, 1, 0, tempVerts);
     
     sceGuColor(0xFFFFFFFF);

     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);
}

void graphics2dDrawLine(int inSourceX, int inSourceY, int inTargetX, int inTargetY, u32 inColor) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);

     sceGuColor(inColor);

     vertVs* tempVerts = (vertVs*)sceGuGetMemory(sizeof(vertVs) << 1);

     tempVerts[0].vertX = inSourceX;
     tempVerts[0].vertY = inSourceY;
     tempVerts[0].vertZ = 0;

     tempVerts[1].vertX = inTargetX;
     tempVerts[1].vertY = inTargetY;
     tempVerts[1].vertZ = 0;

     sceGuDrawArray(GU_LINES, GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

     sceGuColor(0xFFFFFFFF);

     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);
}

void graphics2dDrawTriangle(int inX0, int inY0, int inX1, int inY1, int inX2, int inY2, u32 inColor) {
     graphics2dDrawTriangleGrad(inX0, inY0, inColor, inX1, inY1, inColor, inX2, inY2, inColor);
}

void graphics2dDrawTriangleFilled(int inX0, int inY0, int inX1, int inY1, int inX2, int inY2, u32 inColor) {
     graphics2dDrawTriangleFilledGrad(inX0, inY0, inColor, inX1, inY1, inColor, inX2, inY2, inColor);
}

void graphics2dDrawQuad(int inX0, int inY0, int inX1, int inY1, int inX2, int inY2, int inX3, int inY3, u32 inColor) {
     graphics2dDrawQuadGrad(inX0, inY0, inColor, inX1, inY1, inColor, inX2, inY2, inColor, inX3, inY3, inColor);
}

void graphics2dDrawQuadFilled(int inX0, int inY0, int inX1, int inY1, int inX2, int inY2, int inX3, int inY3, u32 inColor) {
     graphics2dDrawQuadFilledGrad(inX0, inY0, inColor, inX1, inY1, inColor, inX2, inY2, inColor, inX3, inY3, inColor);
}

void graphics2dDrawRect(int inX, int inY, int inWidth, int inHeight, u32 inColor) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);

     vertCVs* tempVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) * 6);
     
     
     tempVerts[0].vertColor = inColor;
     tempVerts[0].vertX = inX;
     tempVerts[0].vertY = inY;
     tempVerts[0].vertZ = 0;

     tempVerts[1].vertColor = inColor;
     tempVerts[1].vertX = inX + inWidth - 1;
     tempVerts[1].vertY = inY;
     tempVerts[1].vertZ = 0;
     
     tempVerts[2].vertColor = inColor;
     tempVerts[2].vertX = inX + inWidth - 1;
     tempVerts[2].vertY = inY + inHeight;
     tempVerts[2].vertZ = 0;
     
     tempVerts[3].vertColor = inColor;
     tempVerts[3].vertX = inX + inWidth - 1;
     tempVerts[3].vertY = inY + inHeight - 1;
     tempVerts[3].vertZ = 0;
     
     tempVerts[4].vertColor = inColor;
     tempVerts[4].vertX = inX;
     tempVerts[4].vertY = inY + inHeight - 1;
     tempVerts[4].vertZ = 0;
     
     tempVerts[5].vertColor = inColor;
     tempVerts[5].vertX = inX;
     tempVerts[5].vertY = inY;
     tempVerts[5].vertZ = 0;

     sceGuDrawArray(GU_LINE_STRIP, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 3, 0, tempVerts);
     sceGuDrawArray(GU_LINE_STRIP, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 3, 0, &tempVerts[3]);

     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);
}

void graphics2dDrawRectFilled(int inX, int inY, int inWidth, int inHeight, u32 inColor) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);

     vertCVs* tempVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) << 1);

     tempVerts[0].vertColor = inColor;
     tempVerts[0].vertX = inX;
     tempVerts[0].vertY = inY;
     tempVerts[0].vertZ = 0;

     tempVerts[1].vertColor = inColor;
     tempVerts[1].vertX = inX + inWidth;
     tempVerts[1].vertY = inY + inHeight;
     tempVerts[1].vertZ = 0;

     sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);
}

void graphics2dDrawEllipse(int inX, int inY, int inWidth, int inHeight, u32 inColor) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);

     inWidth = pos(inWidth);
     inHeight = pos(inHeight);

     float tempAvgRad = sqrtf((float)((inWidth * inWidth) + (inHeight * inHeight)));
     u32 tempPoints = max(FL_GRAPHICS_2D_ELLIPSE_MINLINES, (int)(tempAvgRad * FL_GRAPHICS_2D_ELLIPSE_EDGESPERRAD));

     vertCVs* tempVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) * (tempPoints + 1));

     inWidth >>= 1;
     inHeight >>= 1;
     inX += inWidth;
     inY += inHeight;

     u32 i;
     float tempAngle = 0;
     for(i = 0; i <= tempPoints; i++) {
          tempAngle = i * ((2 * MATH_PI) / tempPoints);
          tempVerts[i].vertColor = inColor;
          #if FL_MATH != 0
          tempVerts[i].vertX = inX + (inWidth * mathSinf(tempAngle));
          tempVerts[i].vertY = inY + (inHeight * mathCosf(tempAngle));
          #else
          tempVerts[i].vertX = inX + (inWidth * sinf(tempAngle));
          tempVerts[i].vertY = inY + (inHeight * cosf(tempAngle));
          #endif
          tempVerts[i].vertZ = 0;
     }

     sceGuDrawArray(GU_LINE_STRIP, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, (tempPoints + 1), 0, tempVerts);

     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);
}

void graphics2dDrawEllipseFilled(int inX, int inY, int inWidth, int inHeight, u32 inColor) {
     graphics2dDrawEllipseFilledGrad(inX, inY, inWidth, inHeight, inColor, inColor);
}





#if FL_COLOR != 0
void graphics2dDrawSpheroidShaded(int inX, int inY, int inWidth, int inHeight, u32 inColor, float inShadeAngle) { //bool inHShading) {
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_TEXTURE_2D);
     sceGuDisable(GU_CULL_FACE);

     inWidth = pos(inWidth);
     inHeight = pos(inHeight);
     float tempAvgRad = sqrtf((float)((inWidth * inWidth) + (inHeight * inHeight)));
     u32 tempPoints = max(FL_GRAPHICS_2D_ELLIPSE_MINLINES, (int)(tempAvgRad * FL_GRAPHICS_2D_ELLIPSE_EDGESPERRAD));

     vertCVs* tempVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) * (tempPoints + 2));

     inWidth >>= 1;
     inHeight >>= 1;
     inX += inWidth;
     inY += inHeight;

     tempVerts[0].vertColor = inColor;
     tempVerts[0].vertX = inX;
     tempVerts[0].vertY = inY;
     tempVerts[0].vertZ = 0;

     u32 i;
     float tempAngle = 0.0f;;
     float tempFract = 0.0f;
     for(i = 0; i <= tempPoints; i++) {
          tempAngle = i * ((2 * MATH_PI) / tempPoints);
          
          #if FL_MATH != 0
          tempVerts[i + 1].vertX = inX + (inWidth * mathSinf(tempAngle));
          tempVerts[i + 1].vertY = inY + (inHeight * mathCosf(tempAngle));
          tempFract = (tempAngle - inShadeAngle);
          tempFract = 1.0f + (0.0f - mathCosf(tempFract));
          #else
          tempVerts[i + 1].vertX = inX + (inWidth * sinf(tempAngle));
          tempVerts[i + 1].vertY = inY + (inHeight * cosf(tempAngle));
          tempFract = (tempAngle - inShadeAngle);
          tempFract = 1.0f + (0.0f - cosf(tempFract));
          #endif
          
          tempVerts[i + 1].vertColor = colorLumMult8888(inColor, tempFract);
          //if(inHShading)
          //     tempVerts[i + 1].vertColor = colorLumMult8888(inColor, ((float)((tempVerts[i + 1].vertX - inX) + inWidth) / (float)(inWidth << 1)));
          //else
          //     tempVerts[i + 1].vertColor = colorLumMult8888(inColor, ((float)((tempVerts[i + 1].vertY - inY) + inHeight) / (float)(inHeight << 1)));
          tempVerts[i + 1].vertZ = 0;
     }

     sceGuDrawArray(GU_TRIANGLE_FAN, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, (tempPoints + 2), 0, tempVerts);

     sceGuEnable(GU_CULL_FACE);
     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_DEPTH_TEST);     
}
#endif









#if FL_TEXTURE != 0
void graphics2dDrawTexture(int inX, int inY, int inWidth, int inHeight, Texture* inTex) {
     if(!inTex) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to draw NULL texture.");
          #endif
          return;
     }

     #if (FL_GRAPHICS_2D_USEDMA != 0) && (FL_TEXTURE_PRESERVENONALPHA != 0)
     if(!inTex->texAlpha && (inTex->texPixelFormat == GU_PSM_8888)) {
          if((inWidth == inTex->texWidth) && (inHeight == inTex->texHeight)) {
               #if FL_MEMORY != 0
               sceGuCopyImage(GU_PSM_8888, 0, 0, inTex->texDataWidth, inTex->texHeight, inTex->texStride, inTex->texData, inX, inY, SCREEN_PITCH, vmemAbsolutePtr(graphicsDrawBuffer));
               #else
               sceGuCopyImage(GU_PSM_8888, 0, 0, inTex->texDataWidth, inTex->texHeight, inTex->texStride, inTex->texData, inX, inY, SCREEN_PITCH, (void*)((unsigned int)graphicsDrawBuffer | 0x04000000));
               #endif
               return;
          }
     }
     #endif

     sceGuDisable(GU_DEPTH_TEST);

     texBind(inTex);
     sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
     
     if(inWidth == inTex->texWidth) {
          if(inHeight == inTex->texHeight)
               sceGuTexFilter(GU_NEAREST, GU_NEAREST);
          // The slicewidth calculation was taken from some benchmarks on ps2dev, maybe possible make a better calculation.
          int tempSliceWidth = (inTex->texPixelFormat == GU_PSM_T4 ? 32 : 64);
          int i;
          for(i = 0; i < inTex->texWidth; i += tempSliceWidth) {
               if((i + tempSliceWidth) > inTex->texWidth)
                    tempSliceWidth = (inTex->texWidth - i);

               vertTsVs* tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);

               tempVerts[0].vertX = inX + i;
               tempVerts[0].vertY = inY;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = i;
               tempVerts[0].vertV = 0;

               tempVerts[1].vertX = inX + i + tempSliceWidth;
               tempVerts[1].vertY = inY + inHeight;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = i + tempSliceWidth;
               tempVerts[1].vertV = inTex->texHeight;
     
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
          }
     } else {
          sceGuTexFilter(GU_LINEAR, GU_LINEAR);

          vertTsVs* tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);

          tempVerts[0].vertX = inX;
          tempVerts[0].vertY = inY;
          tempVerts[0].vertZ = 0;
          tempVerts[0].vertU = 0;
          tempVerts[0].vertV = 0;

          tempVerts[1].vertX = inX + inWidth;
          tempVerts[1].vertY = inY + inHeight;
          tempVerts[1].vertZ = 0;
          tempVerts[1].vertU = inTex->texWidth;
          tempVerts[1].vertV = inTex->texHeight;
     
          sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
     }

     sceGuEnable(GU_DEPTH_TEST);
}

void graphics2dDrawTextureSection(int inX, int inY, int inWidth, int inHeight, Texture* inTex, unsigned int inSrcX, unsigned int inSrcY, unsigned int inSrcWidth, unsigned int inSrcHeight) {
     #if (FL_GRAPHICS_2D_USEDMA != 0) && (FL_TEXTURE_PRESERVENONALPHA != 0)
     if(!inTex->texAlpha && (inTex->texPixelFormat == GU_PSM_8888)) {
          if((inWidth == inSrcWidth) && (inHeight == inSrcHeight)) {
               #if FL_MEMORY != 0
               sceGuCopyImage(GU_PSM_8888, inSrcX, inSrcY, inSrcWidth, inSrcHeight, inTex->texStride, inTex->texData, inX, inY, SCREEN_PITCH, vmemAbsolutePtr(graphicsDrawBuffer));
               #else
               sceGuCopyImage(GU_PSM_8888, inSrcX, inSrcY, inSrcWidth, inSrcHeight, inTex->texStride, inTex->texData, inX, inY, SCREEN_PITCH, (void*)((unsigned int)graphicsDrawBuffer | 0x04000000));
               #endif
               return;
          }
     }
     #endif

     sceGuDisable(GU_DEPTH_TEST);

     texBind(inTex);
     sceGuTexEnvColor(0xFFFFFFFF);
     sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
     if((inWidth == inSrcWidth) && (inHeight == inSrcHeight))
          sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     else
          sceGuTexFilter(GU_LINEAR, GU_LINEAR);
    
     vertTsVs* tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);

     tempVerts[0].vertX = inX;
     tempVerts[0].vertY = inY;
     tempVerts[0].vertZ = 0;
     tempVerts[0].vertU = inSrcX;
     tempVerts[0].vertV = inSrcY;

     tempVerts[1].vertX = inX + inWidth;
     tempVerts[1].vertY = inY + inHeight;
     tempVerts[1].vertZ = 0;
     tempVerts[1].vertU = inSrcX + inSrcWidth;
     tempVerts[1].vertV = inSrcY + inSrcHeight;

     sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

     sceGuEnable(GU_DEPTH_TEST);
}
#endif

#endif
