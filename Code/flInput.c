#include "flGlobal.h"
#if FL_INPUT != 0
#include <stdio.h>
#include <stdlib.h>
#include <pspctrl.h>

#if FL_INCLUDE_ALL_C == 0
#include "flInput.h"

#if FL_TIMER != 0
#include "flTimer.h"
#endif
#endif

u8          inputStates[32];
SceCtrlData inputData[2];

#if FL_TIMER != 0
Timer* inputTimer;
#endif
float  inputLockTime = 0;

void       (*inputPause)() = NULL;
u8         inputPauseButton = 0;
u8         inputPauseState = 0;

void inputInit() {
     sceCtrlReadBufferPositive(&inputData[0], 1);
     #if FL_TIMER != 0
     inputTimer = timerCreate();
     #endif
}

u8 inputCheck(u8 inInput) {
     if(inputLockTime > 0)
          return 0;
     return inputStates[inInput];
}

bool inputDown(u8 inInput) {
     if(inputLockTime > 0)
          return 0;
     return (inputStates[inInput] & 1);
}

void inputPoll() {
     if(inputLockTime > 0.0f) {
          #if FL_TIMER != 0
          inputLockTime -= timerGetDeltaTime(inputTimer);
          #else
          inputLockTime--;
          #endif
          if(inputLockTime < 0.0f)
               inputLockTime = 0.0f;
     } else {
          timerGetDeltaTime(inputTimer);
     }
     inputData[0] = inputData[1];
     sceCtrlPeekBufferPositive(&inputData[1], 1);
     int i;
     for(i = 0; i < 32; i++) {
          if(inputData[1].Buttons & (1 << i))
               inputStates[i] = 1;
          else
               inputStates[i] = 0;
          if((inputData[0].Buttons & (1 << i)) != (inputData[1].Buttons & (1 << i)))
               inputStates[i] |= 2;
     }
     if(inputPause && (inputLockTime <= 0.0f)) {
          if(inputStates[inputPauseButton] == inputPauseState) {
               (*inputPause)();
               inputLock(0.0625);
          }
     }
}

void inputLock(float inTime) {
     if(inputLockTime == 0)
          timerGetDeltaTime(inputTimer);
     inputLockTime += inTime;
}

void inputPauseFunc(u8 inInput, u8 inState, void (*inFunc)()) {
     inputPause = inFunc;
     inputPauseButton = inInput;
     inputPauseState = (inState & 3);
}

#endif
