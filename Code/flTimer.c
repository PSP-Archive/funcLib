#include "flGlobal.h"
#if FL_TIMER
#include <stdio.h>
#include <psprtc.h>
#include <pspkernel.h>

#if !FL_INCLUDE_ALL_C
#include "flTimer.h"
#include "flMemory.h"
#include "flDebug.h"
#endif

u64 timerPauseUntil = 0;

Timer* timerCreate() {
     Timer* tempTimer = (Timer*)memAlloc(sizeof(Timer));
     if(!tempTimer) {
          #if FL_DEBUG_ERROR
          debugError("Couldn't create timer.\nOut of memory.");
          #endif
          return NULL;
     }
     tempTimer->timerTimeNow = 0;
     sceRtcGetCurrentTick(&tempTimer->timerTimeLast);
     tempTimer->timerTickResolution = sceRtcGetTickResolution();
     return tempTimer;
}

double timerGetDeltaTime(Timer* inTimer) {
     if(!inTimer) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to get delta time from a NULL timer.");
          #endif
          return 0;
     }
     if(inTimer->timerTimeLast < timerPauseUntil)
          inTimer->timerTimeLast = timerPauseUntil;
     sceRtcGetCurrentTick(&inTimer->timerTimeNow);
     double tempDeltaTime = (inTimer->timerTimeNow - inTimer->timerTimeLast) / ((float)inTimer->timerTickResolution);
     inTimer->timerTimeLast = inTimer->timerTimeNow;
 
     return tempDeltaTime;
}

void timerFree(Timer* inTimer) {
     #if FL_MEMORY
     memFree(inTimer);
     #else
     if(inTimer)
          memFree(inTimer);
     #endif
}

void timerPauseAll() {
     sceRtcGetCurrentTick(&timerPauseUntil);
}

#endif
