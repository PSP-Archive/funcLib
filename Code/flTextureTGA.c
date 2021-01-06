#include "flGlobal.h"
#if FL_TEXTURE_TGA != 0
#include <stdlib.h>
#include <string.h>
#include <pspgu.h>

#if FL_INCLUDE_ALL_C == 0
#include "flTextureTGA.h"
#include "flMemory.h"
#include "flFile.h"

#if FL_DEBUG != 0
#include "flDebug.h"
#endif

#endif

typedef struct {
     u8  tgaIdentSize;
     u8  tgaPaletteType;
     u8  tgaImageType;
     u16 tgaPaletteStart;
     u16 tgaPaletteLen;
     u8  tgaPaletteBPP;
     s16 tgaXStart;
     s16 tgaYStart;
     u16 tgaWidth;
     u16 tgaHeight;
     u8  tgaBPP;
     u8  tgaDescriptor;
} tgaFileHeader;

Texture* texLoadTGA(char* inPath) {
     #if FL_FILE != 0
     File* tempFile = fileOpen(inPath, FILE_MODE_READ | FILE_MODE_BINARY);
     #else
     FILE* tempFile = fopen(inPath, "rb");
     #endif
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          debugError("TGA load error.\nFile(%s) cannot be opened", inPath);
          #endif
          return NULL;
     }
     tgaFileHeader tempHeader;
     
     fileRead(&tempHeader.tgaIdentSize, 1, tempFile);
     fileRead(&tempHeader.tgaPaletteType, 1, tempFile);
     fileRead(&tempHeader.tgaImageType, 1, tempFile);
     fileRead(&tempHeader.tgaPaletteStart, 2, tempFile);
     fileRead(&tempHeader.tgaPaletteLen, 2, tempFile);
     fileRead(&tempHeader.tgaPaletteBPP, 1, tempFile);
     fileRead(&tempHeader.tgaXStart, 2, tempFile);
     fileRead(&tempHeader.tgaYStart, 2, tempFile);
     fileRead(&tempHeader.tgaWidth, 2, tempFile);
     fileRead(&tempHeader.tgaHeight, 2, tempFile);
     fileRead(&tempHeader.tgaBPP, 1, tempFile);
     fileRead(&tempHeader.tgaDescriptor, 1, tempFile);
    
     if(tempHeader.tgaIdentSize)
          fileSeek(tempFile, tempHeader.tgaIdentSize, FILE_SEEK_CUR);
     
     if(tempHeader.tgaImageType == 0) {
          #if FL_DEBUG_ERROR != 0
          debugError("No image data included within tga(%s).", inPath);
          #endif
          fileClose(tempFile);
          return NULL;
     }
     
     if(tempHeader.tgaPaletteType & 0xFE) {
          #if FL_DEBUG_ERROR != 0
          debugError("Palette format (%i) unsupported in tga file (%s).", tempHeader.tgaPaletteType, inPath);
          #endif
          fileClose(tempFile);
          return NULL;
     }
     
     u8 tempBPP = (tempHeader.tgaPaletteType ? tempHeader.tgaPaletteBPP : tempHeader.tgaBPP);
     if((tempBPP != 15) && (tempBPP != 16) && (tempBPP != 32) && (tempBPP != 24) && (tempBPP != 8)) {
          #if FL_DEBUG_ERROR != 0
          debugError("BPP(%i) value unsupported in tga file (%s).", tempBPP, inPath);
          #endif
          fileClose(tempFile);
          return NULL;
     }
     if(tempHeader.tgaPaletteType && (tempHeader.tgaBPP != 1)  && (tempHeader.tgaBPP != 2) && (tempHeader.tgaBPP != 4) && (tempHeader.tgaBPP != 8)) {
          #if FL_DEBUG_ERROR != 0
          debugError("BPP(%i) value unsupported for palettes in tga file (%s).", tempBPP, inPath);
          #endif
          fileClose(tempFile);
          return NULL;
     }
     if((tempBPP == 8) && (tempHeader.tgaPaletteType || ((tempHeader.tgaImageType & 3) != 3))) {
          #if FL_DEBUG_ERROR != 0
          debugError("Error in tga file (%s), %i bits per pixel in color/shade data is only supported for grayscale images.", inPath, tempBPP);
          #endif
          fileClose(tempFile);
          return NULL;
     }
     if(((tempHeader.tgaImageType & 3) == 3) && (tempBPP != 8)) {
          #if FL_DEBUG_ERROR != 0
          debugError("Error in tga file (%s), only 8 bits per pixel is only supported for grayscale images, not %i.", inPath, tempBPP);
          #endif
          fileClose(tempFile);
          return NULL;
     }
     
     u8 tempByPP = ((tempHeader.tgaBPP + 1) >> 3);
     
     u8 tempPixelFormat = 0;
     u8 tempPaletteFormat = 0;
     if(tempHeader.tgaPaletteType) {
          if((tempHeader.tgaBPP <= 4) && (tempHeader.tgaBPP != 3)) {
               tempPixelFormat = GU_PSM_T4;
          } else if(tempHeader.tgaBPP == 8) {
               tempPixelFormat = GU_PSM_T8;
          }
          if(tempHeader.tgaPaletteBPP > 16) {
               tempPaletteFormat = GU_PSM_8888;
          } else if(tempHeader.tgaPaletteBPP == 16) {
               if((tempHeader.tgaDescriptor & 0x0F) == 0)
                    tempPaletteFormat = GU_PSM_5650;
               else if((tempHeader.tgaDescriptor & 0x0F) == 1)
                    tempPaletteFormat = GU_PSM_5551;
               else if((tempHeader.tgaDescriptor & 0x0F) == 4)
                    tempPaletteFormat = GU_PSM_4444;
          } else {
               tempPaletteFormat = GU_PSM_5551;
          }
     } else {
          if(tempHeader.tgaBPP > 16) {
               tempPixelFormat = GU_PSM_8888;
          } else if(tempHeader.tgaBPP == 16) {
               if((tempHeader.tgaDescriptor & 0x0F) == 0)
                         tempPixelFormat = GU_PSM_5650;
                    else if((tempHeader.tgaDescriptor & 0x0F) == 1)
                         tempPixelFormat = GU_PSM_5551;
                    else if((tempHeader.tgaDescriptor & 0x0F) == 4)
                         tempPixelFormat = GU_PSM_4444;
          } else {
               tempPixelFormat = GU_PSM_5551;
          }
     }
     
     u32 i, j;
     Palette* tempPalette = NULL;
     #if FL_TEXTURE_PRESERVENONALPHA != 0
     bool tempAlpha = (tempHeader.tgaDescriptor & 0x0F);
     #endif
     if(tempHeader.tgaPaletteType) {
          tempPalette = palCreate(tempHeader.tgaPaletteLen, tempPaletteFormat, false);
          switch(tempHeader.tgaPaletteBPP) {
               case 32:
                    for(i = 0; i < tempHeader.tgaPaletteLen; i++) {
                         fileRead(&tempPalette->palData[(i << 2) + 2], 1, tempFile);
                         fileRead(&tempPalette->palData[(i << 2) + 1], 1, tempFile);
                         fileRead(&tempPalette->palData[(i << 2) + 0], 1, tempFile);
                         fileRead(&tempPalette->palData[(i << 2) + 3], 1, tempFile);
                    }
                    break;
               case 24:
                    #if FL_TEXTURE_PRESERVENONALPHA != 0
                    tempAlpha = false;
                    #endif
                    for(i = 0; i < tempHeader.tgaPaletteLen; i++) {
                         fileRead(&tempPalette->palData[(i << 2) + 2], 1, tempFile);
                         fileRead(&tempPalette->palData[(i << 2) + 1], 1, tempFile);
                         fileRead(&tempPalette->palData[(i << 2) + 0], 1, tempFile);
                         tempPalette->palData[(i << 2) + 3] = 0xFF;
                    }
                    break;
          }
     } else if((tempHeader.tgaImageType & 3) == 3) {
          tempPalette = palGrayscaleT8();
          tempPixelFormat = GU_PSM_T8;
          tempPaletteFormat = tempPalette->palPixelFormat;
          #if FL_TEXTURE_PRESERVENONALPHA != 0
          tempAlpha = false;
          #endif
     }
     
     u32 tempDataSize = ((tempHeader.tgaWidth * tempHeader.tgaHeight * ((tempHeader.tgaBPP + 1) & ~7)) >> 3);
     u8* tempData = (u8*)memQalloc(tempDataSize);
     if(!tempData) {
          #if FL_DEBUG_ERROR != 0
          debugError("Couldn't create temporary data, while loading \"%s\".\nOut of memory.", inPath);
          #endif
          palFree(tempPalette);
          fileClose(tempFile);
          return NULL;
     }
     
     u8 tempChar;
     u32 tempPtr = 0;
     u32 tempPixel;
     if(tempHeader.tgaImageType & 8) {
          while(tempPtr < (tempHeader.tgaWidth * tempHeader.tgaHeight)) {
               fileRead(&tempChar, 1, tempFile);
               if(tempChar & 0x80) {
                    fileRead(&tempPixel, tempByPP, tempFile);
                    for(i = 0; i <= (tempChar & 0x7F); i++)
                         memCopy(&tempData[(tempPtr + i) * tempByPP], &tempPixel, tempByPP);
               } else {
                    for(i = 0; i <= (tempChar & 0x7F); i++)
                         fileRead(&tempData[(tempPtr + i) * tempByPP], tempByPP, tempFile);
               }
               tempPtr += ((tempChar & 0x7F) + 1);
          }
     } else {
          fileRead(tempData, tempDataSize, tempFile);
     }
     fileClose(tempFile);

     u32 tempDataLineSize = ((tempHeader.tgaWidth * ((tempHeader.tgaBPP + 1) & ~7)) >> 3);     
     if(!(tempHeader.tgaDescriptor & 0x20)) {
          u8* tempDataLine = (u8*)memQalloc(tempDataLineSize);
          if(!tempDataLine) {
               #if FL_DEBUG_ERROR != 0
               debugError("Couldn't create temporary data cache, while loading \"%s\".\nOut of memory.", inPath);
               #endif
               palFree(tempPalette);
               memFree(tempData);
               return NULL;
          }
          for(i = 0; i < (tempHeader.tgaHeight >> 1); i++) {
               memCopy(tempDataLine, &tempData[i * tempDataLineSize], tempDataLineSize);
               memCopy(&tempData[i * tempDataLineSize], &tempData[((tempHeader.tgaHeight - 1) - i) * tempDataLineSize], tempDataLineSize);
               memCopy(&tempData[((tempHeader.tgaHeight - 1) - i) * tempDataLineSize], tempDataLine, tempDataLineSize);
          }
          memFree(tempDataLine);
     }
     if(tempHeader.tgaDescriptor & 0x10) {
          for(j = 0; j < tempHeader.tgaHeight; j++) {
               for(i = 0; i < (tempHeader.tgaWidth >> 1); i++) {
                    memCopy(&tempPixel, &tempData[(j * tempDataLineSize) + (i * tempByPP)], tempByPP);
                    memCopy(&tempData[(j * tempDataLineSize) + (i * tempByPP)], &tempData[(j * tempDataLineSize) + (((tempHeader.tgaWidth - 1) - i) * tempByPP)], tempByPP);
                    memCopy(&tempData[(j * tempDataLineSize) + (((tempHeader.tgaWidth - 1) - i) * tempByPP)], &tempPixel, tempByPP);
               }
          }
     }
     
     Texture* tempOut = texCreate(tempHeader.tgaWidth, tempHeader.tgaHeight, tempPixelFormat);
     if(!tempOut) {
          #if FL_DEBUG_ERROR != 0
          debugError("Couldn't create texture struct, while loading \"%s\".\nProbably out of memory.", inPath);
          #endif
          palFree(tempPalette);
          memFree(tempData);
          return NULL;
     }
     #if FL_TEXTURE_PRESERVENONALPHA != 0
     tempOut->texAlpha = tempAlpha;
     #endif
     
     if(tempHeader.tgaPaletteType) {
          tempOut->texPalette = tempPalette;
          if(tempHeader.tgaBPP == 8) {
               for(j = 0; j < tempHeader.tgaHeight; j++) {
                    for(i = 0; i < tempHeader.tgaWidth; i++) {
                         tempOut->texData[((j * tempOut->texDataWidth) + i)] = tempData[(j * tempDataLineSize) + i];
                    }
               }
          } else if(tempHeader.tgaBPP == 4) {
               for(j = 0; j < tempHeader.tgaHeight; j++) {
                    for(i = 0; i < ((tempHeader.tgaWidth + 1) >> 1); i++) {
                         tempOut->texData[((j * tempOut->texDataWidth) + i)] = tempData[(j * tempDataLineSize) + i];
                    }
               }
          }
     } else {
          if((tempHeader.tgaImageType & 3) == 3) {
               if(tempHeader.tgaBPP == 8) {
                    for(j = 0; j < tempHeader.tgaHeight; j++) {
                         for(i = 0; i < tempHeader.tgaWidth; i++) {
                              tempOut->texData[((j * tempOut->texDataWidth) + i)] = tempData[(j * tempDataLineSize) + i];
                         }
                    }
               }
          } else {
               if((tempHeader.tgaBPP == 24) && ((tempHeader.tgaDescriptor & 0x0F) == 0)) {
                    #if FL_TEXTURE_PRESERVENONALPHA != 0
                    tempOut->texAlpha = false;
                    #endif
                    for(j = 0; j < tempHeader.tgaHeight; j++) {
                         for(i = 0; i < tempHeader.tgaWidth; i++) {
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 2) + 0] = tempData[(j * tempDataLineSize) + (i * 3) + 2];
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 2) + 1] = tempData[(j * tempDataLineSize) + (i * 3) + 1];
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 2) + 2] = tempData[(j * tempDataLineSize) + (i * 3) + 0];
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 2) + 3] = 0xFF;
                         }
                    }
               } else if((tempHeader.tgaBPP == 32) && ((tempHeader.tgaDescriptor & 0x0F) == 8)) {
                    for(j = 0; j < tempHeader.tgaHeight; j++) {
                         for(i = 0; i < tempHeader.tgaWidth; i++) {
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 2) + 0] = tempData[(j * tempDataLineSize) + (i << 2) + 2];
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 2) + 1] = tempData[(j * tempDataLineSize) + (i << 2) + 1];
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 2) + 2] = tempData[(j * tempDataLineSize) + (i << 2) + 0];
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 2) + 3] = tempData[(j * tempDataLineSize) + (i << 2) + 3];
                         }
                    }
               } else if((tempHeader.tgaBPP == 15) || ((tempHeader.tgaBPP == 16) && ((tempHeader.tgaDescriptor & 0x0F) == 1))) {
                    for(j = 0; j < tempHeader.tgaHeight; j++) {
                         for(i = 0; i < tempHeader.tgaWidth; i++) {
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 1) + 0] = tempData[(j * tempDataLineSize) + (i << 1) + 1];
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 1) + 1] = tempData[(j * tempDataLineSize) + (i << 1)];
                         }
                    }
               } else if((tempHeader.tgaBPP == 16) && ((tempHeader.tgaDescriptor & 0x0F) == 0)) {
                    #if FL_TEXTURE_PRESERVENONALPHA != 0
                    tempOut->texAlpha = false;
                    #endif
                    for(j = 0; j < tempHeader.tgaHeight; j++) {
                         for(i = 0; i < tempHeader.tgaWidth; i++) {
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 1) + 0] = tempData[(j * tempDataLineSize) + (i << 1) + 1];
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 1) + 1] = tempData[(j * tempDataLineSize) + (i << 1)];
                         }
                    }
               } else if((tempHeader.tgaBPP == 16) && ((tempHeader.tgaDescriptor & 0x0F) == 4)) {
                    for(j = 0; j < tempHeader.tgaHeight; j++) {
                         for(i = 0; i < tempHeader.tgaWidth; i++) {
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 1) + 0] = tempData[(j * tempDataLineSize) + (i << 1) + 1];
                              tempOut->texData[(((j * tempOut->texDataWidth) + i) << 1) + 1] = tempData[(j * tempDataLineSize) + (i << 1)];
                         }
                    }
               }
          }
     }
     
     memFree(tempData);
     return tempOut;
}

bool texSaveTGA(Texture* inTex, char* inPath) {
     if(!inTex) {
          #if FL_DEBUG_ERROR != 0
          debugError("Trying to save NULL texture.");
          #endif
          return false;
     }
     if(texPalettized(inTex)) {
          if(!inTex->texPalette) {
               #if FL_DEBUG_WARNING != 0
               debugError("Trying to save palettized texture without palette.");
               #endif
               return false;
          }
          if(inTex->texPalette->palPixelFormat != GU_PSM_8888) {
               #if FL_DEBUG_WARNING != 0
               debugWarning("Only 24/32 bit textures are supported for saving at the minute.");
               #endif
               return false;
          }
     } else if(inTex->texPixelFormat != GU_PSM_8888) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Only 32 bit textures are supported for saving at the minute.");
          #endif
          return false;
     }
     
     #if FL_GRAPHICS != 0
     bool tempReswizzle = false;
     if(inTex->texSwizzled) {
          if(texUnswizzle(inTex)) {
               tempReswizzle = true;
          } else {
               #if FL_DEBUG_WARNING != 0
               debugWarning("Can't un-swizzle texture for saving.");
               #endif
               return false;
          }
     }
     #else
     if(inTex->texSwizzled) {
          #if FL_DEBUG_WARNING != 0
               debugWarning("Can't un-swizzle texture for saving, because flGraphics isn't compiled in.");
               #endif
          return false;
     }
     #endif

     tgaFileHeader tempHeader;
     tempHeader.tgaIdentSize = 0;
     if(texPalettized(inTex)) {
          tempHeader.tgaImageType = 1;
          tempHeader.tgaPaletteType = 1;
          tempHeader.tgaPaletteLen = inTex->texPalette->palEntries;
          tempHeader.tgaPaletteStart = 0;
          switch(inTex->texPalette->palPixelFormat) {
               case GU_PSM_8888:
                    #if FL_TEXTURE_PRESERVENONALPHA != 0
                    if(inTex->texAlpha) {
                         tempHeader.tgaDescriptor = 8;
                         tempHeader.tgaPaletteBPP = 32;
                    } else {
                         tempHeader.tgaDescriptor = 0;
                         tempHeader.tgaPaletteBPP = 24;
                    }
                    #else
                    tempHeader.tgaDescriptor = 8;
                    tempHeader.tgaPaletteBPP = 32;
                    #endif
                    break;
               case GU_PSM_4444:
                    tempHeader.tgaDescriptor = 4;
                    tempHeader.tgaPaletteBPP = 16;
                    break;
               case GU_PSM_5551:
                    tempHeader.tgaDescriptor = 1;
                    tempHeader.tgaPaletteBPP = 16;
                    break;
               case GU_PSM_5650:
                    tempHeader.tgaDescriptor = 0;
                    tempHeader.tgaPaletteBPP = 16;
                    break;
               default:
                    tempHeader.tgaDescriptor = 0;
                    tempHeader.tgaPaletteBPP = 16;
                    break;
          }
          tempHeader.tgaBPP = texBPP(inTex);
     } else {
          tempHeader.tgaImageType = 2;
          tempHeader.tgaPaletteType = 0;
          tempHeader.tgaPaletteBPP = 0;
          tempHeader.tgaPaletteLen = 0;
          tempHeader.tgaPaletteStart = 0;
          switch(inTex->texPixelFormat) {
               case GU_PSM_8888:
                    #if FL_TEXTURE_PRESERVENONALPHA != 0
                    if(inTex->texAlpha) {
                         tempHeader.tgaDescriptor = 8;
                         tempHeader.tgaBPP = 32;
                    } else {
                         tempHeader.tgaDescriptor = 0;
                         tempHeader.tgaBPP = 24;
                    }
                    #else
                    tempHeader.tgaDescriptor = 8;
                    tempHeader.tgaBPP = 32;
                    #endif
                    break;
               case GU_PSM_4444:
                    tempHeader.tgaDescriptor = 4;
                    tempHeader.tgaBPP = 16;
                    break;
               case GU_PSM_5551:
                    tempHeader.tgaDescriptor = 1;
                    tempHeader.tgaBPP = 16;
                    break;
               case GU_PSM_5650:
                    tempHeader.tgaDescriptor = 0;
                    tempHeader.tgaBPP = 16;
                    break;
               default:
                    tempHeader.tgaDescriptor = 0;
                    tempHeader.tgaBPP = texBPP(inTex);
                    break;
          }
     }
     tempHeader.tgaXStart = 0;
     tempHeader.tgaYStart = 0;
     tempHeader.tgaWidth = inTex->texWidth;
     tempHeader.tgaHeight = inTex->texHeight;
     
     #if FL_FILE != 0
     File* tempFile = fileOpen(inPath, FILE_MODE_WRITE | FILE_MODE_BINARY);
     #else
     FILE* tempFile = fopen(inPath, "wb");
     #endif
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          debugError("Cannot open TGA file(%s) for writing.", inPath);
          #endif
          return false;
     }
     fileWrite(&tempHeader.tgaIdentSize, 1, tempFile);
     fileWrite(&tempHeader.tgaPaletteType, 1, tempFile);
     fileWrite(&tempHeader.tgaImageType, 1, tempFile);
     fileWrite(&tempHeader.tgaPaletteStart, 2, tempFile);
     fileWrite(&tempHeader.tgaPaletteLen, 2, tempFile);
     fileWrite(&tempHeader.tgaPaletteBPP, 1, tempFile);
     fileWrite(&tempHeader.tgaXStart, 2, tempFile);
     fileWrite(&tempHeader.tgaYStart, 2, tempFile);
     fileWrite(&tempHeader.tgaWidth, 2, tempFile);
     fileWrite(&tempHeader.tgaHeight, 2, tempFile);
     fileWrite(&tempHeader.tgaBPP, 1, tempFile);
     fileWrite(&tempHeader.tgaDescriptor, 1, tempFile);
     int i, j;     
     if(texPalettized(inTex)) {
          switch(inTex->texPalette->palPixelFormat) {
               case GU_PSM_8888:
                    #if FL_TEXTURE_PRESERVENONALPHA != 0
                    if(inTex->texAlpha) {
                    #endif
                         for(i = 0; i < tempHeader.tgaPaletteLen; i++) {
                              fileWrite(&inTex->texPalette->palData[(i << 2) + 2], 1, tempFile);
                              fileWrite(&inTex->texPalette->palData[(i << 2) + 1], 1, tempFile);
                              fileWrite(&inTex->texPalette->palData[(i << 2) + 0], 1, tempFile);
                              fileWrite(&inTex->texPalette->palData[(i << 2) + 3], 1, tempFile);
                         }
                    #if FL_TEXTURE_PRESERVENONALPHA != 0
                    } else {
                         for(i = 0; i < tempHeader.tgaPaletteLen; i++) {
                              fileWrite(&inTex->texPalette->palData[(i << 2) + 2], 1, tempFile);
                              fileWrite(&inTex->texPalette->palData[(i << 2) + 1], 1, tempFile);
                              fileWrite(&inTex->texPalette->palData[(i << 2) + 0], 1, tempFile);
                         }
                    }
                    #endif
                    break;
               case GU_PSM_4444:
                    break;
               case GU_PSM_5650:
                    break;
               case GU_PSM_5551:
                    break;
          }
          for(j = (inTex->texHeight - 1); j >= 0; j--)
               fileWrite(&inTex->texData[(j * inTex->texDataWidth * texBPP(inTex)) >> 3], ((inTex->texWidth * texBPP(inTex)) >> 3), tempFile);
     } else {
          switch(inTex->texPixelFormat) {
               case GU_PSM_8888:
                    #if FL_TEXTURE_PRESERVENONALPHA != 0
                    if(inTex->texAlpha) {
                    #endif
                         for(j = (inTex->texHeight - 1); j >= 0; j--) {
                              for(i = 0; i < inTex->texWidth; i++) {
                                   fileWrite(&inTex->texData[(((j * inTex->texDataWidth) + i) << 2) + 2], 1, tempFile);
                                   fileWrite(&inTex->texData[(((j * inTex->texDataWidth) + i) << 2) + 1], 1, tempFile);
                                   fileWrite(&inTex->texData[(((j * inTex->texDataWidth) + i) << 2) + 0], 1, tempFile);
                                   fileWrite(&inTex->texData[(((j * inTex->texDataWidth) + i) << 2) + 3], 1, tempFile);
                              }
                         }
                    #if FL_TEXTURE_PRESERVENONALPHA != 0
                    } else {
                         for(j = (inTex->texHeight - 1); j >= 0; j--) {
                              for(i = 0; i < inTex->texWidth; i++) {
                                   fileWrite(&inTex->texData[(((j * inTex->texDataWidth) + i) << 2) + 2], 1, tempFile);
                                   fileWrite(&inTex->texData[(((j * inTex->texDataWidth) + i) << 2) + 1], 1, tempFile);
                                   fileWrite(&inTex->texData[(((j * inTex->texDataWidth) + i) << 2) + 0], 1, tempFile);
                              }
                         }
                    }
                    #endif
                    break;
               case GU_PSM_4444:
                    break;
               case GU_PSM_5650:
                    break;
               case GU_PSM_5551:
                    break;
          }
     }
     
     fileClose(tempFile);
     
     #if FL_GRAPHICS != 0
     if(tempReswizzle)
          texSwizzle(inTex);
     #endif
     
     return true;
}

#endif
