// Funclib PbpPlus v1.0.0
// 
// This module contains functions used to access data archived in PBP and PbpPlus files.
// It is used as an extension to flFile and can be used by programmers as an alternative to bin2o.
// 
// Contributor(s): Flatmush



#ifndef FLFILEPBPPLUS_H
#define FLFILEPBPPLUS_H

#include "flGlobal.h"
#include <stdio.h>

#if FL_FILE_PBPPLUS

#ifdef __cplusplus
extern "C" {
#endif

// Open a pbpPlus context to for use in all other functions.
extern void* filePbpPlusOpen(char* inPBP);
// Get the size of a file within a pbpPlus file.
extern int   filePbpPlusFileSize(void* inContext, char* inFile);
// Get the offset of a file within a pbpPlus file.
extern int   filePbpPlusFileOffset(void* inContext, char* inFile);
// Extract a file from pbpPlus as a char array.
extern char* filePbpPlusFileRead(void* inContext, char* inFile);
// Write a file back to pbpPlus from a char array.
extern void filePbpPlusFileWrite(void* inContext, char* inFile, char* inData);

#ifdef __cplusplus
}
#endif

#endif

#endif
