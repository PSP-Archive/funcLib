#include "flGlobal.h"
#if FL_GRAPHICS_3D != 0
#include <pspgu.h>
#include <pspgum.h>

#if !FL_INCLUDE_ALL_C
#include "flGraphics3d.h"
#include "flGraphics.h"
#include "flMemory.h"
#include "flColor.h"
#include "flMath.h"
#endif

void graphics3dDrawOrigin() {
     sceGuDisable(GU_TEXTURE_2D);

     sceGumPushMatrix();
     sceGumLoadIdentity();
     
     vertcVf* tempVerts = (vertcVf*)sceGuGetMemory(sizeof(vertcVf) * 7);
 
     tempVerts[0].vertX = -0.5f; tempVerts[0].vertY = 0.0f; tempVerts[0].vertZ = 0.0f; tempVerts[0].vertColor = 0xF003;
     tempVerts[1].vertX = +0.5f; tempVerts[1].vertY = 0.0f; tempVerts[1].vertZ = 0.0f; tempVerts[1].vertColor = 0xF77F;
     
     tempVerts[2].vertX = 0.0f; tempVerts[2].vertY = -0.5f; tempVerts[2].vertZ = 0.0f; tempVerts[2].vertColor = 0xF030;
     tempVerts[3].vertX = 0.0f; tempVerts[3].vertY = +0.5f; tempVerts[3].vertZ = 0.0f; tempVerts[3].vertColor = 0xF7F7;
     
     tempVerts[4].vertX = 0.0f; tempVerts[4].vertY = 0.0f; tempVerts[4].vertZ = -0.5f; tempVerts[4].vertColor = 0xF300;
     tempVerts[5].vertX = 0.0f; tempVerts[5].vertY = 0.0f; tempVerts[5].vertZ = +0.5f; tempVerts[5].vertColor = 0xFF77;

     sceGumDrawArray(GU_LINES, GU_COLOR_4444 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 6, 0, tempVerts);
     sceGumPopMatrix();
     sceGuEnable(GU_TEXTURE_2D);
}

void graphics3dDrawCubeWireframe() {
     sceGuDisable(GU_TEXTURE_2D);

     vertVf* tempVerts = (vertVf*)sceGuGetMemory(sizeof(vertVf) * 24);
 
     // Vertical struts
     tempVerts[0].vertX = -0.5f; tempVerts[0].vertY = -0.5f; tempVerts[0].vertZ = -0.5f;
     tempVerts[1].vertX = -0.5f; tempVerts[1].vertY = +0.5f; tempVerts[1].vertZ = -0.5f;
     tempVerts[2].vertX = +0.5f; tempVerts[2].vertY = -0.5f; tempVerts[2].vertZ = -0.5f;
     tempVerts[3].vertX = +0.5f; tempVerts[3].vertY = +0.5f; tempVerts[3].vertZ = -0.5f;
     tempVerts[4].vertX = -0.5f; tempVerts[4].vertY = -0.5f; tempVerts[4].vertZ = +0.5f;
     tempVerts[5].vertX = -0.5f; tempVerts[5].vertY = +0.5f; tempVerts[5].vertZ = +0.5f;
     tempVerts[6].vertX = +0.5f; tempVerts[6].vertY = -0.5f; tempVerts[6].vertZ = +0.5f;
     tempVerts[7].vertX = +0.5f; tempVerts[7].vertY = +0.5f; tempVerts[7].vertZ = +0.5f;
     
     // Horizontal struts
     tempVerts[8].vertX = -0.5f; tempVerts[8].vertY = -0.5f; tempVerts[8].vertZ = -0.5f;
     tempVerts[9].vertX = +0.5f; tempVerts[9].vertY = -0.5f; tempVerts[9].vertZ = -0.5f;
     tempVerts[10].vertX = -0.5f; tempVerts[10].vertY = +0.5f; tempVerts[10].vertZ = -0.5f;
     tempVerts[11].vertX = +0.5f; tempVerts[11].vertY = +0.5f; tempVerts[11].vertZ = -0.5f;
     tempVerts[12].vertX = -0.5f; tempVerts[12].vertY = -0.5f; tempVerts[12].vertZ = +0.5f;
     tempVerts[13].vertX = +0.5f; tempVerts[13].vertY = -0.5f; tempVerts[13].vertZ = +0.5f;
     tempVerts[14].vertX = -0.5f; tempVerts[14].vertY = +0.5f; tempVerts[14].vertZ = +0.5f;
     tempVerts[15].vertX = +0.5f; tempVerts[15].vertY = +0.5f; tempVerts[15].vertZ = +0.5f;
     
     // Depth struts
     tempVerts[16].vertX = -0.5f; tempVerts[16].vertY = -0.5f; tempVerts[16].vertZ = -0.5f;
     tempVerts[17].vertX = -0.5f; tempVerts[17].vertY = -0.5f; tempVerts[17].vertZ = +0.5f;
     tempVerts[18].vertX = +0.5f; tempVerts[18].vertY = -0.5f; tempVerts[18].vertZ = -0.5f;
     tempVerts[19].vertX = +0.5f; tempVerts[19].vertY = -0.5f; tempVerts[19].vertZ = +0.5f;
     tempVerts[20].vertX = -0.5f; tempVerts[20].vertY = +0.5f; tempVerts[20].vertZ = -0.5f;
     tempVerts[21].vertX = -0.5f; tempVerts[21].vertY = +0.5f; tempVerts[21].vertZ = +0.5f;
     tempVerts[22].vertX = +0.5f; tempVerts[22].vertY = +0.5f; tempVerts[22].vertZ = -0.5f;
     tempVerts[23].vertX = +0.5f; tempVerts[23].vertY = +0.5f; tempVerts[23].vertZ = +0.5f;
 
     sceGumDrawArray(GU_LINES, GU_VERTEX_32BITF | GU_TRANSFORM_3D, 24, 0, tempVerts);
     sceGuEnable(GU_TEXTURE_2D);
}

void graphics3dDrawCube() {
     if(graphicsWireframe) {
          graphics3dDrawCubeWireframe();
          return;
     }
     sceGuDisable(GU_TEXTURE_2D);

     vertVf* tempVerts = (vertVf*)sceGuGetMemory(sizeof(vertVf) * 18);

     // Surround
     tempVerts[0].vertX = -0.5f; tempVerts[0].vertY = +0.5f; tempVerts[0].vertZ = -0.5f;
     tempVerts[1].vertX = -0.5f; tempVerts[1].vertY = -0.5f; tempVerts[1].vertZ = -0.5f;
     
     tempVerts[2].vertX = +0.5f; tempVerts[2].vertY = +0.5f; tempVerts[2].vertZ = -0.5f;
     tempVerts[3].vertX = +0.5f; tempVerts[3].vertY = -0.5f; tempVerts[3].vertZ = -0.5f;
     
     tempVerts[4].vertX = +0.5f; tempVerts[4].vertY = +0.5f; tempVerts[4].vertZ = +0.5f;
     tempVerts[5].vertX = +0.5f; tempVerts[5].vertY = -0.5f; tempVerts[5].vertZ = +0.5f;

     tempVerts[6].vertX = -0.5f; tempVerts[6].vertY = +0.5f; tempVerts[6].vertZ = +0.5f;
     tempVerts[7].vertX = -0.5f; tempVerts[7].vertY = -0.5f; tempVerts[7].vertZ = +0.5f;
     
     tempVerts[8] = tempVerts[0];
     tempVerts[9] = tempVerts[1];
     
     // Base
     tempVerts[10].vertX = -0.5f; tempVerts[10].vertY = +0.5f; tempVerts[10].vertZ = -0.5f;
     tempVerts[11].vertX = -0.5f; tempVerts[11].vertY = +0.5f; tempVerts[11].vertZ = +0.5f;
     tempVerts[12].vertX = +0.5f; tempVerts[12].vertY = +0.5f; tempVerts[12].vertZ = +0.5f;
     tempVerts[13].vertX = +0.5f; tempVerts[13].vertY = +0.5f; tempVerts[13].vertZ = -0.5f;
     
     // Top
     int i;
     for(i = 14; i < 18; i++) {
          tempVerts[i] = tempVerts[i - 4];
          tempVerts[i].vertY = -0.5f;
     }
 
     sceGumDrawArray(GU_TRIANGLE_STRIP, GU_VERTEX_32BITF | GU_TRANSFORM_3D, 18, 0, tempVerts);
     sceGumDrawArray(GU_TRIANGLE_FAN, GU_VERTEX_32BITF | GU_TRANSFORM_3D, 4, 0, &tempVerts[10]);
     sceGumDrawArray(GU_TRIANGLE_FAN, GU_VERTEX_32BITF | GU_TRANSFORM_3D, 4, 0, &tempVerts[14]);
     
     sceGuEnable(GU_TEXTURE_2D);
}

void graphics3dDrawSphere(u32 inDetail) {
     if(graphicsWireframe) {
          graphics3dDrawSphereWireframe(inDetail);
          return;
     }
     
     sceGumPushMatrix();
     vect3f tempScale = (vect3f){ 0.5f, 0.5f, 0.5f };
     sceGumScale(&tempScale);

     sceGuDisable(GU_TEXTURE_2D);

     if(inDetail < 3)
          inDetail = 3;

     vertVf* tempVerts;
     u32 i, j;
     
     float tempSin[inDetail + 1];
     float tempCos[inDetail + 1];
     float tempFloat = (MATH_PI * 2.0f) / (float)inDetail;
     for(i = 0; i < inDetail; i++) {
          tempSin[i] = mathSinf((float)i * tempFloat);
          tempCos[i] = mathCosf((float)i * tempFloat);
     }
     tempSin[inDetail] = tempSin[0];
     tempCos[inDetail] = tempCos[0];
     
     float tempLatitudes[inDetail];
     for(i = 0; i < inDetail; i++) {
          tempFloat = ((float)i / (float)inDetail);
          tempFloat -= 0.5f;
          tempFloat *= -MATH_PI;
          tempFloat = mathSinf(tempFloat);
          tempLatitudes[i] = tempFloat;
     }
     
     float tempScales[inDetail];
     for(i = 0; i < inDetail; i++) {
          tempFloat = ((float)i / (float)inDetail);
          tempFloat -= 0.5f;
          tempFloat *= -MATH_PI;
          tempFloat = mathCosf(tempFloat);
          tempScales[i] = tempFloat;
     }

     #if FL_GRAPHICS_USEDEGENERATETRIS
     u32 tempVertCount = (((inDetail << 1) * inDetail) + (inDetail << 1)) - 2;
     tempVerts = (vertVf*)sceGuGetMemory(tempVertCount * sizeof(vertVf));
     u32 tempBase = 0;
     u32 tempIndex;
     for(i = 0; i < inDetail; i++) {
          tempVerts[tempBase + 0].vertX = 0.0f;
          tempVerts[tempBase + 0].vertY = 0.0f;
          tempVerts[tempBase + 0].vertZ = 1.0f;
               
          if(i > 0) {
               tempBase++;
               tempVerts[tempBase + 0] = tempVerts[tempBase - 1];
          }
          
          for(j = 1, tempIndex = 1; j < inDetail; j++, tempIndex += 2) {
               tempVerts[tempBase + tempIndex + 0].vertX = tempSin[i] * tempScales[j];
               tempVerts[tempBase + tempIndex + 0].vertY = tempCos[i] * tempScales[j];
               tempVerts[tempBase + tempIndex + 0].vertZ = tempLatitudes[j];

               tempVerts[tempBase + tempIndex + 1].vertX = tempSin[i + 1] * tempScales[j];
               tempVerts[tempBase + tempIndex + 1].vertY = tempCos[i + 1] * tempScales[j];
               tempVerts[tempBase + tempIndex + 1].vertZ = tempLatitudes[j];
          }
          
          tempVerts[tempBase + (inDetail << 1) - 1].vertX = 0.0f;
          tempVerts[tempBase + (inDetail << 1) - 1].vertY = 0.0f;
          tempVerts[tempBase + (inDetail << 1) - 1].vertZ = -1.0f;
          
          if(i < (inDetail - 1)) {
               tempBase++;
               tempVerts[tempBase + (inDetail << 1) - 1] = tempVerts[tempBase + (inDetail << 1) - 2];
          }
          
          tempBase += (inDetail << 1);
     }
     sceGumDrawArray(GU_TRIANGLE_STRIP, GU_VERTEX_32BITF | GU_TRANSFORM_3D, tempVertCount, 0, tempVerts);
     #else
     u32 tempIndex;
     for(i = 0; i < inDetail; i++) {
          tempVerts = (vertVf*)sceGuGetMemory((inDetail << 1) * sizeof(vertVf));
          
          tempVerts[0].vertX = 0.0f;
          tempVerts[0].vertY = 0.0f;
          tempVerts[0].vertZ = 1.0f;
          
          for(j = 1, tempIndex = 1; j < inDetail; j++, tempIndex += 2) {
               tempVerts[tempIndex + 0].vertX = tempSin[i] * tempScales[j];
               tempVerts[tempIndex + 0].vertY = tempCos[i] * tempScales[j];
               tempVerts[tempIndex + 0].vertZ = tempLatitudes[j];

               tempVerts[tempIndex + 1].vertX = tempSin[i + 1] * tempScales[j];
               tempVerts[tempIndex + 1].vertY = tempCos[i + 1] * tempScales[j];
               tempVerts[tempIndex + 1].vertZ = tempLatitudes[j];
          }
          
          tempVerts[(inDetail << 1) - 1].vertX = 0.0f;
          tempVerts[(inDetail << 1) - 1].vertY = 0.0f;
          tempVerts[(inDetail << 1) - 1].vertZ = -1.0f;

          sceGumDrawArray(GU_TRIANGLE_STRIP, GU_VERTEX_32BITF | GU_TRANSFORM_3D, (inDetail << 1), 0, tempVerts);
     }
     #endif
     
     sceGuEnable(GU_TEXTURE_2D);
     
     sceGumPopMatrix();
}

void graphics3dDrawSphereWireframe(u32 inDetail) {
     sceGumPushMatrix();
     vect3f tempScale = (vect3f){ 0.5f, 0.5f, 0.5f };
     sceGumScale(&tempScale);

     sceGuDisable(GU_TEXTURE_2D);

     if(inDetail < 3)
          inDetail = 3;

     vertVf* tempVerts;
     u32 i, j;
     
     float tempSin[inDetail + 1];
     float tempCos[inDetail + 1];
     float tempFloat = (MATH_PI * 2.0f) / (float)inDetail;
     for(i = 0; i < inDetail; i++) {
          tempSin[i] = mathSinf((float)i * tempFloat);
          tempCos[i] = mathCosf((float)i * tempFloat);
     }
     tempSin[inDetail] = tempSin[0];
     tempCos[inDetail] = tempCos[0];
     
     float tempLatitudes[inDetail];
     for(i = 0; i < inDetail; i++) {
          tempFloat = ((float)i / (float)inDetail);
          tempFloat -= 0.5f;
          tempFloat *= -MATH_PI;
          tempFloat = mathSinf(tempFloat);
          tempLatitudes[i] = tempFloat;
     }
     
     float tempScales[inDetail];
     for(i = 0; i < inDetail; i++) {
          tempFloat = ((float)i / (float)inDetail);
          tempFloat -= 0.5f;
          tempFloat *= -MATH_PI;
          tempFloat = mathCosf(tempFloat);
          tempScales[i] = tempFloat;
     }

     u32 tempIndex;
     u32 tempVertCount = ((inDetail << 2) - 2) * inDetail;
     u32 tempBase = 0;
     tempVerts = (vertVf*)sceGuGetMemory(tempVertCount * sizeof(vertVf));
     for(i = 0; i < inDetail; i++) {
          
          tempVerts[tempBase + 0].vertX = 0.0f;
          tempVerts[tempBase + 0].vertY = 0.0f;
          tempVerts[tempBase + 0].vertZ = 1.0f;
          
          for(j = 1, tempIndex = 1; j < inDetail; j++, tempIndex += 4) {
               tempVerts[tempBase + tempIndex + 0].vertX = tempSin[i] * tempScales[j];
               tempVerts[tempBase + tempIndex + 0].vertY = tempCos[i] * tempScales[j];
               tempVerts[tempBase + tempIndex + 0].vertZ = tempLatitudes[j];
               
               tempVerts[tempBase + tempIndex + 1] = tempVerts[tempBase + tempIndex + 0];

               tempVerts[tempBase + tempIndex + 2].vertX = tempSin[i + 1] * tempScales[j];
               tempVerts[tempBase + tempIndex + 2].vertY = tempCos[i + 1] * tempScales[j];
               tempVerts[tempBase + tempIndex + 2].vertZ = tempLatitudes[j];
               
               tempVerts[tempBase + tempIndex + 3] = tempVerts[tempBase + tempIndex + 0];
          }
          
          tempBase += ((inDetail << 2) - 2);
          
          tempVerts[tempBase - 1].vertX = 0.0f;
          tempVerts[tempBase - 1].vertY = 0.0f;
          tempVerts[tempBase - 1].vertZ = -1.0f;
     }
     sceGumDrawArray(GU_LINES, GU_VERTEX_32BITF | GU_TRANSFORM_3D, tempVertCount, 0, tempVerts);
     
     sceGuEnable(GU_TEXTURE_2D);
     
     sceGumPopMatrix();
}

void graphics3dDrawCubeWireframea(vect3f inLocation, vect3f inDimensions, vect3f inRotation, u32 inColor) {
          sceGumPushMatrix();
          sceGumScale(&inDimensions);
          sceGumRotateXYZ(&inRotation);
          sceGumTranslate(&inLocation);
          sceGuColor(inColor);
          graphics3dDrawCubeWireframe();
          sceGumPopMatrix();
}

void graphics3dDrawCubeColoreda(vect3f inLocation, vect3f inDimensions, vect3f inRotation, u32 inColor) {
          if(graphicsWireframe)
               graphics3dDrawCubeWireframea(inLocation, inDimensions, inRotation, inColor);
          sceGumPushMatrix();
          sceGumScale(&inDimensions);
          sceGumRotateXYZ(&inRotation);
          sceGumTranslate(&inLocation);
          sceGuColor(inColor);
          graphics3dDrawCube();
          sceGumPopMatrix();
}



#if FL_TEXTURE != 0
void graphics3dDrawCubeTextured(Texture* inTex) {
     texBind(inTex);
     if(inTex->texMipMaps)
          sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR, GU_LINEAR_MIPMAP_LINEAR);
     else
          sceGuTexFilter(GU_LINEAR, GU_LINEAR);
     
     vertTfVf* tempVerts = (vertTfVf*)sceGuGetMemory(sizeof(vertTfVf) * 24);

     // Front
     tempVerts[0].vertX = -0.5f; tempVerts[0].vertY = +0.5f; tempVerts[0].vertZ = -0.5f;
     tempVerts[1].vertX = -0.5f; tempVerts[1].vertY = -0.5f; tempVerts[1].vertZ = -0.5f;
     tempVerts[2].vertX = +0.5f; tempVerts[2].vertY = -0.5f; tempVerts[2].vertZ = -0.5f;
     tempVerts[3].vertX = +0.5f; tempVerts[3].vertY = +0.5f; tempVerts[3].vertZ = -0.5f;
     
     // Right
     tempVerts[4] = tempVerts[3];
     tempVerts[5] = tempVerts[2];
     tempVerts[6].vertX = +0.5f; tempVerts[6].vertY = -0.5f; tempVerts[6].vertZ = +0.5f;
     tempVerts[7].vertX = +0.5f; tempVerts[7].vertY = +0.5f; tempVerts[7].vertZ = +0.5f;
     
     // Back
     tempVerts[8] = tempVerts[7];
     tempVerts[9] = tempVerts[6];
     tempVerts[10].vertX = -0.5f; tempVerts[10].vertY = -0.5f; tempVerts[10].vertZ = +0.5f;
     tempVerts[11].vertX = -0.5f; tempVerts[11].vertY = +0.5f; tempVerts[11].vertZ = +0.5f;
     
     // Left
     tempVerts[12] = tempVerts[11];
     tempVerts[13] = tempVerts[10];
     tempVerts[14] = tempVerts[1];
     tempVerts[15] = tempVerts[0];
     
     // Bottom
     tempVerts[16].vertX = -0.5f; tempVerts[16].vertY = +0.5f; tempVerts[16].vertZ = +0.5f;
     tempVerts[17].vertX = -0.5f; tempVerts[17].vertY = +0.5f; tempVerts[17].vertZ = -0.5f;
     tempVerts[18].vertX = +0.5f; tempVerts[18].vertY = +0.5f; tempVerts[18].vertZ = -0.5f;
     tempVerts[19].vertX = +0.5f; tempVerts[19].vertY = +0.5f; tempVerts[19].vertZ = +0.5f;
     
     // Top
     tempVerts[20].vertX = -0.5f; tempVerts[20].vertY = -0.5f; tempVerts[20].vertZ = -0.5f;
     tempVerts[21].vertX = -0.5f; tempVerts[21].vertY = -0.5f; tempVerts[21].vertZ = +0.5f;
     tempVerts[22].vertX = +0.5f; tempVerts[22].vertY = -0.5f; tempVerts[22].vertZ = +0.5f;
     tempVerts[23].vertX = +0.5f; tempVerts[23].vertY = -0.5f; tempVerts[23].vertZ = -0.5f;
     
     int i;
     for(i = 0; i < 24; i += 4) {
          tempVerts[i + 0].vertU = 1.0f;
          tempVerts[i + 0].vertV = 0.0f;
          tempVerts[i + 1].vertU = 1.0f;
          tempVerts[i + 1].vertV = 1.0f;
          tempVerts[i + 2].vertU = 0.0f;
          tempVerts[i + 2].vertV = 1.0f;
          tempVerts[i + 3].vertU = 0.0f;
          tempVerts[i + 3].vertV = 0.0f;
     }

     sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 4, 0, &tempVerts[0]);
     sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 4, 0, &tempVerts[4]);
     sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 4, 0, &tempVerts[8]);
     sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 4, 0, &tempVerts[12]);
     sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 4, 0, &tempVerts[16]);
     sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 4, 0, &tempVerts[20]);
}

void graphics3dDrawSphereTextured(u32 inDetail, u8 inMapType, Texture* inTex) {
     sceGumPushMatrix();
     vect3f tempScale = (vect3f){ 0.5f, 0.5f, 0.5f };
     sceGumScale(&tempScale);

     texBind(inTex);
     sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
     if(inTex->texMipMaps)
          sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR, GU_LINEAR_MIPMAP_LINEAR);
     else
          sceGuTexFilter(GU_LINEAR, GU_LINEAR);

     if(inDetail < 3)
          inDetail = 3;

     vertTfVf* tempVerts;
     u32 i, j;
     
     float tempSin[inDetail + 1];
     float tempCos[inDetail + 1];
     float tempFloat = (MATH_PI * 2.0f) / (float)inDetail;
     for(i = 0; i < inDetail; i++) {
          tempSin[i] = mathSinf((float)i * tempFloat);
          tempCos[i] = mathCosf((float)i * tempFloat);
     }
     tempSin[inDetail] = tempSin[0];
     tempCos[inDetail] = tempCos[0];
     
     float tempLatitudes[inDetail];
     for(i = 0; i < inDetail; i++) {
          tempFloat = ((float)i / (float)inDetail);
          tempFloat -= 0.5f;
          tempFloat *= -MATH_PI;
          tempFloat = mathSinf(tempFloat);
          tempLatitudes[i] = tempFloat;
     }
     
     float tempScales[inDetail];
     for(i = 0; i < inDetail; i++) {
          tempFloat = ((float)i / (float)inDetail);
          tempFloat -= 0.5f;
          tempFloat *= -MATH_PI;
          tempFloat = mathCosf(tempFloat);
          tempScales[i] = tempFloat;
     }
     
     float tempU[2];
     float tempV;
     
     #if FL_GRAPHICS_USEDEGENERATETRIS
     u32 tempVertCount = (((inDetail << 1) * inDetail) + (inDetail << 1)) - 2;
     tempVerts = (vertTfVf*)sceGuGetMemory(tempVertCount * sizeof(vertTfVf));
     u32 tempBase = 0;
     u32 tempIndex;
     for(i = 0; i < inDetail; i++) {
          
          tempU[0] = 1.0f - ((float)i / (float)inDetail);
          tempU[1] = 1.0f - ((float)(i + 1) / (float)inDetail);
          
          tempVerts[tempBase + 0].vertX = 0.0f;
          tempVerts[tempBase + 0].vertY = 0.0f;
          tempVerts[tempBase + 0].vertZ = 1.0f;
          tempVerts[tempBase + 0].vertU = (tempU[0] + tempU[1]) * 0.5f;
          tempVerts[tempBase + 0].vertV = 0.0f;
               
          if(i > 0) {
               tempBase++;
               tempVerts[tempBase + 0] = tempVerts[tempBase - 1];
          }
          
          for(j = 1, tempIndex = 1; j < inDetail; j++, tempIndex += 2) {
               if(inMapType == SPHERE_MAP_CYLINDER_EQUAL)
                    tempV = 1.0f - ((tempLatitudes[j] + 1.0f) * 0.5f);
               else
                    tempV = ((float)j / (float)inDetail);
               
               tempVerts[tempBase + tempIndex + 0].vertX = tempSin[i] * tempScales[j];
               tempVerts[tempBase + tempIndex + 0].vertY = tempCos[i] * tempScales[j];
               tempVerts[tempBase + tempIndex + 0].vertZ = tempLatitudes[j];
               tempVerts[tempBase + tempIndex + 0].vertU = tempU[0];
               tempVerts[tempBase + tempIndex + 0].vertV = tempV;

               tempVerts[tempBase + tempIndex + 1].vertX = tempSin[i + 1] * tempScales[j];
               tempVerts[tempBase + tempIndex + 1].vertY = tempCos[i + 1] * tempScales[j];
               tempVerts[tempBase + tempIndex + 1].vertZ = tempLatitudes[j];
               tempVerts[tempBase + tempIndex + 1].vertU = tempU[1];
               tempVerts[tempBase + tempIndex + 1].vertV = tempV;
          }
          
          tempVerts[tempBase + (inDetail << 1) - 1].vertX = 0.0f;
          tempVerts[tempBase + (inDetail << 1) - 1].vertY = 0.0f;
          tempVerts[tempBase + (inDetail << 1) - 1].vertZ = -1.0f;
          tempVerts[tempBase + (inDetail << 1) - 1].vertU = (tempU[0] + tempU[1]) * 0.5f;
          tempVerts[tempBase + (inDetail << 1) - 1].vertV = 1.0f;
          
          if(i < (inDetail - 1)) {
               tempBase++;
               tempVerts[tempBase + (inDetail << 1) - 1] = tempVerts[tempBase + (inDetail << 1) - 2];
          }
          
          tempBase += (inDetail << 1);
     }
     sceGumDrawArray(GU_TRIANGLE_STRIP, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, tempVertCount, 0, tempVerts);
     #else
     u32 tempIndex;
     for(i = 0; i < inDetail; i++) {
          tempVerts = (vertTfVf*)sceGuGetMemory((inDetail << 1) * sizeof(vertTfVf));
          
          tempU[0] = 1.0f - ((float)i / (float)inDetail);
          tempU[1] = 1.0f - ((float)(i + 1) / (float)inDetail);
          
          tempVerts[0].vertX = 0.0f;
          tempVerts[0].vertY = 0.0f;
          tempVerts[0].vertZ = 1.0f;
          tempVerts[0].vertU = (tempU[0] + tempU[1]) * 0.5f;
          tempVerts[0].vertV = 0.0f;
          
          for(j = 1, tempIndex = 1; j < inDetail; j++, tempIndex += 2) {
               if(inMapType == SPHERE_MAP_CYLINDER_EQUAL)
                    tempV = 1.0f - ((tempLatitudes[j] + 1.0f) * 0.5f);
               else
                    tempV = ((float)j / (float)inDetail);
               
               tempVerts[tempIndex + 0].vertX = tempSin[i] * tempScales[j];
               tempVerts[tempIndex + 0].vertY = tempCos[i] * tempScales[j];
               tempVerts[tempIndex + 0].vertZ = tempLatitudes[j];
               tempVerts[tempIndex + 0].vertU = tempU[0];
               tempVerts[tempIndex + 0].vertV = tempV;

               tempVerts[tempIndex + 1].vertX = tempSin[i + 1] * tempScales[j];
               tempVerts[tempIndex + 1].vertY = tempCos[i + 1] * tempScales[j];
               tempVerts[tempIndex + 1].vertZ = tempLatitudes[j];
               tempVerts[tempIndex + 1].vertU = tempU[1];
               tempVerts[tempIndex + 1].vertV = tempV;
          }
          
          tempVerts[(inDetail << 1) - 1].vertX = 0.0f;
          tempVerts[(inDetail << 1) - 1].vertY = 0.0f;
          tempVerts[(inDetail << 1) - 1].vertZ = -1.0f;
          tempVerts[(inDetail << 1) - 1].vertU = (tempU[0] + tempU[1]) * 0.5f;
          tempVerts[(inDetail << 1) - 1].vertV = 1.0f;

          sceGumDrawArray(GU_TRIANGLE_STRIP, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, (inDetail << 1), 0, tempVerts);
     }
     #endif
     
     sceGumPopMatrix();
}
#endif

#endif
