// Funclib TextureTGA v1.0.0
// 
// This module adds TGA texture loading and saving functionality.
// 
// Contributor(s): Flatmush



#ifndef FLCONFIG_H
#define FLCONFIG_H

#include "flGlobal.h"

#if FL_CONFIG

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_NODE_TYPE_NULL    0
#define CONFIG_NODE_TYPE_VALUE   1
#define CONFIG_NODE_TYPE_SECTION 2

typedef struct {
     u32   cfgnType;
     char* cfgnKey;
     char* cfgnValue;
     void* cfgnParent;
     
     void* cfgnNext;
     void* cfgnLast;
     
     u32   cfgnReserved[2];
}

typedef struct {
     ConfigNode* cfgBase;
     ConfigNode* cfgSection;
     char*       cfgPath;
     u32         cfgReserved;
} Congig;

Config* configLoad(char* inPath);
bool    configSave(char* inPath, Config* inConfig);

Config* configLoadINI(char* inPath);
bool    configSaveINI(char* inPath, Config* inConfig);

void    configFree(Config* inConfig);

void*   configOpenSection(Config* inConfig);
void*   configCloseSection(Config* inConfig);

s64     configReadInt(Config* inConfig, char* inSection, char* inKey);
s64     configReadHexInt(Config* inConfig, char* inSection, char* inKey);
char    configReadChar(Config* inConfig, char* inSection, char* inKey);
char*   configReadString(Config* inConfig, char* inSection, char* inKey);

#endif

#endif

#endif
