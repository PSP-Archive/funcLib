// Funclib Input v1.0.0 (CORE)
// 
// This module contains functions for input polling.
// It adds button states to make input handling easier.
// 
// Contributor(s): Flatmush



#ifndef FLINPUT_H
#define FLINPUT_H

#include "flGlobal.h"

#if FL_INPUT

#ifdef __cplusplus
extern "C" {
#endif



#define INPUT_STILL_UP   0 ///< This button state represents a button that is not pressed and wasn't pressed last frame either.
#define INPUT_JUST_UP    2 ///< This button state represents a button that has just been released.
#define INPUT_STILL_DOWN 1 ///< This button state represents a button that is down and was down last frame too.
#define INPUT_JUST_DOWN  3 ///< This button state represents a button that has just been pressed.

#define INPUT_BUTTON_SELECT   0 ///< This is the button number representing the SELECT button.
#define INPUT_BUTTON_START    3 ///< This is the button number representing the START button.
#define INPUT_DPAD_UP         4 ///< This is the button number representing the DPAD UP button.
#define INPUT_DPAD_RIGHT      5 ///< This is the button number representing the DPAD RIGHT button.
#define INPUT_DPAD_DOWN       6 ///< This is the button number representing the DPAD DOWN button.
#define INPUT_DPAD_LEFT       7 ///< This is the button number representing the DPAD LEFT button.
#define INPUT_BUTTON_LTRIGGER 8 ///< This is the button number representing the LEFT TRIGGER button.
#define INPUT_BUTTON_RTRIGGER 9 ///< This is the button number representing the RIGHT TRIGGER button.
#define INPUT_BUTTON_TRIANGLE 12 ///< This is the button number representing the TRIANGLE button.
#define INPUT_BUTTON_CIRCLE   13 ///< This is the button number representing the CIRCLE button.
#define INPUT_BUTTON_CROSS    14 ///< This is the button number representing the CROSS button.
#define INPUT_BUTTON_SQUARE   15 ///< This is the button number representing the SQUARE button.

#define INPUT_BUTTON_HOME     16 ///< This is the button number representing the HOME button, this requires KERNEL mode to poll.
#define INPUT_BUTTON_HOLD     17 ///< This is the button number representing the POWER HOLD slider, this requires KERNEL mode to poll.
#define INPUT_BUTTON_NOTE     18 ///< This is the button number representing the NOTE/MUSIC button, this requires KERNEL mode to poll.
#define INPUT_BUTTON_SCREEN   19 ///< This is the button number representing the SCREEN button, this requires KERNEL mode to poll.
#define INPUT_BUTTON_VOLUP    20 ///< This is the button number representing the VOLUME UP button, this requires KERNEL mode to poll.
#define INPUT_BUTTON_VOLDOWN  21 ///< This is the button number representing the VOLUME DOWN button, this requires KERNEL mode to poll.
#define INPUT_BUTTON_WLANUP   22 ///< This is the button number representing the WLAN slider, this requires KERNEL mode to poll.
#define INPUT_BUTTON_REMOTE   23 ///< This is the button number representing the HEADPHONE REMOTE button, this requires KERNEL mode to poll.
#define INPUT_BUTTON_DISC     24 ///< This is the button number representing the DISC status, this requires KERNEL mode to poll.
#define INPUT_BUTTON_MS       25 ///< This is the button number representing the MEMORY STICK status, this requires KERNEL mode to poll.



/**
  * Initializes the input system, this is called by flInitialize().
  * @see flInitialize()
  */
extern void       inputInit();

/**
  * This returns the current state of a button.
  * @param inInput The number of the button whose state will be checked.
  * @return The state of the button.
  */
extern u8         inputCheck(u8 inInput);

/**
  * This returns a non-zero value if the button supplied by inInput is currently pressed.
  * @param inInput The number of the button whose to be checked.
  * @return Non-zero if the button is pressed.
  */
extern bool       inputDown(u8 inInput);

/**
  * This updates the input system, and should be called at least once every frame in which input is required.
  */
extern void       inputPoll();

/**
  * This locks the input system for a small period so that button presses are ignored.
  * @param inTime The amount of time in seconds that the system will be locked for.
  */
extern void       inputLock(float inTime);

/**
  * This function allows you to define a button, a state and a function to call,
  * so that you can register a pause event, in retrospect callbacks would possibly be better.
  * @param inInput The number of the button that will be used for pausing.
  * @param inState The state the button must be in for inFunc() to be called.
  * @param inFunc A pointer to the pause function to be used.
  * @warning This function doesn't seem to work, although is pretty much untested.
  */
extern void       inputPauseFunc(u8 inInput, u8 inState, void (*inFunc)());

#ifdef __cplusplus
}
#endif

#endif

#endif
