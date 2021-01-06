// Funclib Debug v1.0.0 (CORE)
// 
// This module contains functions for error reporting and logging
// Useful for debugging your programs and providing safe error reporting
// 
// Contributor(s): Flatmush (Time-stamping idea by Sleepy, help with CALLEDFROM macro help by Raphael and InsertWittyName)



#ifndef FLDEBUG_H
#define FLDEBUG_H

#include "flGlobal.h"

#if FL_DEBUG != 0

#ifdef __cplusplus
extern "C" {
#endif

extern bool debugEnabled;

extern void debugInit();
#if FL_DEBUG_CALLEDFROM != 0
extern void debugErrorFrom(char* inMessage, const char* inFile, const char* inFunc, int inLine);
extern void debugWarningFrom(char* inMessage, const char* inFile, const char* inFunc, int inLine);
extern void debugDevWarningFrom(char* inMessage, const char* inFile, const char* inFunc, int inLine);
extern void debugCriticalErrorFrom(char* inMessage, const char* inFile, const char* inFunc, int inLine);
extern void debugLogFrom(char* inMessage, char* inType, const char* inFile, const char* inFunc, int inLine);
extern void debugStatusLogFrom(char* inMessage, const char* inFile, const char* inFunc, int inLine);
#define debugError(inMessage) debugErrorFrom(inMessage, __FILE__, __FUNCTION__, __LINE__)
#define debugWarning(inMessage) debugWarningFrom(inMessage, __FILE__, __FUNCTION__, __LINE__)
#define debugDevWarning(inMessage) debugDevWarningFrom(inMessage, __FILE__, __FUNCTION__, __LINE__)
#define debugCriticalError(inMessage) debugCriticalErrorFrom(inMessage, __FILE__, __FUNCTION__, __LINE__)
#define debugStatusLog(inMessage) debugStatusLogFrom(inMessage, __FILE__, __FUNCTION__, __LINE__)
#else
extern void debugError(char* inMessage);
extern void debugWarning(char* inMessage);
extern void debugDevWarning(char* inMessage);
extern void debugCriticalError(char* inMessage);
extern void debugStatusLog(char* inMessage);
#endif

extern void debugLog(char* inMessage, char* inType);

#if (FL_GRAPHICS != 0) && (FL_TEXTURE != 0)
extern void debugScreenshot();
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
