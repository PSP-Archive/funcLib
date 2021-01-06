#include "flGlobal.h"
#if FL_DEBUG != 0
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspctrl.h>

#if FL_DEBUG_TIMESTAMP != 0
#include <psprtc.h>
#endif

#if FL_INCLUDE_ALL_C == 0
#include "flDebug.h"
#include "funcLib.h"
#include "flFile.h"

#if FL_GRAPHICS != 0
#include "flGraphics.h"
#if FL_TEXTURE != 0
#include "flTexture.h"
#endif
#endif

#if FL_GRAPHICS_SIMPLE != 0
#include "flGraphicsSimple.h"
#endif
#endif

bool debugEnabled = true;

void debugInit() {
     #if FL_DEBUG_LOGCLEAR != 0
     #if FL_FILE != 0
     fileDelete(FL_DEBUG_LOGPATH);
     #else
     sceIoRemove(FL_DEBUG_LOGPATH);
     #endif
     #endif
}

#if FL_DEBUG_CALLEDFROM != 0
void debugErrorFrom(char* inMessage, const char* inFile, const char* inFunc, int inLine) {
#else
void debugError(char* inMessage) {
#endif
     #if FL_DEBUG_ERROR == 0
     return;
     #endif
     if(!debugEnabled)
          return;
     debugEnabled = false;
     
     #if FL_DEBUG_LOGGING != 0
     #if FL_DEBUG_CALLEDFROM != 0
     debugLogFrom(inMessage, "ERROR", inFile, inFunc, inLine);
     #else
     debugLog(inMessage, "ERROR");
     #endif
     #endif
     
     #if (FL_GRAPHICS != 0) && (FL_TEXTURE != 0) && (FL_DEBUG_ERROR_SCREENSHOT != 0)
     debugScreenshot();
     #endif
     
     #if FL_DEBUG_ERROR_SHOW != 0
     SceCtrlData inputPad;
     int inputLock = 25;

     #if FL_GRAPHICS != 0
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     #endif
     #if FL_GRAPHICS_SIMPLE != 0
     pspDebugScreenSetOffset((int)graphicsVRamDrawBuffer());
     #endif
     pspDebugScreenClear ();
     pspDebugScreenSetTextColor(FL_DEBUG_ERROR_COLOR);
     pspDebugScreenPrintf("ERROR!\n\n");
     #if FL_DEBUG_CALLEDFROM != 0
     pspDebugScreenPrintf("%s:%s(@%i)> ", inFile, inFunc, inLine);
     #endif
     pspDebugScreenPrintf(inMessage);
     pspDebugScreenSetTextColor(0xFFFFFFFF);
     pspDebugScreenPrintf("\n\nPress [X] to continue or [O] to quit.\n");
     #if FL_GRAPHICS != 0
     sceDisplayWaitVblankStart();
     graphicsDispBuffer = graphicsDrawBuffer;
     graphicsDrawBuffer = sceGuSwapBuffers();
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     pspDebugScreenSetXY(0, 0);
     #endif
     #if FL_GRAPHICS_SIMPLE != 0
     sceDisplayWaitVblankStart();
     graphicsSwapBuffers();
     #endif
     
     while(1) {
          if(inputLock > 0) {
               inputLock--;
          } else {
               sceCtrlReadBufferPositive(&inputPad, 1);
               if(inputPad.Buttons & PSP_CTRL_CROSS) {
                    break;
               } else if(inputPad.Buttons & PSP_CTRL_CIRCLE) {
                    flTerminate();
               }
          }     
     }
     #endif
     debugEnabled = true;
}

#if FL_DEBUG_CALLEDFROM != 0
void debugWarningFrom(char* inMessage, const char* inFile, const char* inFunc, int inLine) {
#else
void debugWarning(char* inMessage) {
#endif
     #if FL_DEBUG_WARNING == 0
     return;
     #endif
     if(!debugEnabled)
          return;
     debugEnabled = false;
     
     #if FL_DEBUG_LOGGING != 0
     #if FL_DEBUG_CALLEDFROM != 0
     debugLogFrom(inMessage, "WARNING", inFile, inFunc, inLine);
     #else
     debugLog(inMessage, "WARNING");
     #endif
     #endif

     #if (GRAPHICS != 0) && (TEXTURE != 0) && (DEBUG_WARNING_SCREENSHOT != 0)
     debugScreenshot();
     #endif

     #if FL_DEBUG_WARNING_SHOW != 0
     SceCtrlData inputPad;
     int inputLock = 25;

     #if FL_GRAPHICS != 0
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     #endif
     #if FL_GRAPHICS_SIMPLE != 0
     pspDebugScreenSetOffset((int)graphicsVRamDrawBuffer());
     #endif
     pspDebugScreenClear ();
     pspDebugScreenSetTextColor(FL_DEBUG_WARNING_COLOR);
     pspDebugScreenPrintf("WARNING!\n\n");
     #if FL_DEBUG_CALLEDFROM != 0
     pspDebugScreenPrintf("%s:%s(@%i)> ", inFile, inFunc, inLine);
     #endif
     pspDebugScreenPrintf(inMessage);
     pspDebugScreenSetTextColor(0xFFFFFFFF);
     pspDebugScreenPrintf("\n\nPress [X] to continue.\n");
     #if FL_GRAPHICS != 0
     sceDisplayWaitVblankStart();
     graphicsDispBuffer = graphicsDrawBuffer;
     graphicsDrawBuffer = sceGuSwapBuffers();
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     pspDebugScreenSetXY(0, 0);
     #endif
     #if FL_GRAPHICS_SIMPLE != 0
     sceDisplayWaitVblankStart();
     graphicsSwapBuffers();
     #endif
     
     while(1) {
          if(inputLock > 0) {
               inputLock--;
          } else {
               sceCtrlReadBufferPositive(&inputPad, 1);
               if(inputPad.Buttons & PSP_CTRL_CROSS) {
                    break;
               }
          }     
     }
     #endif
     debugEnabled = true;
}

#if FL_DEBUG_CALLEDFROM != 0
void debugDevWarningFrom(char* inMessage, const char* inFile, const char* inFunc, int inLine) {
#else
void debugDevWarning(char* inMessage) {
#endif
     #if FL_DEBUG_DEVWARNING == 0
     return;
     #endif
     if(!debugEnabled)
          return;
     debugEnabled = false;
     
     #if FL_DEBUG_LOGGING != 0
     #if FL_DEBUG_CALLEDFROM != 0
     debugLogFrom(inMessage, "DEV-WARNING", inFile, inFunc, inLine);
     #else
     debugLog(inMessage, "DEV-WARNING");
     #endif
     #endif

     #if (GRAPHICS != 0) && (TEXTURE != 0) && (DEBUG_DEVWARNING_SCREENSHOT != 0)
     debugScreenshot();
     #endif

     #if FL_DEBUG_DEVWARNING_SHOW != 0
     SceCtrlData inputPad;
     int inputLock = 25;

     #if FL_GRAPHICS != 0
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     #endif
     #if FL_GRAPHICS_SIMPLE != 0
     pspDebugScreenSetOffset((int)graphicsVRamDrawBuffer());
     #endif
     pspDebugScreenClear ();
     pspDebugScreenSetTextColor(FL_DEBUG_DEVWARNING_COLOR);
     pspDebugScreenPrintf("(i) DEV WARNING.\n\n");
     #if FL_DEBUG_CALLEDFROM != 0
     pspDebugScreenPrintf("%s:%s(@%i)> ", inFile, inFunc, inLine);
     #endif
     pspDebugScreenPrintf(inMessage);
     pspDebugScreenSetTextColor(0xFFFFFFFF);
     pspDebugScreenPrintf("\n\nPress [X] to continue.\n");
     #if FL_GRAPHICS != 0
     sceDisplayWaitVblankStart();
     graphicsDispBuffer = graphicsDrawBuffer;
     graphicsDrawBuffer = sceGuSwapBuffers();
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     pspDebugScreenSetXY(0, 0);
     #endif
     #if FL_GRAPHICS_SIMPLE != 0
     sceDisplayWaitVblankStart();
     graphicsSwapBuffers();
     #endif
     
     while(1) {
          if(inputLock > 0) {
               inputLock--;
          } else {
               sceCtrlReadBufferPositive(&inputPad, 1);
               if(inputPad.Buttons & PSP_CTRL_CROSS) {
                    break;
               }
          }     
     }
     #endif
     debugEnabled = true;
}

#if FL_DEBUG_CALLEDFROM != 0
void debugCriticalErrorFrom(char* inMessage, const char* inFile, const char* inFunc, int inLine) {
#else
void debugCriticalError(char* inMessage) {
#endif
     if(!debugEnabled) {
          flTerminate();
          return;
     }
     debugEnabled = false;

     SceCtrlData inputPad;
     int inputLock = 25;

     #if FL_DEBUG_LOGGING != 0
     #if FL_DEBUG_CALLEDFROM != 0
     debugLogFrom(inMessage, "CRITICAL-ERROR", inFile, inFunc, inLine);
     #else
     debugLog(inMessage, "CRITICAL-ERROR");
     #endif
     #endif
     
     #if (FL_GRAPHICS != 0) && (FL_TEXTURE != 0) && (FL_DEBUG_CRITICALERROR_SCREENSHOT != 0)
     debugScreenshot();
     #endif
     
     #if FL_DEBUG_CRITICALERROR_SHOW != 0
     flTerminate();
     return;
     #endif

     #if FL_GRAPHICS != 0
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     #endif
     #if FL_GRAPHICS_SIMPLE != 0
     pspDebugScreenSetOffset((int)graphicsVRamDrawBuffer());
     #endif
     pspDebugScreenClear ();
     pspDebugScreenSetTextColor(FL_DEBUG_CRITICALERROR_COLOR);
     pspDebugScreenPrintf("CRITICAL ERROR!\n\n");
     #if FL_DEBUG_CALLEDFROM != 0
     pspDebugScreenPrintf("%s:%s(@%i)> ", inFile, inFunc, inLine);
     #endif
     pspDebugScreenPrintf(inMessage);
     pspDebugScreenSetTextColor(0xFFFFFFFF);
     pspDebugScreenPrintf("\n\nPress [X] or [O] to quit.\n");
     #if FL_GRAPHICS != 0
     sceDisplayWaitVblankStart();
     graphicsDispBuffer = graphicsDrawBuffer;
     graphicsDrawBuffer = sceGuSwapBuffers();
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     pspDebugScreenSetXY(0, 0);
     #endif
     #if GRAPHICS_SIMPLE != 0
     sceDisplayWaitVblankStart();
     graphicsSwapBuffers();
     #endif
     
     while(1) {
          if(inputLock > 0) {
               inputLock--;
          } else {
               sceCtrlReadBufferPositive(&inputPad, 1);
               if((inputPad.Buttons & PSP_CTRL_CROSS) || (inputPad.Buttons & PSP_CTRL_CIRCLE)) {
                    flTerminate();
               }
          }     
     }
}

#if FL_DEBUG_CALLEDFROM != 0
void debugLogFrom(char* inMessage, char* inType, const char* inFile, const char* inFunc, int inLine) {
     #if FL_FILE != 0
     File* tempFile = fileOpen(FL_DEBUG_LOGPATH, FILE_MODE_APPEND);
     #else
     FILE* tempFile = fopen(FL_DEBUG_LOGPATH, "a");
     #endif
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          debugError("Cannot open log file.");
          #endif
          return;
     }
     char tempString[256];
     #if FL_DEBUG_TIMESTAMP != 0
     pspTime tempTime;
     sceRtcGetCurrentClockLocalTime(&tempTime);
     #if FL_DEBUG_DATEFORMAT_AMERICAN != 0
     sprintf(tempString, "[%02i/%02i/%02i - %02i:%02i:%02i] ", tempTime.month, tempTime.day, tempTime.year, tempTime.hour, tempTime.minutes, tempTime.seconds);
     #else
     sprintf(tempString, "[%02i/%02i/%02i - %02i:%02i:%02i] ", tempTime.day, tempTime.month, tempTime.year, tempTime.hour, tempTime.minutes, tempTime.seconds);
     #endif
     filePuts(tempString, tempFile);
     #endif
     
     filePuts("[", tempFile);
     filePuts(inType, tempFile);
     filePuts("] ", tempFile);
     
     sprintf(tempString, "%s:%s(@%i)> ", inFile, inFunc, inLine);
     filePuts(tempString, tempFile);
     filePuts(inMessage, tempFile);
     filePuts("\r\n", tempFile);
     fileClose(tempFile);
}
#endif

void debugLog(char* inMessage, char* inType) {
     #if FL_FILE != 0
     File* tempFile = fileOpen(FL_DEBUG_LOGPATH, FILE_MODE_APPEND);
     #else
     FILE* tempFile = fopen(FL_DEBUG_LOGPATH, "a");
     #endif
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          debugError("Cannot open log file.");
          #endif
          return;
     }
     #if FL_DEBUG_TIMESTAMP != 0
     char tempString[256];
     pspTime tempTime;
     sceRtcGetCurrentClockLocalTime(&tempTime);
     #if FL_DEBUG_DATEFORMAT_AMERICAN != 0
     sprintf(tempString, "[%02i/%02i/%04i - %02i:%02i:%02i] ", tempTime.month, tempTime.day, tempTime.year, tempTime.hour, tempTime.minutes, tempTime.seconds);
     #else
     sprintf(tempString, "[%02i/%02i/%04i - %02i:%02i:%02i] ", tempTime.day, tempTime.month, tempTime.year, tempTime.hour, tempTime.minutes, tempTime.seconds);
     #endif
     filePuts(tempString, tempFile);
     #endif
     
     filePuts("[", tempFile);
     filePuts(inType, tempFile);
     filePuts("] ", tempFile);
     
     filePuts(inMessage, tempFile);
     filePuts("\r\n", tempFile);
     fileClose(tempFile);
}

#if FL_DEBUG_CALLEDFROM != 0
void debugStatusLogFrom(char* inMessage, const char* inFile, const char* inFunc, int inLine) {
#else
void debugStatusLog(char* inMessage) {
#endif
     #if FL_DEBUG_CALLEDFROM != 0
     debugLogFrom(inMessage, "STATUS", inFile, inFunc, inLine);
     #else
     debugLog(inMessage, "STATUS");
     #endif
}

#if (FL_GRAPHICS != 0) && (FL_TEXTURE != 0)
void debugScreenshot() {
     Texture* tempScreen = texScreenshot();
     if(!tempScreen) {
          #if DEBUG_ERROR != 0
          debugError("Cannot retrieve error screenshot.");
          #endif
          return;
     }
     char tempPath[128];
     int tempShotNumber = 0;
     sprintf(tempPath, FL_DEBUG_SCREENSHOTPATH, tempShotNumber);
     #if FL_FILE != 0
     File* tempFile = fileOpen(tempPath, FILE_MODE_READ | FILE_MODE_BINARY);
     #else
     FILE* tempFile = fopen(tempPath, "rb");
     #endif
     while(tempFile) {
          fileClose(tempFile);
          tempShotNumber++;
          sprintf(tempPath, FL_DEBUG_SCREENSHOTPATH, tempShotNumber);
          #if FL_FILE != 0
          tempFile = fileOpen(tempPath, FILE_MODE_READ | FILE_MODE_BINARY);
          #else
          tempFile = fopen(tempPath, "rb");
          #endif
     }
     if(!texSave(tempScreen, tempPath)) {
          #if DEBUG_ERROR != 0
          debugError("Cannot save error screenshot.");
          #endif
          return;
     }
     #if FL_DEBUG_LOGVERBOSE != 0
     char tempString[256];
     sprintf(tempString, "Saved screenshot: %s", tempPath);
     debugStatusLog(tempString);
     #endif
}
#endif

#endif

