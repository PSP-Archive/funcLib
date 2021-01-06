// Funclib Timer v1.0.0 (CORE)
// 
// This module contains functions for timing
// It is a simple interface for the psp's realtime-clock
// 
// Contributor(s): Flatmush (Based off the timer class in the PSP NeHe tutorial ports)



#ifndef FLTIMER_H
#define FLTIMER_H

#include "flGlobal.h"

#if FL_TIMER != 0

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
     u64 timerTimeNow;
     u64 timerTimeLast;
     u32 timerTickResolution;
} Timer;

extern Timer* timerCreate();
extern double timerGetDeltaTime(Timer* inTimer);
extern void   timerFree(Timer* inTimer);
extern void   timerPauseAll();

#ifdef __cplusplus
}
#endif

#endif

#define timerWait(inTime) sceKernelDelayThread(inTime * 1000000)

#endif
