#include "ship.h"
#include <pspgum.h>
#include <pspgu.h>

Ship* shipCreate(Mdl3dStat* inModel, Level* inLevel) {
     Ship* tempOut = (Ship*)memAlloc(sizeof(Ship));
     if(!tempOut)
          return NULL;
     tempOut->shipPos = (vect3f){ 0.0f, 0.0f, 0.0f };
     tempOut->shipMdl = inModel;
     //tempOut->shipScale = (vect3f){ 1.0f, 1.0f, 1.0f };
     tempOut->shipRot = (vect2f){ 0.0f, 0.0f };
     tempOut->shipVel = (vect3f){ 0.0f, 0.0f, 0.0f };
     tempOut->shipLevel = inLevel;
     
     tempOut->shipThrustCur = 0.0f;
     tempOut->shipThrusting = false;
     return tempOut;
}

void shipDraw(Ship* inShip) {
     sceGumPushMatrix();
     
     //sceGumScale(&inShip->shipScale);
     sceGumTranslate(&inShip->shipPos);
     sceGumRotateZ(inShip->shipRot.x);
     sceGumRotateX(inShip->shipRot.y);
     
     sceGuColor(0xFF7F7F7F);
     mdl3dStatDraw(inShip->shipMdl);
     
     sceGuColor(0xFFFFFFFF);
     graphicsWireframe = true;
     mdl3dStatDraw(inShip->shipMdl);
     graphicsWireframe = false;
     
     sceGumPopMatrix();
}

void shipFree(Ship* inShip) {
     memFree(inShip);
}

void shipCamUpdate(Ship* inShip, Camera* inCam) {
     camMoveTo(inCam, (vect3f){ inShip->shipPos.x, (inShip->shipPos.y - 5.0f), inShip->shipPos.z });
     inCam->camRot.x = 0;
     inCam->camRot.y = 0;
}

void shipCockpitCamUpdate(Ship* inShip, Camera* inCam) {
     camMoveTo(inCam, (vect3f){ inShip->shipPos.x, inShip->shipPos.y, inShip->shipPos.z });
     inCam->camRot.x = -inShip->shipRot.x;
     inCam->camRot.y = inShip->shipRot.y;
}

void shipAngleV(Ship* inShip, float inValue) {
     inShip->shipRot.y += inValue;
     if(inShip->shipRot.y > (MATH_PI * 0.5f))
          inShip->shipRot.y = (MATH_PI * 0.5f);
     if(inShip->shipRot.y < (-MATH_PI * 0.5f))
          inShip->shipRot.y = (-MATH_PI * 0.5f);
}

void shipAngleH(Ship* inShip, float inValue) {
     inShip->shipRot.x += inValue;
     if(inShip->shipRot.x > (MATH_PI * 0.5f))
          inShip->shipRot.x = (MATH_PI * 0.5f);
     if(inShip->shipRot.x < (-MATH_PI * 0.5f))
          inShip->shipRot.x = (-MATH_PI * 0.5f);
}

void shipThrust(Ship* inShip, float inValue) {
     inShip->shipThrustCur = inValue;
     inShip->shipThrusting = true;
}

void shipTick(Ship* inShip) {
     if(inShip->shipThrusting) {
          inShip->shipThrustTemp += SHIP_THRUST_WARMUP;
          if(inShip->shipThrustTemp > 1.0f)
               inShip->shipThrustTemp = 1.0f;
     } else {
          inShip->shipThrustTemp -= SHIP_THRUST_WARMDOWN;
          if(inShip->shipThrustTemp < 0.0f)
               inShip->shipThrustTemp = 0.0f;
     }
     inShip->shipThrustCur *= inShip->shipThrustTemp;
     inShip->shipVel.y += inShip->shipThrustCur * (mathCosf(inShip->shipRot.x) * mathCosf(inShip->shipRot.y));
     inShip->shipVel.x -= inShip->shipThrustCur * mathSinf(inShip->shipRot.x);
     inShip->shipVel.z += inShip->shipThrustCur * mathSinf(inShip->shipRot.y);
     
     inShip->shipVel.z += (inShip->shipThrustCur * 0.1f);
     
     inShip->shipVel.z += (inShip->shipLevel->lvlGravity);
     
     inShip->shipVel.x *= (inShip->shipLevel->lvlFriction);
     inShip->shipVel.y *= (inShip->shipLevel->lvlFriction);
     inShip->shipVel.z *= (inShip->shipLevel->lvlFriction);
     
     inShip->shipPos.x += inShip->shipVel.x;
     inShip->shipPos.y += inShip->shipVel.y;
     inShip->shipPos.z += inShip->shipVel.z;
     
     inShip->shipThrusting = false;
}
