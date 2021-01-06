#include "flGlobal.h"
#if FL_FONT != 0
#include <string.h>
#include <stdio.h>
#include <pspgu.h>
#include <pspkernel.h>

#if FL_GRAPHICS == 0
#include <pspdebug.h>
#endif

#if FL_INCLUDE_ALL_C == 0
#include "flFont.h"
#include "flMemory.h"

#if FL_DEBUG != 0
#include "flDebug.h"
#endif

#if FL_GRAPHICS != 0
#include "flGraphics.h"
#if FL_GRAPHICS_2D != 0
#include "flGraphics2d.h"
#endif
#endif
#endif

#if FL_GRAPHICS != 0
extern u8 msx[];
u8* fontDebugTex = NULL;
u8* fontMonoPal = NULL;
//u32 fontDebugColorCur     = 0xFFFFFFFF;
//u32 fontDebugBackColorCur = 0x00FFFFFF;
#endif

Font* fontDefault = NULL;

void fontInit() {
     fontDefault = fontCreateDebug(0, 0, FL_FONT_COLOR_DEFAULT);
}

Font* fontLoad(char* inPath) {
     Font* tempOut = NULL;
     #if FL_TEXTURE != 0
     Texture* tempTex = texLoad(inPath);
     if(tempTex) {
          tempOut = fontCreateTexture(tempTex, FL_FONT_HSPACE_DEFAULT, FL_FONT_VSPACE_DEFAULT, FL_FONT_COLOR_DEFAULT);
          tempOut->fontDataGlobal = false;
          return tempOut;
     }
     #endif

     char tempExt[5];
     strcpy(tempExt, &inPath[strlen(inPath) - 4]);
     int i;
     for(i = 0; i < 4; i++) {
          if((tempExt[i] >= ASCII_A) && (tempExt[i] <= ASCII_Z))
               tempExt[i] = (tempExt[i] - 32);
     }
     if(strcmp(tempExt, ".pgf") == 0) {
          #if FL_FONT_PGF != 0
          return fontLoadPGF(inPath);
          #else
          #if FL_DEBUG_DEVWARNING != 0
          debugDevWarning("PGF textures not supported in this build.");
          #endif
          #endif
     }
     
     #if FL_DEBUG_WARNING != 0
     char tempString[256];
     sprintf(tempString, "Error loading font(%s).", inPath);
     #if FL_TEXTURE != 0
     sprintf(tempString, "%s\nTry including TEXTURE for more success.", tempString);
     #endif
     debugWarning(tempString);
     #endif
     
     return NULL;
}

bool fontSave(char* inPath, Font* inFont) {
     #if FL_DEBUG_DEVWARNING != 0
     debugDevWarning("fontSave not yet implemented.");
     #endif
     return false;
}

#if FL_TEXTURE != 0
Font* fontCreateTexture(Texture* inTex, int inHSpace, int inVSpace, u32 inColor) {
     if(!inTex) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to create font from NULL texture.");
          #endif
          return NULL;
     }
     Font* tempOut = memAlloc(sizeof(Font));
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Error couldn't create font structure.\nProbably out of memory.");
          #endif
          return NULL;
     }
     tempOut->fontType = FONT_TYPE_TEXTURE;
     tempOut->fontData = inTex;
     tempOut->fontHSpace = inHSpace;
     tempOut->fontVSpace = inVSpace;
     tempOut->fontColor = inColor;
     tempOut->fontBackColor = (inColor & 0x00FFFFFF);
     tempOut->fontFixedWidth = true;
     tempOut->fontDataGlobal = true;
     
     return tempOut;
}
#endif

Font* fontCreateDebug(int inHSpace, int inVSpace, u32 inColor) {
     Font* tempOut = memAlloc(sizeof(Font));
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Error couldn't create font structure.\nProbably out of memory.");
          #endif
          return NULL;
     }
     tempOut->fontHSpace = inHSpace;
     tempOut->fontVSpace = inVSpace;
     tempOut->fontColor = inColor;
     tempOut->fontBackColor = (inColor & 0x00FFFFFF);
     tempOut->fontFixedWidth = true;
     tempOut->fontType = FONT_TYPE_DEBUG;
     tempOut->fontData = NULL;
     tempOut->fontDataGlobal = true;
     
     #if FL_GRAPHICS != 0
     if(!fontDebugTex)
          fontCreateDebugTex();
     #endif
     
     return tempOut;
}

#if FL_GRAPHICS != 0
bool fontCreateDebugTex() {
     u8* tempFont = msx;
     
     #if FL_MEMORY_VMEM != 0
     u8* tempBuffer = vmemAlloc(8192);
     if(!tempBuffer)
          tempBuffer = memAlign(16, 8192);
     #else
     u8* tempBuffer = memAlign(16, 8192);
     #endif
     
     if(!tempBuffer) {
          #if FL_DEBUG_ERROR != 0
          debugError("Not enough memory to create CLUT4 debug font.");
          #endif
          return false;
     }
     u8* tempBufferPtr = tempBuffer;
     u8  tempPixel = 0;
     int i;
     for(i = 0; i < 16384; i++) {
          tempPixel >>= 4;
          if(*tempFont & (128 >> (i & 7)))
               tempPixel += 0xF0;
          if(i & 1) {
               *tempBufferPtr = tempPixel;
               tempPixel = 0;
               tempBufferPtr++;
          }
          if((i & 1023) == 1023) {
               tempFont++;
          } else if((i & 127) == 127) {
               tempFont -= 119;
          } else if((i & 7) == 7) {
               tempFont += 8;
          }
     }

     graphicsSwizzleData(tempBuffer, 128, 128, GU_PSM_T4);
     fontDebugTex = tempBuffer;

     /*#if FL_MEMORY_VMEM != 0
     fontMonoPal = vmemAlloc(64);
     if(!fontMonoPal)
          fontMonoPal = memAlign(16, 64);
     #else*/
     fontMonoPal = memAlign(16, 64);
     //#endif
     
     tempBuffer = fontMonoPal;
     for(i = 0; i < 16; i++) {
          *(tempBuffer++) = 0xFF;
          *(tempBuffer++) = 0xFF;
          *(tempBuffer++) = 0xFF;
          *(tempBuffer++) = (i * 17);
     }
     
     sceKernelDcacheWritebackRange((void*)fontMonoPal, 64);
     
     return true;
}
#endif

void fontFree(Font* inFont) {
     if(!inFont) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to free a NULL font.");
          #endif
          return;
     }
     if(!inFont->fontDataGlobal) {
          #if FL_TEXTURE != 0
          if((inFont->fontType == FONT_TYPE_TEXTURE) || (inFont->fontType == FONT_TYPE_TEXTURE_MONO)) {
               texFree(inFont->fontData);
          }
          #endif
     }
     memFree(inFont);
}





int fontHeight(Font* inFont) {
     #if FL_TEXTURE != 0
     if((inFont->fontType == FONT_TYPE_TEXTURE) || (inFont->fontType == FONT_TYPE_TEXTURE_MONO)) {
          if(!inFont->fontData) {
               #if FL_DEBUG_WARNING != 0
               debugWarning("Texture font has no data.");
               #endif
               return 0;
          }
          Texture* tempTex = (Texture*)inFont->fontData;
          return (tempTex->texHeight >> 4);
     }
     #endif
     if(inFont->fontType == FONT_TYPE_DEBUG)
          return 8;
     return 0;
}

int fontCharWidth(Font* inFont, char inChar) {
     if(inFont->fontFixedWidth) {
          #if FL_TEXTURE != 0
          if((inFont->fontType == FONT_TYPE_TEXTURE) || (inFont->fontType == FONT_TYPE_TEXTURE_MONO)) {
               if(!inFont->fontData) {
                    #if FL_DEBUG_WARNING != 0
                    debugWarning("Texture font has no data.");
                    #endif
                    return 0;
               }
               Texture* tempTex = (Texture*)inFont->fontData;
               if(inChar == ASCII_TAB)
                    return ((tempTex->texWidth >> 4) * FL_FONT_TABWIDTH_DEFAULT);
               else if(inChar == ASCII_BS)
                    return (0 - (tempTex->texWidth >> 4));
               return (tempTex->texWidth >> 4);
          }
          #endif
          if(inFont->fontType == FONT_TYPE_DEBUG) {
               if(inChar == ASCII_TAB)
                    return (8 * FL_FONT_TABWIDTH_DEFAULT);
               else if(inChar == ASCII_BS)
                    return -8;
               return 8;
          }
     }

     // TODO - Variable width fonts.

     return 0;
}

int fontStringWidth(Font* inFont, char* inString) {
     int tempMax = 0;
     unsigned int i;
     int tempCur = 0;
     for(i = 0; i < strlen(inString); i++) {
          if(((unsigned char)inString[i] < ASCII_SPACE) || ((unsigned char)inString[i] == 0x7F) || ((unsigned char)inString[i] == 0xFF))
               continue;
          if(inString[i] == ASCII_LF) {
               if(tempCur > tempMax)
                    tempMax = tempCur;
               tempCur = 0;
          } else {
               tempCur += fontCharWidth(inFont, inString[i]) + inFont->fontHSpace;
          }
     }
     if(tempMax > tempCur)
          return tempMax;
     return tempCur;
}

int fontStringHeight(Font* inFont, char* inString) {
     int tempOut = fontHeight(inFont);
     char* tempPtr;
     for(tempPtr = inString; tempPtr[0] != 0; tempPtr++) {
          if(tempPtr[0] == '\n')
               tempOut += fontHeight(inFont);
     }
     return tempOut;
}




bool fontDraw2dChar(int inX, int inY, Font* inFont, char inChar) {
     #if FL_GRAPHICS != 0
     #if FL_TEXTURE != 0
     if((inFont->fontType == FONT_TYPE_TEXTURE) || (inFont->fontType == FONT_TYPE_TEXTURE_MONO)) {
          if(!inFont->fontData) {
               #if FL_DEBUG_WARNING != 0
               debugWarning("Texture font has no data.");
               #endif
               return false;
          }
          if(inFont->fontType == FONT_TYPE_TEXTURE_MONO) {
               Texture* tempTex = (Texture*)inFont->fontData;
               u32* tempColorPtr = (u32*)tempTex->texPalette->palData;
               if((inFont->fontColor != tempColorPtr[15]) || (inFont->fontBackColor != tempColorPtr[0])) {
                    palFree(tempTex->texPalette);
                    tempTex->texPalette = NULL;
                    tempTex->texPalette = palCreateMonoT4(inFont->fontColor, inFont->fontBackColor);
                    if(!tempTex->texPalette) {
                         #if FL_DEBUG_ERROR != 0
                         debugError("Couldn't recreate palette for mono texture.");
                         #endif
                         return false;
                    }
               }
               if(!tempTex->texPalette) {
                    #if FL_DEBUG_ERROR != 0
                    debugError("Can't draw from mono texture, as it has no palette.");
                    #endif
                    return false;
               }
          } else {
               if((inFont->fontBackColor & 0xFF000000)!= 0x00000000) {
                    vertCVs* tempBackVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) << 1);
                    tempBackVerts[0].vertX = inX;
                    tempBackVerts[0].vertY = inY;
                    tempBackVerts[0].vertZ = 0;
                    tempBackVerts[0].vertColor = inFont->fontBackColor;
                    tempBackVerts[1].vertX = inX + fontCharWidth(inFont, inChar);
                    tempBackVerts[1].vertY = inY + fontHeight(inFont);
                    tempBackVerts[1].vertZ = 0;
                    tempBackVerts[1].vertColor = inFont->fontBackColor;
                    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempBackVerts);
               }
          }
          texBind((Texture*)inFont->fontData);
          sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
          sceGuTexFilter(GU_NEAREST, GU_NEAREST);
          vertTsVs* tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
          tempVerts[0].vertX = inX;
          tempVerts[0].vertY = inY;
          tempVerts[0].vertZ = 0;
          tempVerts[0].vertU = ((inChar & 15) * (((Texture*)inFont->fontData)->texWidth >> 4));
          tempVerts[0].vertV = (((inChar >> 4) & 15) * (((Texture*)inFont->fontData)->texHeight >> 4));
          tempVerts[1].vertX = inX + fontCharWidth(inFont, inChar);
          tempVerts[1].vertY = inY + fontHeight(inFont);
          tempVerts[1].vertZ = 0;
          tempVerts[1].vertU = ((inChar & 15) * (((Texture*)inFont->fontData)->texWidth >> 4)) + fontCharWidth(inFont, inChar);
          tempVerts[1].vertV = (((inChar >> 4) & 15) * (((Texture*)inFont->fontData)->texHeight >> 4)) + fontHeight(inFont);
          sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
          return true;
     }
     #endif
     if(inFont->fontType == FONT_TYPE_DEBUG) {
          sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);
          #if FL_TEXTURE
          if((u32)palBound != (u32)fontMonoPal) {
               sceGuClutLoad(2, (void*)fontMonoPal);
               palBound = (void*)fontMonoPal;
          }
          #else
          sceGuClutLoad(2, (void*)fontMonoPal);
          #endif

          sceGuTexFilter(GU_NEAREST, GU_NEAREST);
          sceGuTexMode(GU_PSM_T4, 0, 0, GU_TRUE);

          #if FL_TEXTURE
          if((u32)texBound != (u32)fontDebugTex) {
               sceGuTexImage(0, 128, 128, 128, (void*)fontDebugTex);
               texBound = (void*)fontDebugTex;
          }
          #else
          sceGuTexImage(0, 128, 128, 128, (void*)fontDebugTex);
          #endif
 
          sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
          sceGuColor(inFont->fontColor);
          
          vertTsVs* tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
          tempVerts[0].vertX = inX;
          tempVerts[0].vertY = inY;
          tempVerts[0].vertZ = 0;
          tempVerts[0].vertU = ((inChar & 15) << 3);
          tempVerts[0].vertV = (((inChar >> 4) & 15) << 3);
          tempVerts[1].vertX = inX + 8;
          tempVerts[1].vertY = inY + 8;
          tempVerts[1].vertZ = 0;
          tempVerts[1].vertU = ((inChar & 15) << 3) + 8;
          tempVerts[1].vertV = (((inChar >> 4) & 15) << 3) + 8;
          sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
          
          sceGuColor(0xFFFFFFFF);
          return true;
     }
     #else
     if(inFont->fontType == FONT_TYPE_DEBUG) {
          pspDebugScreenSetBackColor(inFont->fontBackColor);
          pspDebugScreenPutChar(inX, inY, inFont->fontColor, (u8)inChar);
          return true;
     }
     #endif
     #if FL_DEBUG_WARNING != 0
     char tempString[512];
     sprintf(tempString, "Error drawing 2d char.");
     #if FL_GRAPHICS == 0
     sprintf(tempString, "%s\nTry including GRAPHICS for more success.", tempString);
     #endif
     #if FL_TEXTURE == 0
     sprintf(tempString, "%s\nTry including TEXTURE for more success.", tempString);
     #endif
     debugWarning(tempString);
     #endif
     return false;
}

void fontDraw2dString(int inX, int inY, Font* inFont, char* inString) {
     if(!inFont || !inString || !inString[0])
          return;
     #if FL_GRAPHICS != 0
     #if FL_TEXTURE != 0
     if((inFont->fontType == FONT_TYPE_TEXTURE) || (inFont->fontType == FONT_TYPE_TEXTURE_MONO)) {
          if(!inFont->fontData) {
               #if FL_DEBUG_WARNING != 0
               debugWarning("Texture font has no data.");
               #endif
               return;
          }
          if(inFont->fontType == FONT_TYPE_TEXTURE_MONO) {
               Texture* tempTex = (Texture*)inFont->fontData;
               u32* tempColorPtr = (u32*)tempTex->texPalette->palData;
               if((inFont->fontColor != tempColorPtr[15]) || (inFont->fontBackColor != tempColorPtr[0])) {
                    palFree(tempTex->texPalette);
                    tempTex->texPalette = NULL;
                    tempTex->texPalette = palCreateMonoT4(inFont->fontColor, inFont->fontBackColor);
                    if(!tempTex->texPalette) {
                         #if FL_DEBUG_ERROR != 0
                         debugError("Couldn't recreate palette for mono texture.");
                         #endif
                         return;
                    }
               }
               if(!tempTex->texPalette) {
                    #if FL_DEBUG_ERROR != 0
                    debugError("Can't draw from mono texture, as it has no palette.");
                    #endif
                    return;
               }
          }
          sceGuDisable(GU_DEPTH_TEST);
          texBind((Texture*)inFont->fontData);
          sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
          sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     }
     vertCVs* tempBackVerts;
     #else
     if(inFont->fontType == FONT_TYPE_DEBUG)
          pspDebugScreenSetBackColor(inFont->fontBackColor);
     #endif
     if(inFont->fontType == FONT_TYPE_DEBUG) {
          sceGuDisable(GU_DEPTH_TEST);
          sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);

          #if FL_TEXTURE
          if((u32)palBound != (u32)fontMonoPal) {
               sceGuClutLoad(2, (void*)fontMonoPal);
               palBound = (void*)fontMonoPal;
          }
          #else
          sceGuClutLoad(2, (void*)fontMonoPal);
          #endif
          
          sceGuTexFilter(GU_NEAREST, GU_NEAREST);
          sceGuTexMode(GU_PSM_T4, 0, 0, GU_TRUE);

          #if FL_TEXTURE
          if((u32)texBound != (u32)fontDebugTex) {
               sceGuTexImage(0, 128, 128, 128, (void*)fontDebugTex);
               texBound = (void*)fontDebugTex;
          }
          #else
          sceGuTexImage(0, 128, 128, 128, (void*)fontDebugTex);

          #endif
          sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
          sceGuColor(inFont->fontColor);
     }
     vertTsVs* tempVerts;
     #endif
     
     int tempLen = strlen(inString);
     int tempX, tempY;
     tempX = inX;
     tempY = inY;
     int i;
     for(i = 0; i < tempLen; i++) {
          if(inString[i] == ASCII_LF) {
               tempX = inX;
               tempY += (fontHeight(inFont) + inFont->fontVSpace);
          } else if(inString[i] == ASCII_CR) {
               tempX = inX;
          } else if(inString[i] == ASCII_BS) {
               tempX += (fontCharWidth(inFont, ASCII_BS) - inFont->fontHSpace);
          } else if(inString[i] == ASCII_TAB) {
               tempX += (fontCharWidth(inFont, ASCII_TAB) + inFont->fontHSpace);
          } else if(((unsigned char)inString[i] < ASCII_SPACE) || ((unsigned char)inString[i] == 0x7F) || ((unsigned char)inString[i] == 0xFF)) {
               continue;
          } else {
               #if FL_GRAPHICS != 0
               #if FL_TEXTURE != 0
               if((inFont->fontType == FONT_TYPE_TEXTURE) || (inFont->fontType == FONT_TYPE_TEXTURE_MONO)) {
                    tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
                    tempVerts[0].vertX = tempX;
                    tempVerts[0].vertY = tempY;
                    tempVerts[0].vertZ = 0;
                    tempVerts[0].vertU = ((inString[i] & 15) * (((Texture*)inFont->fontData)->texWidth >> 4));
                    tempVerts[0].vertV = (((inString[i] >> 4) & 15) * (((Texture*)inFont->fontData)->texHeight >> 4));
                    tempVerts[1].vertX = tempX + fontCharWidth(inFont, inString[i]);
                    tempVerts[1].vertY = tempY + fontHeight(inFont);
                    tempVerts[1].vertZ = 0;
                    tempVerts[1].vertU = ((inString[i] & 15) * (((Texture*)inFont->fontData)->texWidth >> 4)) + fontCharWidth(inFont, inString[i]);
                    tempVerts[1].vertV = (((inString[i] >> 4) & 15) * (((Texture*)inFont->fontData)->texHeight >> 4)) + fontHeight(inFont);
                    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
                    if((inFont->fontType == FONT_TYPE_TEXTURE) && ((inFont->fontBackColor & 0xFF000000)!= 0x00000000)) {
                         tempBackVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) << 1);
                         tempBackVerts[0].vertX = tempX;
                         tempBackVerts[0].vertY = tempY;
                         tempBackVerts[0].vertZ = 0;
                         tempBackVerts[0].vertColor = inFont->fontBackColor;
                         tempBackVerts[1].vertX = tempX + fontCharWidth(inFont, inString[i]);
                         tempBackVerts[1].vertY = tempY + fontHeight(inFont);
                         tempBackVerts[1].vertZ = 0;
                         tempBackVerts[1].vertColor = inFont->fontBackColor;
                         sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempBackVerts);
                    }
               }
               #endif
               if(inFont->fontType == FONT_TYPE_DEBUG) {
                    tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
                    tempVerts[0].vertX = tempX;
                    tempVerts[0].vertY = tempY;
                    tempVerts[0].vertZ = 0;
                    tempVerts[0].vertU = ((inString[i] & 15) << 3);
                    tempVerts[0].vertV = (((inString[i] >> 4) & 15) << 3);
                    tempVerts[1].vertX = tempX + 8;
                    tempVerts[1].vertY = tempY + 8;
                    tempVerts[1].vertZ = 0;
                    tempVerts[1].vertU = ((inString[i] & 15) << 3) + 8;
                    tempVerts[1].vertV = (((inString[i] >> 4) & 15) << 3) + 8;
                    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               }
               #else
               if(inFont->fontType == FONT_TYPE_DEBUG)
                    pspDebugScreenPutChar(tempX, tempY, inFont->fontColor, (u8)inString[i]);
               #endif
               tempX += (fontCharWidth(inFont, inString[i]) + inFont->fontHSpace);
          }
     }
     
     #if FL_GRAPHICS != 0
     #if FL_TEXTURE != 0
     if((inFont->fontType == FONT_TYPE_TEXTURE) || (inFont->fontType == FONT_TYPE_TEXTURE_MONO))
          sceGuEnable(GU_DEPTH_TEST);
     #endif
     if(inFont->fontType == FONT_TYPE_DEBUG) {
          sceGuEnable(GU_DEPTH_TEST);
          sceGuColor(0xFFFFFFFF);
     }
     #endif
}

void fontDraw2dStringCenter(int inY, Font* inFont, char* inString) {
     char* tempPtr;
     char* tempLineStart = inString;
     int tempY = inY;
     for(tempPtr = inString; tempPtr[0] != 0; tempPtr++) {
          if(tempPtr[0] == '\n') {
               tempPtr[0] = 0;
               fontDraw2dString(((SCREEN_WIDTH - fontStringWidth(inFont, tempLineStart)) >> 1), tempY, inFont, tempLineStart);
               tempY += fontHeight(inFont);
               tempPtr[0] = '\n';
               tempLineStart = &tempPtr[1];
          }
     }
     if(tempLineStart[0] != 0)
          fontDraw2dString(((SCREEN_WIDTH - fontStringWidth(inFont, tempLineStart)) >> 1), tempY, inFont, tempLineStart);
}

/* REPLACED BY A MACRO
void fontDraw2dStringFromRight(int inX, int inY, Font* inFont, char* inString) {
     fontDraw2dString((inX - fontStringWidth(inFont, inString)), inY, inFont, inString);
}*/

#endif
