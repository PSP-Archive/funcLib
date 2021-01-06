#include "ship.h"

Ship* shipCreate(Level* inLevel) {
     Ship* tempOut = (Ship*)memAlloc(sizeof(Ship));
     if(!tempOut)
          return NULL;
          
     tempOut->shipPos = (vect3f){ 0.0f, 0.0f, 0.0f };
     tempOut->shipRot = (vect2f){ 0.0f, 0.0f };
     tempOut->shipVel = (vect3f){ 0.0f, 0.0f, 0.0f };
     tempOut->shipLevel = inLevel;
     
     return tempOut;
}

void shipFree(Ship* inShip) {
     memFree(inShip);
}
void shipCamUpdate(Ship* inShip, Camera* inCam) {
     camMoveTo(inCam, (vect3f){ inShip->shipPos.x, inShip->shipPos.y, inShip->shipPos.z });
     inCam->camRot.x = -inShip->shipRot.x;
     inCam->camRot.y = inShip->shipRot.y;
}

void shipThrust(Ship* inShip, vect3f inValue) {
     inShip->shipVel = vect3f_Add(inShip->shipVel, inValue);
}

void shipTick(Ship* inShip) {
     inShip->shipVel.z += (inShip->shipLevel->lvlGravity);
     
     inShip->shipVel.x *= (inShip->shipLevel->lvlFriction);
     //inShip->shipVel.y *= (inShip->shipLevel->lvlFriction);
     inShip->shipVel.z *= (inShip->shipLevel->lvlFriction);
     
     inShip->shipPos.x += inShip->shipVel.x;
     inShip->shipPos.y += inShip->shipVel.y;
     inShip->shipPos.z += inShip->shipVel.z;
}
