// Funclib Notify v1.0.0 (CORE)
// 
// This module contains functions for notifyin the user about the psp's status
// It shows simple volume and battery guages.
// 
// Contributor(s): Flatmush (Based off the timer class in the PSP NeHe tutorial ports)



#ifndef FLNOTIFY_H
#define FLNOTIFY_H

#include "flGlobal.h"

#if FL_NOTIFY != 0

#ifdef __cplusplus
extern "C" {
#endif

#define NOTIFY_VISIBLE_NEVER  0
#define NOTIFY_VISIBLE_AUTO   1
#define NOTIFY_VISIBLE_ALWAYS 2

#if FL_NOTIFY_TRAYSIZE > 0
typedef struct {
     char  ntiMessage[248];
     u32   ntiColor;
     float ntiTime;
} NotifyTrayItem;
#endif

extern u32    ntfyTickCount;
extern bool   ntfyInitialized;
extern u8     ntfyVisible;

extern void ntfyInit();
extern void ntfyTick();
extern void ntfyShowAll(float inTime);

#if FL_KERNELMODE
extern void ntfyCheckVolume();
#endif
extern void ntfyCheckBattery();
extern void ntfyCheckTime();

#if FL_KERNELMODE
extern void ntfyShowVolume();
#endif
extern void ntfyShowBattery();
extern void ntfyShowTime();
extern void ntfyShowDate();

#if FL_NOTIFY_TRAYSIZE
extern void ntfyTrayAdd(char* inMessage, u32 inColor);
extern void ntfyTrayTick();
extern void ntfyTrayShow();
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
