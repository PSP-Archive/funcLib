#include "././Code/funcLib.h"
#include "level.h"

#ifndef _ship_h
#define _ship_h

typedef struct {
     vect3f     shipPos;
     vect2f     shipRot;
     vect3f     shipVel;
     Level*     shipLevel;
} Ship;

extern Ship* shipCreate(Level* inLevel);
extern void  shipCamUpdate(Ship* inShip, Camera* inCam);
extern void  shipFree(Ship* inShip);

extern void shipThrust(Ship* inShip, vect3f inValue);
extern void  shipTick(Ship* inShip);

#endif
