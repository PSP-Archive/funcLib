#include "././Code/funcLib.h"
#include "level.h"

#define SHIP_THRUST_WARMUP   (1.0f / 50.0f)
#define SHIP_THRUST_WARMDOWN (1.0f / 50.0f)

#ifndef _ship_h
#define _ship_h

typedef struct {
     vect3f     shipPos;
     //vect3f     shipScale;
     Mdl3dStat* shipMdl;
     vect2f     shipRot;
     vect3f     shipVel;
     float      shipThrustTemp;
     Level*     shipLevel;
     
     float      shipThrustCur;
     bool       shipThrusting;
} Ship;

extern Ship* shipCreate(Mdl3dStat* inModel, Level* inLevel);
extern void  shipDraw(Ship* inShip);
extern void  shipCamUpdate(Ship* inShip, Camera* inCam);
extern void  shipCockpitCamUpdate(Ship* inShip, Camera* inCam);
extern void  shipFree(Ship* inShip);

extern void  shipAngleV(Ship* inShip, float inValue);
extern void  shipAngleH(Ship* inShip, float inValue);
extern void  shipThrust(Ship* inShip, float inValue);
extern void  shipTick(Ship* inShip);

#endif
