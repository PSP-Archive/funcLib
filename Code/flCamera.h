// Funclib Camera v1.0.0
// 
// This module contains functions for manipulating the 3d view.
// It is basically an easy to use 3d camera class, with roll implemented too.
// 
// Contributor(s): Flatmush (Based off the timer class in the PSP NeHe tutorial ports)



#ifndef FLCAMERA_H
#define FLCAMERA_H

#include "flGlobal.h"

#if FL_CAMERA

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This structure holds all information about the cameras position and orientation.
 * @see camCreate(), vect3f
 */ 
typedef struct {
     /** A vect3f representing the position of the Camera in 3d space.
       * @note Defaults to (0, 0, 0).
       */
     vect3f camPos;
     /** A vect3f representing a point in 3d space that the camera is pointing towards.
       * @note Defaults to (0, 1, 0).
       */
     vect3f camView;
     /** A vect3f representing the direction of that is considered Up.
       * @note Defaults to (0, 0, 1).
       */
     vect3f camUp;
     /** A vect3f representing the yaw pitch and roll of the camera.
       * @note Defaults to (0, 0, 0).     
       */
     vect3f camRot;
} Camera;

/**
  * Create a valid initialized Camera struct.
  * @return An initialized Camera struct.
  * @see camFree()
  * @note This uses memAlloc() internally so it needs to be free'd using camFree() when finished.
  */
extern Camera* camCreate();

/**
  * Free a camera struct from memory.
  * @param inCam The camera struct to be free'd.
  * @see camCreate()
  * @note This uses memFree() internally.
  */
extern void    camFree(Camera* inCam);

/**
  * Reset the camera to its home position.
  * @param inCam The Camera to be reset.
  * @see camCreate()
  * @note This resets the camera so that it looks down the Y axis with Z being up, as does camCreate().
  */
extern void    camReset(Camera* inCam);

/**
  * Set the view matrix to the cameras view.
  * @param inCam The Camera to be viewed.
  * @note This should be called every frame that the camera is in use.
  */
extern void    camView(Camera* inCam);

/**
  * Move a Camera relative to it's current position.
  * @param inCam The Camera to be moved.
  * @param inMove A vector representing the amount to be moved in each dimension relative to it's current position.
  */
extern void    camMove(Camera* inCam, vect3f inMove);

/**
  * Move a Camera to a specific location.
  * @param inCam The Camera to be moved.
  * @param inTarget A vector representing the destination that the Camera will be moved to.
  */
extern void    camMoveTo(Camera* inCam, vect3f inTarget);

/**
  * Move a Camera forward in the direction it is pointing in.
  * @param inCam The Camera to be moved.
  * @param inSize The amount the camera should be moved forward.
  */
extern void    camMoveForward(Camera* inCam, float inSize);

/**
  * Move a Camera horizontally with respect to the direction it is pointing.
  * @param inCam The Camera to be moved.
  * @param inSize The amount the camera should be strafed right by.
  * @warning There is an error in this function which means it may not strafe correctly when the cameras roll isn't default.
  */
extern void    camMoveStrafe(Camera* inCam, float inSize);

/**
  * Rotate the cameras yaw.
  * @param inCam The Camera to be moved.
  * @param inAngle The amount the Cameras yaw should be modified by.
  * @warning This is a macro and as such is not type safe, inCam is expected to be a Camera* and inAngle a float.
  */
#define camRotateYaw(inCam, inAngle) ((inCam)->camRot.y += (inAngle))

/**
  * Rotate the cameras pitch.
  * @param inCam The Camera to be moved.
  * @param inAngle The amount the Cameras pitch should be modified by.
  * @warning This is a macro and as such is not type safe, inCam is expected to be a Camera* and inAngle a float.
  */
#define camRotatePitch(inCam, inAngle) ((inCam)->camRot.x += (inAngle))

/**
  * Rotate the cameras roll.
  * @param inCam The Camera to be moved.
  * @param inAngle The amount the Cameras roll should be modified by.
  * @warning This is a macro and as such is not type safe, inCam is expected to be a Camera* and inAngle a float.
  */
#define camRotateRoll(inCam, inAngle) ((inCam)->camRot.z += (inAngle))

#ifdef __cplusplus
}
#endif

#endif

#endif
