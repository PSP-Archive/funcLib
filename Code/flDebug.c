#include "flGlobal.h"
#if FL_DEBUG != 0
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspctrl.h>
#include <string.h>
#include <stdio.h>

#if FL_DEBUG_TIMESTAMP != 0
#include <psprtc.h>
#endif

#if FL_INCLUDE_ALL_C == 0
#include "flDebug.h"
#include "funcLib.h"
#include "flFile.h"
#include "flGraphics.h"
#include "flTexture.h"
#include "flNotify.h"
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

#if FL_DEBUG_ERROR != 0
#if FL_DEBUG_CALLEDFROM != 0
void debugErrorFrom(const char* inFile, const char* inFunc, int inLine, char* inMessage, ...) {
#else
void debugError(char* inMessage, ...) {
#endif
     if(!debugEnabled)
          return;
     debugEnabled = false;
     
     u32 tempLen = strlen(inMessage);
     tempLen += max(DEBUG_ARGEXTEND, (tempLen >> 1));
     char* tempMessage = memQalloc(tempLen);
     
     va_list tempArgList;
     va_start(tempArgList, inMessage);     
     vsprintf(tempMessage, inMessage, tempArgList);
     va_end(tempArgList);
     
     #if FL_DEBUG_LOGGING != 0
     #if FL_DEBUG_CALLEDFROM != 0
     debugLogFrom("ERROR", inFile, inFunc, inLine, tempMessage);
     #else
     debugLog("ERROR", tempMessage);
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
     pspDebugScreenClear ();
     pspDebugScreenSetTextColor(FL_DEBUG_ERROR_COLOR);
     pspDebugScreenPrintf("ERROR!\n\n");
     #if FL_DEBUG_CALLEDFROM != 0
     pspDebugScreenPrintf("%s:%s(@%i)> ", inFile, inFunc, inLine);
     #endif
     pspDebugScreenPrintf(tempMessage);
     pspDebugScreenSetTextColor(0xFFFFFFFF);
     pspDebugScreenPrintf("\n\nPress [X] to continue or [O] to quit.\n");
     #if FL_GRAPHICS != 0
     sceDisplayWaitVblankStart();
     graphicsDispBuffer = graphicsDrawBuffer;
     graphicsDrawBuffer = sceGuSwapBuffers();
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     pspDebugScreenSetXY(0, 0);
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
     #else
     #if FL_DEBUG_ERROR_NOTIFY
     ntfyTrayAdd(tempMessage, FL_DEBUG_ERROR_COLOR);
     #endif
     #endif
     
     memFree(tempMessage);
     
     debugEnabled = true;
}
#endif

#if FL_DEBUG_WARNING != 0
#if FL_DEBUG_CALLEDFROM != 0
void debugWarningFrom(const char* inFile, const char* inFunc, int inLine, char* inMessage, ...) {
#else
void debugWarning(char* inMessage, ...) {
#endif
     if(!debugEnabled)
          return;
     debugEnabled = false;
     
     u32 tempLen = strlen(inMessage);
     tempLen += max(DEBUG_ARGEXTEND, (tempLen >> 1));
     char* tempMessage = memQalloc(tempLen);
     
     va_list tempArgList;
     va_start(tempArgList, inMessage);     
     vsprintf(tempMessage, inMessage, tempArgList);
     va_end(tempArgList);
     
     #if FL_DEBUG_LOGGING != 0
     #if FL_DEBUG_CALLEDFROM != 0
     debugLogFrom("WARNING", inFile, inFunc, inLine, tempMessage);
     #else
     debugLog("WARNING", tempMessage);
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
     pspDebugScreenClear ();
     pspDebugScreenSetTextColor(FL_DEBUG_WARNING_COLOR);
     pspDebugScreenPrintf("WARNING!\n\n");
     #if FL_DEBUG_CALLEDFROM != 0
     pspDebugScreenPrintf("%s:%s(@%i)> ", inFile, inFunc, inLine);
     #endif
     pspDebugScreenPrintf(tempMessage);
     pspDebugScreenSetTextColor(0xFFFFFFFF);
     pspDebugScreenPrintf("\n\nPress [X] to continue.\n");
     #if FL_GRAPHICS != 0
     sceDisplayWaitVblankStart();
     graphicsDispBuffer = graphicsDrawBuffer;
     graphicsDrawBuffer = sceGuSwapBuffers();
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     pspDebugScreenSetXY(0, 0);
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
     #else
     #if FL_DEBUG_WARNING_NOTIFY
     ntfyTrayAdd(tempMessage, FL_DEBUG_WARNING_COLOR);
     #endif
     #endif
     
     memFree(tempMessage);
     
     debugEnabled = true;
}
#endif

#if FL_DEBUG_DEVWARNING != 0
#if FL_DEBUG_CALLEDFROM != 0
void debugDevWarningFrom(const char* inFile, const char* inFunc, int inLine, char* inMessage, ...) {
#else
void debugDevWarning(char* inMessage, ...) {
#endif
     if(!debugEnabled)
          return;
     debugEnabled = false;
     
     u32 tempLen = strlen(inMessage);
     tempLen += max(DEBUG_ARGEXTEND, (tempLen >> 1));
     char* tempMessage = memQalloc(tempLen);
     
     va_list tempArgList;
     va_start(tempArgList, inMessage);     
     vsprintf(tempMessage, inMessage, tempArgList);
     va_end(tempArgList);
     
     #if FL_DEBUG_LOGGING != 0
     #if FL_DEBUG_CALLEDFROM != 0
     debugLogFrom("DEV-WARNING", inFile, inFunc, inLine, tempMessage);
     #else
     debugLog("DEV-WARNING", tempMessage);
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
     pspDebugScreenClear ();
     pspDebugScreenSetTextColor(FL_DEBUG_DEVWARNING_COLOR);
     pspDebugScreenPrintf("(i) DEV WARNING.\n\n");
     #if FL_DEBUG_CALLEDFROM != 0
     pspDebugScreenPrintf("%s:%s(@%i)> ", inFile, inFunc, inLine);
     #endif
     pspDebugScreenPrintf(tempMessage);
     pspDebugScreenSetTextColor(0xFFFFFFFF);
     pspDebugScreenPrintf("\n\nPress [X] to continue.\n");
     #if FL_GRAPHICS != 0
     sceDisplayWaitVblankStart();
     graphicsDispBuffer = graphicsDrawBuffer;
     graphicsDrawBuffer = sceGuSwapBuffers();
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     pspDebugScreenSetXY(0, 0);
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
     #else
     #if FL_DEBUG_DEVWARNING_NOTIFY
     ntfyTrayAdd(tempMessage, FL_DEBUG_DEVWARNING_COLOR);
     #endif
     #endif
     
     memFree(tempMessage);
     
     debugEnabled = true;
}
#endif

#if FL_DEBUG_CALLEDFROM != 0
void debugCriticalErrorFrom(const char* inFile, const char* inFunc, int inLine, char* inMessage, ...) {
#else
void debugCriticalError(char* inMessage, ...) {
#endif
     if(!debugEnabled) {
          flTerminate();
          return;
     }
     debugEnabled = false;

     u32 tempLen = strlen(inMessage);
     tempLen += max(DEBUG_ARGEXTEND, (tempLen >> 1));
     char* tempMessage = memQalloc(tempLen);
     
     va_list tempArgList;
     va_start(tempArgList, inMessage);     
     vsprintf(tempMessage, inMessage, tempArgList);
     va_end(tempArgList);

     SceCtrlData inputPad;
     int inputLock = 25;

     #if FL_DEBUG_LOGGING != 0
     #if FL_DEBUG_CALLEDFROM != 0
     debugLogFrom("CRITICAL-ERROR", inFile, inFunc, inLine, tempMessage);
     #else
     debugLog("CRITICAL-ERROR", tempMessage);
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
     pspDebugScreenClear ();
     pspDebugScreenSetTextColor(FL_DEBUG_CRITICALERROR_COLOR);
     pspDebugScreenPrintf("CRITICAL ERROR!\n\n");
     #if FL_DEBUG_CALLEDFROM != 0
     pspDebugScreenPrintf("%s:%s(@%i)> ", inFile, inFunc, inLine);
     #endif
     pspDebugScreenPrintf(tempMessage);
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
void debugLogFrom(char* inType, const char* inFile, const char* inFunc, int inLine, char* inMessage, ...) {
     u32 tempLen = strlen(inMessage);
     tempLen += max(DEBUG_ARGEXTEND, (tempLen >> 1));
     char* tempMessage = memQalloc(tempLen);
     
     va_list tempArgList;
     va_start(tempArgList, inMessage);     
     vsprintf(tempMessage, inMessage, tempArgList);
     va_end(tempArgList);
     
     #if FL_FILE != 0
     File* tempFile = fileOpen(FL_DEBUG_LOGPATH, FILE_MODE_APPEND);
     #else
     FILE* tempFile = fopen(FL_DEBUG_LOGPATH, "a");
     #endif
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          debugError("Cannot open log file.");
          #endif
          memFree(tempMessage);
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
     filePuts(tempMessage, tempFile);
     filePuts("\r\n", tempFile);
     fileClose(tempFile);
     
     memFree(tempMessage);
}
#endif

void debugLog(char* inType, char* inMessage, ...) {
     u32 tempLen = strlen(inMessage);
     tempLen += max(DEBUG_ARGEXTEND, (tempLen >> 1));
     char* tempMessage = memQalloc(tempLen);
     
     va_list tempArgList;
     va_start(tempArgList, inMessage);     
     vsprintf(tempMessage, inMessage, tempArgList);
     va_end(tempArgList);
     
     #if FL_FILE != 0
     File* tempFile = fileOpen(FL_DEBUG_LOGPATH, FILE_MODE_APPEND);
     #else
     FILE* tempFile = fopen(FL_DEBUG_LOGPATH, "a");
     #endif
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          debugError("Cannot open log file.");
          #endif
          memFree(tempMessage);
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
     
     filePuts(tempMessage, tempFile);
     filePuts("\r\n", tempFile);
     fileClose(tempFile);
     
     memFree(tempMessage);
}

#if FL_DEBUG_CALLEDFROM != 0
void debugStatusLogFrom(const char* inFile, const char* inFunc, int inLine, char* inMessage, ...) {
#else
void debugStatusLog(char* inMessage, ...) {
#endif
     u32 tempLen = strlen(inMessage);
     tempLen += max(DEBUG_ARGEXTEND, (tempLen >> 1));
     char* tempMessage = memQalloc(tempLen);
     
     va_list tempArgList;
     va_start(tempArgList, inMessage);     
     vsprintf(tempMessage, inMessage, tempArgList);
     va_end(tempArgList);

     #if FL_DEBUG_CALLEDFROM != 0
     debugLogFrom("STATUS", inFile, inFunc, inLine, tempMessage);
     #else
     debugLog("STATUS", tempMessage);
     #endif
     
     #if FL_DEBUG_STATUS_NOTIFY
     ntfyTrayAdd(tempMessage, FL_DEBUG_STATUS_COLOR);
     #endif
     
     memFree(tempMessage);
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
     
     #if FL_DEBUG_STATUS != 0
     debugStatusLog("Saved screenshot: %s", tempPath);
     #endif
}
#endif

#endif

