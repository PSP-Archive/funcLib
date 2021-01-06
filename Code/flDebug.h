// Funclib Debug v1.0.0 (CORE)
// 
// This module contains functions for error reporting and logging
// Useful for debugging your programs and providing safe error reporting
// 
// Contributor(s): Flatmush (Time-stamping idea by Sleepy, help with CALLEDFROM macro help by Raphael and InsertWittyName)



#ifndef FLDEBUG_H
#define FLDEBUG_H

#include "flGlobal.h"
#include <stdarg.h>

/**
  * The minimum amount to extend a debug message strings length by, in order to
  * make room for the extra arguments that will be inserted into it.
  * @warning This has never been tested with a value < 256.
  */ 
#define DEBUG_ARGEXTEND 256

#if FL_DEBUG != 0

#ifdef __cplusplus
extern "C" {
#endif

/**
  * A boolean value to set whether debugging is disabled at this point or not.
  * @note This is modified internally to stop infinite loops of errors, but
  *       it is also possible for the user to use this to stop debugging in certain
  *       critical parts of a program.
  */
extern bool debugEnabled;

extern void debugInit();
#if FL_DEBUG_CALLEDFROM
#if FL_DEBUG_ERROR
extern void debugErrorFrom(const char* inFile, const char* inFunc, int inLine, char* inMessage, ...);
#define debugError(inMessage, ...) debugErrorFrom(__FILE__, __FUNCTION__, __LINE__, inMessage, ## __VA_ARGS__)
#endif
#if FL_DEBUG_WARNING
extern void debugWarningFrom(const char* inFile, const char* inFunc, int inLine, char* inMessage, ...);
#define debugWarning(inMessage, ...) debugWarningFrom(__FILE__, __FUNCTION__, __LINE__, inMessage, ## __VA_ARGS__)
#endif
#if FL_DEBUG_DEVWARNING
extern void debugDevWarningFrom(const char* inFile, const char* inFunc, int inLine, char* inMessage, ...);
#define debugDevWarning(inMessage, ...) debugDevWarningFrom(__FILE__, __FUNCTION__, __LINE__, inMessage, ## __VA_ARGS__)
#endif
extern void debugCriticalErrorFrom(const char* inFile, const char* inFunc, int inLine, char* inMessage, ...);
#define debugCriticalError(inMessage, ...) debugCriticalErrorFrom(__FILE__, __FUNCTION__, __LINE__, inMessage, ## __VA_ARGS__)
extern void debugLogFrom(char* inType, const char* inFile, const char* inFunc, int inLine, char* inMessage, ...);
extern void debugStatusLogFrom(const char* inFile, const char* inFunc, int inLine, char* inMessage, ...);
#define debugStatusLog(inMessage, ...) debugStatusLogFrom( __FILE__, __FUNCTION__, __LINE__, inMessage, ## __VA_ARGS__)
#else
#if FL_DEBUG_ERROR
extern void debugError(char* inMessage, ...);
#endif
#if FL_DEBUG_WARNING
extern void debugWarning(char* inMessage, ...);
#endif
#if FL_DEVDEBUG_WARNING
extern void debugDevWarning(char* inMessage, ...);
#endif
extern void debugCriticalError(char* inMessage, ...);
extern void debugStatusLog(char* inMessage, ...);
#endif

extern void debugLog(char* inType, char* inMessage, ...);

#if (FL_GRAPHICS && FL_TEXTURE)
extern void debugScreenshot();
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
