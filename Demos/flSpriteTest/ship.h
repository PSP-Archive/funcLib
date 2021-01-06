#ifndef SHIP_H
#define SHIP_H

#include "././Code/funcLib.h"

typedef struct {
     Sprite* shipSprite;
     float   shipX, shipY;
     float   shipVelX, shipVelY;
     float   shipAngle;
     float   shipAngVel;
     float   shipThrust;
     float   shipAngThrust;
} Ship;

extern Ship* shipCreate(Sprite* inSprite, int inX, int inY);
extern void  shipTick(Ship* inShip);
extern void  shipDraw(Ship* inShip);
extern void  shipThrust(Ship* inShip);
extern void  shipRotate(Ship* inShip, float inRate);

#endif
