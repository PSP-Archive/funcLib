#include "flGlobal.h"
#if FL_COLOR
#include <stdlib.h>
#include <stdio.h>

#if !FL_INCLUDE_ALL_C
#include "flColor.h"
#include "flMath.h"
#endif

u32 colorHSL(u8 inHue, u8 inSat, u8 inLum) {
     if(!inSat)
          return color8888_RGB(inLum, inLum, inLum);
     int tempQ;
     if(inLum < 128)
          tempQ = inLum * (256 + inSat);
     else
          tempQ = (inLum + inSat) - (inLum * inSat);
     int tempP = (512 * inLum) - tempQ;
     int tempOut[3];
     tempOut[0] = inHue + 85;
     tempOut[1] = inHue;
     tempOut[2] = inHue - 85;
     int i;
     for(i = 0; i < 3; i++) {
          while(tempOut[i] < 0)
               tempOut[i] += 256;
          while(tempOut[i] >= 256)
               tempOut[i] -= 256;
          if(tempOut[i] < 43) {
               tempOut[i] = tempP + ((tempQ - tempP) * 6 * tempOut[i]);
          } else if(tempOut[i] < 128) {
               tempOut[i] = tempQ;
          } else if(tempOut[i] < 170) {
               tempOut[i] = tempP + ((tempQ - tempP) * (170 - tempOut[i]) * 6);
          } else {
               tempOut[i] = tempP;
          }
     }
     return color8888_RGB(tempOut[0], tempOut[1], tempOut[2]);
}

u8 colorHue8888(u32 inColor) {
     if(max(max(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)) == min(min(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)))
          return 0;
     int tempHue = 0;
     if(color8888_Red(inColor) > max(color8888_Green(inColor), color8888_Blue(inColor))) {
          if(color8888_Green(inColor) >= color8888_Blue(inColor)) {
               tempHue = (color8888_Green(inColor) - color8888_Blue(inColor));
               tempHue /= (max(max(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)) - min(min(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)));
               tempHue /= 6;
          } else {
               tempHue = ((int)color8888_Green(inColor) - (int)color8888_Blue(inColor));
               tempHue /= (max(max(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)) - min(min(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)));
               tempHue /= 6;
               tempHue += 256;
          }
     } else if(color8888_Green(inColor) > max(color8888_Red(inColor), color8888_Blue(inColor))) {
          tempHue = ((int)color8888_Blue(inColor) - color8888_Red((int)inColor));
          tempHue /= (max(max(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)) - min(min(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)));
          tempHue /= 6;
          tempHue += 85;
     } else {
          tempHue = ((int)color8888_Red(inColor) - color8888_Green((int)inColor));
          tempHue /= (max(max(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)) - min(min(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)));
          tempHue /= 6;
          tempHue += 170;
     }
     return (tempHue & 0xFF);
}

u8 colorSat8888(u32 inColor) {
     u32 tempSat = 0;
     if(color8888_Red(inColor) > color8888_Green(inColor))
          tempSat = color8888_Red(inColor) - color8888_Green(inColor);
     else
          tempSat = color8888_Red(inColor) - color8888_Green(inColor);
     if(color8888_Green(inColor) > color8888_Blue(inColor))
          tempSat = color8888_Green(inColor) - color8888_Blue(inColor);
     else
          tempSat = color8888_Blue(inColor) - color8888_Green(inColor);
     if(color8888_Blue(inColor) > color8888_Red(inColor))
          tempSat = color8888_Blue(inColor) - color8888_Red(inColor);
     else
          tempSat = color8888_Red(inColor) - color8888_Blue(inColor);
     return ((tempSat / 3) & 0x000000FF);
}

u8 colorLum8888(u32 inColor) {
     return ((max(max(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)) >> 1) + (min(min(color8888_Red(inColor), color8888_Green(inColor)), color8888_Blue(inColor)) >> 1));
}

/*u32 colorBlend8888(u32 inColor0, u32 inColor1, float inRatio) {
     inRatio = pos(inRatio);
     if(inRatio > 1)
          inRatio = (1.0f / inRatio);
     u32 tempOut = ((int)(((float)color8888_Red(inColor0) * inRatio) + ((float)color8888_Red(inColor0) * (1.0f - inRatio))) & 0x000000FF);
     tempOut += (((int)(((float)color8888_Green(inColor0) * inRatio) + ((float)color8888_Green(inColor0) * (1.0f - inRatio))) & 0x000000FF) << 8);
     tempOut += (((int)(((float)color8888_Blue(inColor0) * inRatio) + ((float)color8888_Blue(inColor0) * (1.0f - inRatio))) & 0x000000FF) << 16);
     tempOut |= 0xFF000000;
     return tempOut;
}*/

u32 colorMergeN8888(u32* inColor, u32 inColorCount) {
     u32 tempRed = 0, tempGreen = 0, tempBlue = 0, tempAlpha = 0;
     u8* tempPtr = (u8*)inColor;
     u32 i;
     for(i = 0; i < inColorCount; i++, tempPtr += 4) {
          tempRed += tempPtr[0];
          tempGreen += tempPtr[1];
          tempBlue += tempPtr[2];
          tempAlpha += tempPtr[3];
     }
     tempRed /= inColorCount;
     tempGreen /= inColorCount;
     tempBlue /= inColorCount;
     tempAlpha /= inColorCount;
    
     return color8888_RGBA(tempRed, tempGreen, tempBlue, tempAlpha);
}

u16 colorMergeN4444(u16* inColor, u16 inColorCount) {
     u32 tempRed = 0, tempGreen = 0, tempBlue = 0, tempAlpha = 0;
     u32 i;
     for(i = 0; i < inColorCount; i++) {
          tempRed += color4444_Red(inColor[i]);
          tempGreen += color4444_Green(inColor[i]);
          tempBlue += color4444_Blue(inColor[i]);
          tempAlpha += color4444_Alpha(inColor[i]);
     }
     tempRed /= inColorCount;
     tempGreen /= inColorCount;
     tempBlue /= inColorCount;
     tempAlpha /= inColorCount;
    
     return color4444_RGBA(tempRed, tempGreen, tempBlue, tempAlpha);
}

u16 colorMergeN5551(u16* inColor, u16 inColorCount) {
     u32 tempRed = 0, tempGreen = 0, tempBlue = 0, tempAlpha = 0;
     u32 i;
     for(i = 0; i < inColorCount; i++) {
          tempRed += color5551_Red(inColor[i]);
          tempGreen += color5551_Green(inColor[i]);
          tempBlue += color5551_Blue(inColor[i]);
          tempAlpha += color5551_Alpha(inColor[i]);
     }
     tempRed /= inColorCount;
     tempGreen /= inColorCount;
     tempBlue /= inColorCount;
     tempAlpha /= inColorCount;
    
     return color5551_RGBA(tempRed, tempGreen, tempBlue, tempAlpha);
}

u16 colorMergeN565(u16* inColor, u16 inColorCount) {
     u32 tempRed = 0, tempGreen = 0, tempBlue = 0;
     u32 i;
     for(i = 0; i < inColorCount; i++) {
          tempRed += color565_Red(inColor[i]);
          tempGreen += color565_Green(inColor[i]);
          tempBlue += color565_Blue(inColor[i]);
     }
     tempRed /= inColorCount;
     tempGreen /= inColorCount;
     tempBlue /= inColorCount;
    
     return color565_RGB(tempRed, tempGreen, tempBlue);
}

u32 colorRandom8888(u8 inBaseLum, u8 inRandLum, u8 inAlpha) {
     #if FL_MATH != 0
     u32 tempOut = ((inBaseLum + mathRandi(inRandLum)) + ((inBaseLum + mathRandi(inRandLum)) << 8) + ((inBaseLum + mathRandi(inRandLum)) << 16)) | (inAlpha << 24);
     return tempOut;
     #else
     u32 tempOut = (inBaseLum + ((rand() * inRandLum) / RAND_MAX));
     tempOut += ((inBaseLum + ((rand() * inRandLum) / RAND_MAX)) << 8);
     tempOut += ((inBaseLum + ((rand() * inRandLum) / RAND_MAX)) << 16);
     tempOut |= (inAlpha << 24);
     return tempOut;
     #endif
}


u32 colorLumMult8888(u32 inColor, float inLum) {
    inLum = pos(inLum);
    u32 tempOut = (min((int)((float)color8888_Red(inColor) * inLum), 255));
    tempOut += ((min((int)((float)color8888_Green(inColor) * inLum), 255)) << 8);
    tempOut += ((min((int)((float)color8888_Blue(inColor) * inLum), 255)) << 16);
    tempOut |= (inColor & 0xFF000000);
    return tempOut;
}

u32 colorAlphaMult8888(u32 inColor, float inMult) {
     u32 tempOut = (min((int)((float)color8888_Alpha(inColor) * inMult), 255) << 24);
     tempOut = ((inColor & 0x00FFFFFF) | tempOut);
     return tempOut;
}

#endif
