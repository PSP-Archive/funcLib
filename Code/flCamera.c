#include "flGlobal.h"
#if FL_CAMERA != 0
#include <stdio.h>
#include <psprtc.h>
#include <pspkernel.h>
#include <pspgu.h>
#include <pspgum.h>

#if FL_INCLUDE_ALL_C == 0
#include "flCamera.h"
#include "flMemory.h"
#include "flMath.h"

#if FL_DEBUG != 0
#include "flDebug.h"
#endif
#endif

void camViewCalc(Camera* inCam) {
     while(inCam->camRot.y > MATH_PI)
          inCam->camRot.y -= (MATH_PI * 2.0f);
     while(inCam->camRot.y <= -MATH_PI)
          inCam->camRot.y += (MATH_PI * 2.0f);

     while(inCam->camRot.x > MATH_PI)
          inCam->camRot.x -= (MATH_PI * 2.0f);
     while(inCam->camRot.x <= -MATH_PI)
          inCam->camRot.x += (MATH_PI * 2.0f);
          
     while(inCam->camRot.z > MATH_PI)
          inCam->camRot.z -= (MATH_PI * 2.0f);
     while(inCam->camRot.z <= -MATH_PI)
          inCam->camRot.z += (MATH_PI * 2.0f);
     
     inCam->camView.x = (mathSinf(inCam->camRot.x) * mathCosf(inCam->camRot.y));
     inCam->camView.y = (mathCosf(inCam->camRot.x) * mathCosf(inCam->camRot.y));
     inCam->camView.z = mathSinf(inCam->camRot.y);
     
     inCam->camUp.x = (mathCosf(inCam->camRot.x) * mathSinf(inCam->camRot.z));
     inCam->camUp.y = (mathSinf(inCam->camRot.x) * mathSinf(inCam->camRot.z));
     inCam->camUp.z = mathCosf(inCam->camRot.z);
}

Camera* camCreate() {
     Camera* tempOut = memAlloc(sizeof(Camera));
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Couldn't create camera\nProbably out of memory.");
          #endif
          return NULL;
     }
     camReset(tempOut);
     return tempOut;
}

void camFree(Camera* inCam) {
     #if FL_MEMORY == 0
     if(!inCam)
          return;
     #endif
     memFree(inCam);
}

void camReset(Camera* inCam) {
     inCam->camPos = (vect3f){ 0.0f, 0.0f, 0.0f };
     inCam->camView = (vect3f){ 0.0f, 1.0f, 0.0f };
     inCam->camUp = (vect3f){ 0.0f, 0.0f, 1.0f };
     inCam->camRot = (vect3f){ 0.0f, 0.0f, 0.0f };
}

void camView(Camera* inCam) {
     if(!inCam)
          return;
          
     sceGumMatrixMode(GU_VIEW);
     sceGumLoadIdentity();
     
     camViewCalc(inCam);
     
     sceGumLookAt(&inCam->camPos, &(vect3f_Add(inCam->camPos, inCam->camView)), &inCam->camUp);
     sceGumUpdateMatrix();
     sceGumMatrixMode(GU_MODEL);
}

void camMove(Camera* inCam, vect3f inMove) {
     if(!inCam)
          return;
     inCam->camPos = vect3f_Add(inCam->camPos, inMove);
}

void camMoveTo(Camera* inCam, vect3f inTarget) {
     if(!inCam)
          return;
     vect3f tempMove = vect3f_Sub(inTarget, inCam->camPos);
     camMove(inCam, tempMove);
}

void camMoveForward(Camera* inCam, float inSize) {
     if(!inCam)
          return;
     camViewCalc(inCam);
     vect3f tempMove;
     tempMove.x = (inCam->camView.x * inSize);
     tempMove.y = (inCam->camView.y * inSize);
     tempMove.z = (inCam->camView.z * inSize);
     camMove(inCam, tempMove);
}

void camMoveStrafe(Camera* inCam, float inSize) {
     if(!inCam)
          return;
          
     camViewCalc(inCam);
     
     vect3f tempMove;
     
     tempMove.x = (mathCosf(inCam->camRot.x) * mathCosf(inCam->camRot.z)) + (mathSinf(inCam->camRot.y) * mathSinf(inCam->camRot.z));
     tempMove.y = (-mathSinf(inCam->camRot.x) * mathCosf(inCam->camRot.z)) + (mathSinf(inCam->camRot.x) * mathSinf(inCam->camRot.z));
     tempMove.z = (-mathSinf(inCam->camRot.z) * mathCosf(inCam->camRot.y));
     
     tempMove.x *= inSize;
     tempMove.y *= inSize;
     tempMove.z *= inSize;
     
     camMove(inCam, tempMove);
}

/*void camRotateYaw(Camera* inCam, float inAngle) {
     inCam->camRot.y += inAngle;
     //vect3f tempAxis = mathVect3fCrossProd(vect3f_Sub(inCam->camView, inCam->camPos), inCam->camUp);
     //mathVect3fNormalize(&tempAxis);
     //inCam->camView = mathVect3fRotate(inCam->camView, tempAxis, inAngle);
}

void camRotatePitch(Camera* inCam, float inAngle) {
     inCam->camRot.x += inAngle;
     //inCam->camView = mathVect3fRotate(inCam->camView, (vect3f){ 0.0f, 1.0f, 0.0f }, inAngle);
}

void camRotateRoll(Camera* inCam, float inAngle) {
     inCam->camRot.z += inAngle;
}*/

#endif
