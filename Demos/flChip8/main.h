#include "././Code/funcLib.h"

#ifndef MAIN_H
#define MAIN_H

extern Timer*        emuTimer;
extern Font*         emuFont;

extern float         emuTick;
extern float         emuTickTimer;

extern unsigned char emuKeyMap[16];

extern int           main();

extern bool          emuShowDebug;

#endif
