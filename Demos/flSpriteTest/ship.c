#include "ship.h"

Ship* shipCreate(Sprite* inSprite, int inX, int inY) {
     Ship* tempOut = memAlloc(sizeof(Ship));
     tempOut->shipSprite = inSprite;
     tempOut->shipX = (float)inX;
     tempOut->shipY = (float)inY;
     tempOut->shipVelX = 0.0f;
     tempOut->shipVelY = 0.0f;
     tempOut->shipAngle = 0.0f;
     tempOut->shipAngVel = 0.0f;
     tempOut->shipThrust = 1.0f;
     tempOut->shipAngThrust = (MATH_PI / 180.0f);
     return tempOut;
}

void shipTick(Ship* inShip) {
     inShip->shipX += inShip->shipVelX;
     inShip->shipY += inShip->shipVelY;
     inShip->shipAngle += inShip->shipAngVel;
}

void shipDraw(Ship* inShip) {
     inShip->shipSprite->spriteAngle = inShip->shipAngle;
     inShip->shipSprite->spriteX = inShip->shipX;
     inShip->shipSprite->spriteY = inShip->shipY;
     spriteDraw2dFrame(inShip->shipSprite, 0);
}

void shipThrust(Ship* inShip) {
     inShip->shipVelX += (inShip->shipThrust * mathSinf(inShip->shipAngle));
     inShip->shipVelY -= (inShip->shipThrust * mathCosf(inShip->shipAngle));
}

void shipRotate(Ship* inShip, float inRate) {
     inShip->shipAngVel += (inRate * inShip->shipAngThrust);
}
