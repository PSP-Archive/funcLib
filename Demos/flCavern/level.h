#include "././Code/funcLib.h"

#ifndef _level_h
#define _level_h

#define LEVEL_SEGMENT_TYPE_NORMAL 0
#define LEVEL_SEGMENT_TYPE_CAVERN 0 // A cavern is a special case of NORMAL.
#define LEVEL_SEGMENT_TYPE_DOOR   1
#define LEVEL_SEGMENT_TYPE_EXIT   2
#define LEVEL_SEGMENT_TYPE_MAGMA  3
#define LEVEL_SEGMENT_TYPE_WATER  4

#define SIN45 (0.70710678118654752440084436210485f)

typedef struct {
     vect3f lvlSegSize[2];
     vect3f lvlSegPos[2];
     
     u8     lvlSegType;
     void*  lvlSegTypeData;
     
     void*  lvlSegNext;
     
     u32    lvlSegColor[2];
} LevelSegment;

typedef struct {
     LevelSegment* lvlSegTree;
     float         lvlDrawDist;
     
     float         lvlAvgSegDepth;
     float         lvlAvgSegWidth;
     float         lvlAvgSegHeight;
     
     float         lvlMaxVariance;
     float         lvlFriction;
     float         lvlGravity;
} Level;

extern Level* lvlCreate(float inLength);
extern void   lvlFree(Level* inLvl);

extern float  lvlLength(Level* inLvl);
extern u32    lvlSegments(Level* inLvl);

extern void   lvlDraw(Level* inLvl, Camera* inCam);

extern bool   lvlAddSegRandom(Level* inLvl);
extern void   lvlAddSegExit(Level* inLvl);

extern void   lvlSegFree(LevelSegment* lvlSeg);
extern void   lvlSegDraw(LevelSegment* inLvlSeg);

#endif
