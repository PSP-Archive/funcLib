#include "flGlobal.h"
#if FL_TIMER
#include <stdio.h>
#include <string.h>

#if !FL_INCLUDE_ALL_C
#include "flString.h"
#include "flMemory.h"
#include "flDebug.h"
#include "flFile.h"
#endif

Config* configLoad(char* inPath) {
        
}

bool configSave(char* inPath, Config* inConfig) {
        
}


Config* configLoadINI(char* inPath) {
        
}

bool configSaveINI(char* inPath, Config* inConfig) {
        
}


void configFree(Config* inConfig) {
        
}


void* configOpenSection(Config* inConfig) {
        
}

void* configCloseSection(Config* inConfig) {
        
}


s64 configReadInt(Config* inConfig, char* inSection, char* inKey) {
        
}

s64 configReadHexInt(Config* inConfig, char* inSection, char* inKey) {
        
}

char configReadChar(Config* inConfig, char* inSection, char* inKey) {
        
}

char* configReadString(Config* inConfig, char* inSection, char* inKey) {
        
}


#endif
