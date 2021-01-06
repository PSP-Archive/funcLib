// funcLib - flString v1.0.0
//
// Contains functions for string manipulation,
// as well as functions for unicode conversion.
//
// Contributors: Flatmush





#ifndef FLSTRING_H
#define FLSTRING_H

#if FL_STRING != 0

#include "flGlobal.h"

#ifdef __cplusplus
extern "C" {
#endif

extern u32  strLen(u8* inStr);
extern s32  strCmp(u8* inStr0, u8* inStr1);
extern u16* strToUstr(u8* inStr);
extern u8*  strCopy(u8* inStr);
extern u8*  strCopyTo(u8* inStrDest, u8* inStrSrc);
extern u8*  strLCase(u8* inStr);
extern u8*  strUCase(u8* inStr);
extern u32  ustrLen(u16* inStr);
extern s32  ustrCmp(u16* inStr0, u16* inStr1);
extern u8*  ustrToStr(u16* inStr);
extern u16* ustrCopy(u16* inStr);
extern u16* ustrCopyTo(u16* inStrDest, u16* inStrSrc);
extern u16* ustrLCase(u16* inStr);
extern u16* ustrUCase(u16* inStr);

extern int    strToInt(u8* inStr);
extern float  strToFloat(u8* inStr);
extern vect2f strToVect2f(u8* inStr);
extern vect3f strToVect3f(u8* inStr);

#ifdef __cplusplus
}
#endif

#else

#include <string.h>

#define strLen(inStr) strlen((char*)inStr)
#define strCmp(inStr0, inStr1) strcmp((char*)inStr0, (char*)inStr1)
#define strCopyTo(inStrDest, inStrSrc) strcpy((char*)inStrDest, (char*)inStrSrc)

#endif

#endif
