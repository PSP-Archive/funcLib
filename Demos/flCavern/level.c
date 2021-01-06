#include "level.h"
#include <pspgum.h>
#include <pspgu.h>

void lvlSegDrawNormal(LevelSegment* inLvlSeg);
void lvlSegDrawDoor(LevelSegment* inLvlSeg);
void lvlSegDrawExit(LevelSegment* inLvlSeg);
void lvlSegDrawMagma(LevelSegment* inLvlSeg);
void lvlSegDrawWater(LevelSegment* inLvlSeg);

LevelSegment* lvlSegCreateNormal(LevelSegment* inBase, Level* inLvl);
LevelSegment* lvlSegCreateCavern(LevelSegment* inBase, Level* inLvl);

Level* lvlCreate(float inLength) {
     Level* tempOut = (Level*)memAlloc(sizeof(Level));
     if(!tempOut)
          return NULL;
     
     tempOut->lvlSegTree = NULL;
     tempOut->lvlDrawDist = 4096.0f;
     
     tempOut->lvlAvgSegWidth = 24.0f;
     tempOut->lvlAvgSegHeight = 48.0f;
     tempOut->lvlAvgSegDepth = 8.0f;

     tempOut->lvlMaxVariance = 2.0f;
     tempOut->lvlFriction = (127.0f / 128.0f);
     tempOut->lvlGravity = (-1.0f / 256.0f);
     
     while(lvlLength(tempOut) < inLength) {
          if(!lvlAddSegRandom(tempOut))
               break;
     }
     //lvlAddSegExit(tempOut);
     
     return tempOut;
}

void lvlFree(Level* inLvl) {
     if(!inLvl)
          return;
     if(inLvl->lvlSegTree)
          lvlSegFree(inLvl->lvlSegTree);
     memFree(inLvl);
}

float lvlLength(Level* inLvl) {
     if(!inLvl || !inLvl->lvlSegTree)
          return 0.0f;
     LevelSegment* tempSeg = inLvl->lvlSegTree;
     float tempMax = -65536.0f;
     float tempMin = 65536.0f;
     while(tempSeg) {
          if(tempSeg->lvlSegPos[0].y > tempMax)
               tempMax = tempSeg->lvlSegPos[0].y;
          if(tempSeg->lvlSegPos[1].y > tempMax)
               tempMax = tempSeg->lvlSegPos[1].y;
          if(tempSeg->lvlSegPos[0].y < tempMin)
               tempMin = tempSeg->lvlSegPos[0].y;
          if(tempSeg->lvlSegPos[1].y < tempMin)
               tempMin = tempSeg->lvlSegPos[1].y;
          tempSeg = (LevelSegment*)tempSeg->lvlSegNext;
     }
     return (tempMax - tempMin);
}

u32 lvlSegments(Level* inLvl) {
    if(!inLvl || !inLvl->lvlSegTree)
          return 0;
     LevelSegment* tempSeg = inLvl->lvlSegTree;
     u32 tempOut = 0;
     while(tempSeg) {
          tempOut ++;
          tempSeg = (LevelSegment*)tempSeg->lvlSegNext;
     }
     return tempOut;
}

void lvlDraw(Level* inLvl, Camera* inCam) {
     if(!inLvl || !inLvl->lvlSegTree || !inCam)
          return;

     sceGumPushMatrix();
     sceGumLoadIdentity();
          
     LevelSegment* tempSeg = inLvl->lvlSegTree;
     while(tempSeg) {
          if((tempSeg->lvlSegPos[0].y >= inCam->camPos.y) && (tempSeg->lvlSegPos[1].y < (inCam->camPos.y + inLvl->lvlDrawDist)))
               lvlSegDraw(tempSeg);
          tempSeg = (LevelSegment*)tempSeg->lvlSegNext;
     }
     
     sceGumPopMatrix();
}

bool lvlAddSegRandom(Level* inLvl) {
     if(!inLvl)
          return false;
     if(!inLvl->lvlSegTree) {
          inLvl->lvlSegTree = lvlSegCreateNormal(NULL, inLvl);
          return (inLvl->lvlSegTree != NULL);
     }
     
     LevelSegment* tempSegAdd = NULL;
     LevelSegment* tempSeg = inLvl->lvlSegTree;
     while(tempSeg->lvlSegNext)
          tempSeg = (LevelSegment*)tempSeg->lvlSegNext;
     
     int tempRand = mathRandi(128);
     if(tempRand <= 120) {
          tempSegAdd = lvlSegCreateNormal(tempSeg, inLvl);
     } else {
          //tempSegAdd = lvlSegCreateCavern(tempSeg, inLvl);
          tempSegAdd = lvlSegCreateNormal(tempSeg, inLvl);
     }
     tempSeg->lvlSegNext = tempSegAdd;
     return (tempSegAdd != NULL);
}

void lvlSegDraw(LevelSegment* inLvlSeg) {
     if(!inLvlSeg)
          return;
     if(inLvlSeg->lvlSegType == LEVEL_SEGMENT_TYPE_NORMAL) {
          lvlSegDrawNormal(inLvlSeg);
     } else if(inLvlSeg->lvlSegType == LEVEL_SEGMENT_TYPE_DOOR) {
            
     } else if(inLvlSeg->lvlSegType == LEVEL_SEGMENT_TYPE_EXIT) {
          
     }
}

void lvlSegFree(LevelSegment* inLvlSeg) {
     if(!inLvlSeg)
          return;
     if(inLvlSeg->lvlSegNext)
          lvlSegFree((LevelSegment*)inLvlSeg->lvlSegNext);
     memFree(inLvlSeg);
}

void lvlSegDrawNormal(LevelSegment* inLvlSeg) {
     if(!inLvlSeg)
          return;
     
     sceGuDisable(GU_TEXTURE_2D);

     sceGuColor(0xFFFFFFFF);

     vertCVf* tempVerts = (vertCVf*)sceGuGetMemory(sizeof(vertCVf) * 18);
     int i;
     for(i = 0; i < 9; i++) {
          tempVerts[(i << 1) + 1].vertColor = inLvlSeg->lvlSegColor[0];
          
          tempVerts[(i << 1) + 1].vertX = (i < 4 ? 1.0f : -1.0f) * ((i & 1 ? SIN45 : 0.0f) + ((i & 3) == 2 ? 1.0f : 0.0f));
          tempVerts[(i << 1) + 1].vertX *= inLvlSeg->lvlSegSize[0].x;
          tempVerts[(i << 1) + 1].vertX += inLvlSeg->lvlSegPos[0].x;
          
          tempVerts[(i << 1) + 1].vertZ = ((i < 2) || (i > 6) ? 1.0f : -1.0f) * ((i & 1 ? SIN45 : 0.0f) + ((i & 3) == 0 ? 1.0f : 0.0f));
          tempVerts[(i << 1) + 1].vertZ *= inLvlSeg->lvlSegSize[0].z;
          tempVerts[(i << 1) + 1].vertZ += inLvlSeg->lvlSegPos[0].z;
          
          tempVerts[(i << 1) + 1].vertY = inLvlSeg->lvlSegPos[0].y;

          tempVerts[(i << 1) + 1].vertColor = inLvlSeg->lvlSegColor[1];
          
          tempVerts[(i << 1) + 0].vertX = (i < 4 ? 1.0f : -1.0f) * ((i & 1 ? SIN45 : 0.0f) + ((i & 3) == 2 ? 1.0f : 0.0f));
          tempVerts[(i << 1) + 0].vertX *= inLvlSeg->lvlSegSize[1].x;
          tempVerts[(i << 1) + 0].vertX += inLvlSeg->lvlSegPos[1].x;
          
          tempVerts[(i << 1) + 0].vertZ = ((i < 2) || (i > 6) ? 1.0f : -1.0f) * ((i & 1 ? SIN45 : 0.0f) + ((i & 3) == 0 ? 1.0f : 0.0f));
          tempVerts[(i << 1) + 0].vertZ *= inLvlSeg->lvlSegSize[1].z;
          tempVerts[(i << 1) + 0].vertZ += inLvlSeg->lvlSegPos[1].z;
          
          tempVerts[(i << 1) + 0].vertY = inLvlSeg->lvlSegPos[1].y;
     }
     sceGumDrawArray(GU_TRIANGLE_STRIP, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 18, 0, tempVerts);
     
     sceGuEnable(GU_TEXTURE_2D);
}

LevelSegment* lvlSegCreateNormal(LevelSegment* inBase, Level* inLvl) {
     if(!inLvl)
          return NULL;
     LevelSegment* tempOut = (LevelSegment*)memAlloc(sizeof(LevelSegment));
     if(!tempOut)
          return NULL;
     if(!inBase) {
          tempOut->lvlSegSize[0] = (vect3f){ inLvl->lvlAvgSegWidth, inLvl->lvlAvgSegDepth, inLvl->lvlAvgSegHeight };
          tempOut->lvlSegPos[0] = (vect3f){ 0.0f, 0.0f, 0.0f };
          tempOut->lvlSegColor[0] = colorRandom8888(63, 192, 0xFF);
     } else {
          tempOut->lvlSegSize[0] = inBase->lvlSegSize[1];
          tempOut->lvlSegPos[0] = inBase->lvlSegPos[1];
          tempOut->lvlSegColor[0] = (~(inBase->lvlSegColor[1]) | 0xFF000000);
     }
     
     float tempRandf = ((((float)mathRandi(256) / 64.0f) - 1.0f) * ((inLvl->lvlMaxVariance - 1.0f) * 0.125f));
     tempOut->lvlSegSize[1].x = (tempOut->lvlSegSize[0].x + (tempRandf * inLvl->lvlAvgSegWidth));
     if(tempOut->lvlSegSize[1].x > (inLvl->lvlAvgSegWidth * inLvl->lvlMaxVariance))
          tempOut->lvlSegSize[1].x = (inLvl->lvlAvgSegWidth * inLvl->lvlMaxVariance);
     else if(tempOut->lvlSegSize[1].x < (inLvl->lvlAvgSegWidth * (1.0f / inLvl->lvlMaxVariance)))
          tempOut->lvlSegSize[1].x = (inLvl->lvlAvgSegWidth * (1.0f / inLvl->lvlMaxVariance));
     tempRandf = ((((float)mathRandi(256) / 64.0f) - 1.0f) * ((inLvl->lvlMaxVariance - 1.0f) * 0.125f));
     tempOut->lvlSegSize[1].y = (inLvl->lvlAvgSegDepth + (tempRandf * inLvl->lvlAvgSegDepth));
     tempRandf = ((((float)mathRandi(256) / 64.0f) - 1.0f) * ((inLvl->lvlMaxVariance - 1.0f) * 0.125f));
     tempOut->lvlSegSize[1].z = (tempOut->lvlSegSize[0].z + (tempRandf * inLvl->lvlAvgSegHeight));
     if(tempOut->lvlSegSize[1].z > (inLvl->lvlAvgSegHeight * inLvl->lvlMaxVariance))
          tempOut->lvlSegSize[1].z = (inLvl->lvlAvgSegHeight * inLvl->lvlMaxVariance);
     else if(tempOut->lvlSegSize[1].z < (inLvl->lvlAvgSegHeight * (1.0f / inLvl->lvlMaxVariance)))
          tempOut->lvlSegSize[1].z = (inLvl->lvlAvgSegHeight * (1.0f / inLvl->lvlMaxVariance));
     
     tempOut->lvlSegPos[1].x = tempOut->lvlSegPos[0].x;
     tempOut->lvlSegPos[1].y = (tempOut->lvlSegPos[0].y + tempOut->lvlSegSize[1].y);
     tempRandf = ((((float)mathRandi(256) / 64.0f) - 1.0f) * ((inLvl->lvlMaxVariance - 1.0f) * 0.125f));
     if((tempOut->lvlSegPos[1].z - tempOut->lvlSegPos[0].z) > (tempOut->lvlSegSize[1].y * 0.5f))
          tempOut->lvlSegPos[1].z = (tempOut->lvlSegSize[1].y * 0.5f) + tempOut->lvlSegPos[0].z;
     else if((tempOut->lvlSegPos[1].z - tempOut->lvlSegPos[0].z) < (tempOut->lvlSegSize[1].y * -0.5f))
          tempOut->lvlSegPos[1].z = (tempOut->lvlSegSize[1].y * -0.5f) + tempOut->lvlSegPos[0].z;
     tempOut->lvlSegPos[1].z = (tempOut->lvlSegPos[0].z + (tempRandf * inLvl->lvlAvgSegHeight));
     
     tempOut->lvlSegColor[1] = tempOut->lvlSegColor[0];
     
     tempOut->lvlSegType = LEVEL_SEGMENT_TYPE_NORMAL;
     tempOut->lvlSegTypeData = NULL;
     tempOut->lvlSegNext = NULL;
     
     return tempOut;
}
