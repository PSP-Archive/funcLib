#if FL_STRING != 0

#if FL_INCLUDE_ALL_C == 0
#include "flString.h"
#include "flMemory.h"

#if FL_DEBUG != 0
#include "flDebug.h"
#endif
#endif

u32 ustrLen(u16* inStr) {
     if(!inStr)
          return 0;
     u32 tempOut, i;
     for(i = 0, tempOut = 0; inStr[i]; i++)
          tempOut++;
     return tempOut;
}

u32 strLen(u8* inStr) {
     if(!inStr)
          return 0;
     u32 tempOut, i;
     for(i = 0, tempOut = 0; inStr[i]; i++)
          tempOut++;
     return tempOut;
}

u16* strToUstr(u8* inStr) {
     if(!inStr)
          return NULL;
     u16* tempOut = (u16*)memAlloc((strLen(inStr) + 1) << 1);
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot convert string to unicode.\nProbably out of memory.");
          #endif
          return NULL;
     }
     u32 i;
     for(i = 0; inStr[i]; i++)
          tempOut[i] = (u16)inStr[i];
     tempOut[i] = 0;
     return tempOut;
}

u8* ustrToStr(u16* inStr) {
     if(!inStr)
          return NULL;
     u8* tempOut = (u8*)memAlloc(ustrLen(inStr) + 1);
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot convert string to ascii.\nProbably out of memory.");
          #endif
          return NULL;
     }
     u32 i;
     for(i = 0; inStr[i]; i++)
          tempOut[i] = (u8)(inStr[i] & 0x00FF);
     tempOut[i] = 0;
     return tempOut;
}

u8* strCopy(u8* inStr) {
     if(!inStr)
          return NULL;
     u8* tempOut = (u8*)memAlloc(strLen(inStr) + 1);
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot copy string.\nProbably out of memory.");
          #endif
          return NULL;
     }
     memCopy(tempOut, inStr, (strLen(inStr) + 1));
     return tempOut;
}

u8* strCopyTo(u8* inStrDest, u8* inStrSrc) {
     if(!inStrSrc) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to copy NULL string.");
          #endif
          return NULL;
     }
     if(!inStrDest) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to copy string to a NULL destination.");
          #endif
          return NULL;
     }
     memCopy(inStrDest, inStrSrc, (strLen(inStrSrc) + 1));
     return inStrDest;
}

u16* ustrCopy(u16* inStr) {
     if(!inStr)
          return NULL;
     u16* tempOut = (u16*)memAlloc((ustrLen(inStr) + 1)  << 1);
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot copy unicode string.\nProbably out of memory.");
          #endif
          return NULL;
     }
     memCopy(tempOut, inStr, ((ustrLen(inStr) + 1) << 1));
     return tempOut;
}

u16* ustrCopyTo(u16* inStrDest, u16* inStrSrc) {
     if(!inStrSrc) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to copy NULL string.");
          #endif
          return NULL;
     }
     if(!inStrDest) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to copy string to a NULL destination.");
          #endif
          return NULL;
     }
     memCopy(inStrDest, inStrSrc, ((ustrLen(inStrSrc) + 1) << 1));
     return inStrDest;
}

u8* strLCase(u8* inStr) {
     u32 i;
     for(i = 0; inStr[i]; i++) {
          if((inStr[i] >= 'A') && (inStr[i] <= 'Z'))
               inStr[i] += ('a' - 'A');
     }
     return inStr;
}

u8* strUCase(u8* inStr) {
     u32 i;
     for(i = 0; inStr[i]; i++) {
          if((inStr[i] >= 'a') && (inStr[i] <= 'z'))
               inStr[i] -= ('a' - 'A');
     }
     return inStr;
}

u16* ustrLCase(u16* inStr) {
     u32 i;
     for(i = 0; inStr[i]; i++) {
          if((inStr[i] >= 'A') && (inStr[i] <= 'Z'))
               inStr[i] += ('a' - 'A');
     }
     return inStr;
}

u16* ustrUCase(u16* inStr) {
     u32 i;
     for(i = 0; inStr[i]; i++) {
          if((inStr[i] >= 'a') && (inStr[i] <= 'z'))
               inStr[i] -= ('a' - 'A');
     }
     return inStr;
}

s32 strCmp(u8* inStr0, u8* inStr1) {
     if(inStr0 == inStr1)
          return 0;
     if(!inStr0) {
          if(!inStr1)
               return 0;
          return (0 - (s32)inStr1[0]);
     }
     if(!inStr1)
          return ((s32)inStr1[1]);
     u32 i = 0;
     while((inStr0[i] == inStr1[i]) && inStr0[i])
          i++;
     return ((s32)inStr0[i] - (s32)inStr1[i]);
}

s32 ustrCmp(u16* inStr0, u16* inStr1) {
     if(inStr0 == inStr1)
          return 0;
     if(!inStr0) {
          if(!inStr1)
               return 0;
          return (0 - (s32)inStr1[0]);
     }
     if(!inStr1)
          return ((s32)inStr1[1]);
     u32 i = 0;
     while((inStr0[i] == inStr1[i]) && inStr0[i])
          i++;
     return ((s32)inStr0[i] - (s32)inStr1[i]);
}

int strToInt(u8* inStr) {
     if(!inStr || (inStr[0] == 0))
          return 0;
     int tempOut = 0;
     u8* tempStrPtr = inStr;
     bool tempNeg = false;
     if(tempStrPtr[0] == '-') {
          tempNeg = true;
          tempStrPtr++;
     } else if(tempStrPtr[0] == '+') {
          tempStrPtr++;
     }
     while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9')) {
           tempOut *= 10;
           tempOut += (int)(tempStrPtr[0] - '0');
           tempStrPtr++;
     }
     if(tempNeg)
           return -tempOut;
     return tempOut;
}

float strToFloat(u8* inStr) {
     if(!inStr || (inStr[0] == 0))
          return 0.0f;
     float tempOut = 0.0f;
     u8* tempStrPtr = inStr;
     bool tempNeg = false;
     if(tempStrPtr[0] == '-') {
          tempNeg = true;
          tempStrPtr++;
     } else if(tempStrPtr[0] == '+') {
          tempStrPtr++;
     }
     while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9')) {
           tempOut *= 10.0f;
           tempOut += (float)(tempStrPtr[0] - '0');
           tempStrPtr++;
     }
     if(tempStrPtr[0] == '.') {
          tempStrPtr++;
          int i = 1;
          while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9')) {
                if(tempStrPtr[0] != '0')
                     tempOut += ((float)(tempStrPtr[0] - '0') / powf(10.0f, (float)i));
                tempStrPtr++; i++;
          }
     }
     if((tempStrPtr[0] == 'e') || (tempStrPtr[0] == 'E')) {
          tempStrPtr++;
          bool tempExpNeg = false;
          int tempExp = 0;
          if(tempStrPtr[0] == '-') {
               tempStrPtr++;
               tempExpNeg = true;
          } else if(tempStrPtr[0] == '+') {
               tempStrPtr++;
          }
          while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9')) {
               tempExp *= 10;
               tempExp += (int)(tempStrPtr[0] - '0');
               tempStrPtr++;
          }
          if(tempExpNeg)
               tempOut /= powf(10.0f, (float)tempExp);
          else
               tempOut *= powf(10.0f, (float)tempExp);
     }
     if(tempNeg)
           return -tempOut;
     return tempOut;
}

vect2f strToVect2f(u8* inStr) {
     vect2f tempOut;
     tempOut.x = 0.0f;
     tempOut.y = 0.0f;
     
     if(!inStr || (inStr[0] == 0))
          return tempOut;

     u8* tempStrPtr = inStr;
     
     while((tempStrPtr[0] == ' ') || (tempStrPtr[0] == ASCII_TAB) || (tempStrPtr[0] == '('))
          tempStrPtr++;
     if(tempStrPtr[0] == 0)
          return tempOut;
     tempOut.x = strToFloat(tempStrPtr);
     if((tempStrPtr[0] == '+') || (tempStrPtr[0] == '-'))
          tempStrPtr++;
     while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9'))
          tempStrPtr++;
     if(tempStrPtr[0] == '.') {
          tempStrPtr++;
          while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9'))
               tempStrPtr++;
     }
     if((tempStrPtr[0] == 'e') || (tempStrPtr[0] == 'E')) {
          tempStrPtr++;
          if((tempStrPtr[0] == '-') || (tempStrPtr[0] == '+'))
               tempStrPtr++;
          while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9'))
               tempStrPtr++;
     }
     
     while((tempStrPtr[0] == ' ') || (tempStrPtr[0] == ASCII_TAB) || (tempStrPtr[0] == ','))
          tempStrPtr++;
     if(tempStrPtr[0] == 0)
          return tempOut;
     tempOut.y = strToFloat(tempStrPtr);
     
     return tempOut;
}

vect3f strToVect3f(u8* inStr) {
     vect3f tempOut;
     tempOut.x = 0.0f;
     tempOut.y = 0.0f;
     tempOut.z = 0.0f;
     
     if(!inStr || (inStr[0] == 0))
          return tempOut;
     
     u8* tempStrPtr = inStr;
     
     while((tempStrPtr[0] == ' ') || (tempStrPtr[0] == ASCII_TAB) || (tempStrPtr[0] == '('))
          tempStrPtr++;
     if(tempStrPtr[0] == 0)
          return tempOut;
     tempOut.x = strToFloat(tempStrPtr);
     if((tempStrPtr[0] == '+') || (tempStrPtr[0] == '-'))
          tempStrPtr++;
     while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9'))
          tempStrPtr++;
     if(tempStrPtr[0] == '.') {
          tempStrPtr++;
          while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9'))
               tempStrPtr++;
     }
     if((tempStrPtr[0] == 'e') || (tempStrPtr[0] == 'E')) {
          tempStrPtr++;
          if((tempStrPtr[0] == '-') || (tempStrPtr[0] == '+'))
               tempStrPtr++;
          while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9'))
               tempStrPtr++;
     }
     
     while((tempStrPtr[0] == ' ') || (tempStrPtr[0] == ASCII_TAB) || (tempStrPtr[0] == ','))
          tempStrPtr++;
     if(tempStrPtr[0] == 0)
          return tempOut;
     tempOut.y = strToFloat(tempStrPtr);
     if((tempStrPtr[0] == '+') || (tempStrPtr[0] == '-'))
          tempStrPtr++;
     while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9'))
          tempStrPtr++;
     if(tempStrPtr[0] == '.') {
          tempStrPtr++;
          while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9'))
               tempStrPtr++;
     }
     if((tempStrPtr[0] == 'e') || (tempStrPtr[0] == 'E')) {
          tempStrPtr++;
          if((tempStrPtr[0] == '-') || (tempStrPtr[0] == '+'))
               tempStrPtr++;
          while((tempStrPtr[0] >= '0') && (tempStrPtr[0] <= '9'))
               tempStrPtr++;
     }
     
     while((tempStrPtr[0] == ' ') || (tempStrPtr[0] == ASCII_TAB) || (tempStrPtr[0] == ','))
          tempStrPtr++;
     if(tempStrPtr[0] == 0)
          return tempOut;
     tempOut.z = strToFloat(tempStrPtr);
     
     return tempOut;
}

#endif
