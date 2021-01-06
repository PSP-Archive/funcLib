#include "././Code/funcLib.h"

#ifndef _level_h
#define _level_h

typedef struct {
     vect3f lvlSegSize;
     vect2f lvlSegPos;
     u32    lvlSegColor;
     
     u32    lvlSegType;
     void*  lvlSegData;
} LevelSeg;

typedef struct {
     u32       lvlLength;
     LevelSeg* lvlSegments;
     float     lvlGravity;
     float     lvlFriction;
} Level;

extern Level*    levelCreate(u32 inLength);
extern void      levelFree(Level* inLevel);

extern void      levelSegCreate(LevelSeg* inLast, LevelSeg* inCur);

extern void      levelDraw(Level* inLevel, vect3f inPos, float inDrawDist);

#endif
