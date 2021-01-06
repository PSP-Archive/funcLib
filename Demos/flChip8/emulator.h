#include "././Code/funcLib.h"

#ifndef EMULATOR_H
#define EMULATOR_H

extern unsigned char  emuRegisterV[16];
extern unsigned short emuRegisterI;
extern unsigned short emuRegisterPC;
extern unsigned short emuRegisterSP;
extern unsigned char  emuMemory[4096];

extern unsigned char  emuTimerDelay;
extern unsigned char  emuTimerSound;
extern float          emuTimerFrequency;

extern unsigned short emuKeyboard;
extern unsigned char  emuKeyboardWaiting;

extern unsigned char  emuDisplay[8][32];
extern unsigned char  emuDisplayScaleX;
extern unsigned char  emuDisplayScaleY;

extern float          emuFrequency;

extern void emuDebugInfo();
extern bool emuKeyboardDown(unsigned char inKey);
extern void emuInputButtonUp(unsigned char inKey);
extern void emuInputButtonDown(unsigned char inKey) ;
extern void emuDisplayClear();
extern bool emuDisplayPlot(unsigned int inX, unsigned int inY);
extern bool emuDisplayPlotLine(unsigned int inX, unsigned int inY, unsigned char inLine);
extern void emuDisplayRender();
extern void emuDisplayRenderPixel(unsigned int inX, unsigned int inY);
extern void emuMemoryClear();
extern void emuTimerTick();
extern void emuExecSingle();
extern void emuExec(int inNum);
extern void emuProgramLoad(char* inPath);
extern void emuReset();

#endif
