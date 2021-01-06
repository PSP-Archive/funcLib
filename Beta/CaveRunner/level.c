#include "level.h"
#include <pspgum.h>
#include <pspgu.h>

Level* levelCreate(u32 inLength) {
     if(inLength <= 1)
          return NULL;
     Level* tempOut = (Level*)memAlloc(sizeof(Level));
     if(!tempOut)
          return NULL;
     tempOut->lvlLength = inLength;
     tempOut->lvlSegments = (LevelSeg*)memAlloc(sizeof(LevelSeg) * inLength);
     if(!tempOut->lvlSegments) {
          memFree(tempOut);
          return NULL;
     }
     
     u32 i;
     levelSegCreate(NULL, tempOut->lvlSegments);
     for(i = 1; i < inLength; i++)
          levelSegCreate(&tempOut->lvlSegments[i - 1], &tempOut->lvlSegments[i]);
     
     tempOut->lvlGravity = -0.0125f;
     tempOut->lvlFriction = 0.995f;

     return tempOut;
}

void levelFree(Level* inLevel) {
     if(!inLevel)
          return;
     if(inLevel->lvlSegments)
          memFree(inLevel->lvlSegments);
     memFree(inLevel);
}

void levelSegCreate(LevelSeg* inLast, LevelSeg* inCur) {
     if(!inCur)
          return;
     if(!inLast) {
          inCur->lvlSegSize = (vect3f){ 16.0f, 16.0f, 16.0f };
          inCur->lvlSegPos  = (vect2f){ 0.0f, 0.0f };
          inCur->lvlSegColor = 0xFF00FFFF; //colorRandom8888(64, 192, 0xFF);
          inCur->lvlSegType = 0;
          inCur->lvlSegData = NULL;
          return;
     }
     inCur->lvlSegSize = inLast->lvlSegSize;
     inCur->lvlSegPos.x = inLast->lvlSegPos.x;
     //inCur->lvlSegPos.y = inLast->lvlSegPos.y;
     //inCur->lvlSegPos.x = inLast->lvlSegPos.x + ((mathRandi(2) == 0 ? -1 : 1) * (((float)mathRandi(256) / 510.0f) * inLast->lvlSegSize.z));
     inCur->lvlSegPos.y = inLast->lvlSegPos.y + ((mathRandi(2) == 0 ? -1 : 1) * (((float)mathRandi(256) / 510.0f) * inLast->lvlSegSize.z));
     inCur->lvlSegColor = (~inLast->lvlSegColor | 0xFF000000);
     inCur->lvlSegType = 0;
     inCur->lvlSegData = NULL;
    
}

void levelDraw(Level* inLevel, vect3f inPos, float inDrawDist) {
     if(!inLevel || !inLevel->lvlSegments)
          return;

     float tempDist = 0.0f;
     u32 tempStart = 0;
     while((tempDist < inPos.y) && (tempStart < inLevel->lvlLength)) {
          tempDist += inLevel->lvlSegments[tempStart].lvlSegSize.z;
          tempStart++;
     }
     if(tempStart) {
          if(tempStart >= inLevel->lvlLength)
               return;
          tempStart--;
          tempDist += inLevel->lvlSegments[tempStart].lvlSegSize.z;
     }

     u32 tempDrawDistSegs = 0;
     float tempDrawDist = (inPos.y - tempDist);
     while((tempDrawDist < inDrawDist) && (tempDrawDistSegs < (inLevel->lvlLength - tempStart))) {
          tempDrawDist += inLevel->lvlSegments[tempStart + tempDrawDistSegs].lvlSegSize.z;
          tempDrawDistSegs++;
     }
     
     sceGuColor(0xFFFFFFFF);
     sceGuDisable(GU_TEXTURE_2D);
     sceGuDisable(GU_CULL_FACE);
     sceGuDisable(GU_SCISSOR_TEST);
     sceGuDisable(GU_CLIP_PLANES);

     u32 i, j;
     float tempBaseX[2], tempBaseY[2];
     float tempDepth, tempLumMult;
     u32 tempColor;
     vertCVf* tempVerts;
     for(i = 0; i < 16; i++) {
          tempBaseX[0] = mathSinf(((float)i * MATH_PI) / 8);
          j = ((i + 1) & 15);
          tempBaseX[1] = mathSinf(((float)j * MATH_PI) / 8);
          j = ((i + 4) & 15);
          tempBaseY[0] = mathSinf(((float)j * MATH_PI) / 8);
          j = ((i + 5) & 15);
          tempBaseY[1] = mathSinf(((float)j * MATH_PI) / 8);
          
          tempDepth = (tempDist - inPos.y);
          tempVerts = (vertCVf*)sceGuGetMemory(sizeof(vertCVf) * (tempDrawDistSegs << 1));
          for(j = tempStart; j < (tempStart + tempDrawDistSegs); j++) {
               tempColor = inLevel->lvlSegments[j].lvlSegColor;
               tempLumMult = 1.0f - (tempDepth / inDrawDist);
               tempColor = colorLumMult8888(tempColor, tempLumMult);
               
               tempVerts[(j << 1) + 0].vertColor = tempColor;
               tempVerts[(j << 1) + 0].vertX = (tempBaseX[0] * inLevel->lvlSegments[j].lvlSegSize.x) + inLevel->lvlSegments[j].lvlSegPos.x;
               tempVerts[(j << 1) + 0].vertY = tempDepth;
               tempVerts[(j << 1) + 0].vertZ = (tempBaseY[0] * inLevel->lvlSegments[j].lvlSegSize.y) + inLevel->lvlSegments[j].lvlSegPos.y;
               tempVerts[(j << 1) + 1].vertColor = tempColor;
               tempVerts[(j << 1) + 1].vertX = (tempBaseX[1] * inLevel->lvlSegments[j].lvlSegSize.x) + inLevel->lvlSegments[j].lvlSegPos.x;
               tempVerts[(j << 1) + 1].vertY = tempDepth;
               tempVerts[(j << 1) + 1].vertZ = (tempBaseY[1] * inLevel->lvlSegments[j].lvlSegSize.y) + inLevel->lvlSegments[j].lvlSegPos.y;
               tempDepth += inLevel->lvlSegments[j].lvlSegSize.z;
          }
          sceGumDrawArray(GU_TRIANGLE_STRIP, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, (tempDrawDistSegs << 1), 0, tempVerts);
     }
     
     sceGuEnable(GU_TEXTURE_2D);
     sceGuEnable(GU_CULL_FACE);
     sceGuEnable(GU_SCISSOR_TEST);
     sceGuEnable(GU_CLIP_PLANES);
}
