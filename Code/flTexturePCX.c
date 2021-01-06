#include "flGlobal.h"
#if FL_TEXTURE_PCX != 0
#include <stdlib.h>
#include <string.h>
#include <pspgu.h>

#if FL_INCLUDE_ALL_C == 0
#include "flTexturePCX.h"
#include "flMemory.h"
#include "flFile.h"

#if FL_DEBUG != 0
#include "flDebug.h"
#endif

#endif

typedef struct {
     char  manufacturer;
     char  version;
     char  encoding;
     char  bpp;
     short xMin;
     short yMin;
     short xMax;
     short yMax;
     short horizontalDPI;
     short verticalDPI;
     char  palette[48];
     char  reserved;
     char  colorPlanes;
     short bytesPerLine;
     short paletteType;
     short hScrSize;
     short vScrSize;
     char  filler[54];
} pcxFileHeader;

Texture* texLoadPCX(char* inPath) {
     #if FL_FILE != 0
     File* tempFile = fileOpen(inPath, FILE_MODE_READ | FILE_MODE_BINARY);
     #else
     FILE* tempFile = fopen(inPath, "rb");
     #endif
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          debugError("PCX load error (%s).\nFile cannot be opened", inPath);
          #endif
          return NULL;
     }
     pcxFileHeader tempHeader;
     
     fileRead(&tempHeader.manufacturer, 1, tempFile);
     fileRead(&tempHeader.version, 1, tempFile);
     if((tempHeader.version != 0) && (tempHeader.version != 2) && (tempHeader.version != 5)) {
          #if FL_DEBUG_ERROR != 0
          debugError("PCX file error (%s).\nVersion not supported (%i).\nSupported versions are 0, 2 & 5.", inPath, tempHeader.version);
          #endif
          fileClose(tempFile);
          return NULL;
     }
     fileRead(&tempHeader.encoding, 1, tempFile);
     fileRead(&tempHeader.bpp, 1, tempFile);
     if((tempHeader.bpp != 1) && (tempHeader.bpp != 4) && (tempHeader.bpp != 8) && (tempHeader.bpp != 24)) {
          #if FL_DEBUG_ERROR != 0
          debugError("PCX file error (%s).\nBPP not supported (%i).\nSupported BPPs are 1, 4, 8 & 24.", inPath, tempHeader.bpp);
          #endif
          fileClose(tempFile);
          return NULL;
     }
     
     fileRead(&tempHeader.xMin, 2, tempFile);
     fileRead(&tempHeader.yMin, 2, tempFile);
     fileRead(&tempHeader.xMax, 2, tempFile);
     fileRead(&tempHeader.yMax, 2, tempFile);
     fileRead(&tempHeader.horizontalDPI, 2, tempFile);
     fileRead(&tempHeader.verticalDPI, 2, tempFile);
     fileRead(&tempHeader.palette[0], 48, tempFile);
     fileRead(&tempHeader.reserved, 1, tempFile);
     fileRead(&tempHeader.colorPlanes, 1, tempFile);
     //if((tempHeader.colorPlanes != 3) && (tempHeader.colorPlanes != 4))
     //     tempHeader.colorPlanes = 1;
     fileRead(&tempHeader.bytesPerLine, 2, tempFile);
     fileRead(&tempHeader.paletteType, 2, tempFile);
     fileRead(&tempHeader.hScrSize, 2, tempFile);
     fileRead(&tempHeader.vScrSize, 2, tempFile);
     fileRead(&tempHeader.filler[0], 54, tempFile);
     
     int tempWidth = (tempHeader.xMax - tempHeader.xMin);
     int tempHeight = (tempHeader.yMax - tempHeader.yMin);
     if(tempWidth <= 0)
          tempWidth = 0 - tempWidth;
     if(tempHeight <= 0)
          tempHeight = 0 - tempHeight;          
     tempHeight++;
     tempWidth++;
          
     u8* tempData = (u8*)memQalloc(tempHeader.bytesPerLine * tempHeader.colorPlanes * tempHeight);
     if(!tempData) {
          #if FL_DEBUG_ERROR != 0
          debugError("PCX load error (%s).\nOut of memory.", inPath);
          #endif
          fileClose(tempFile);
          return NULL;
     }
     u8    tempChar;
     u32   tempX = 0;
     u32   tempLine;
     u32   tempPlane;
     u8    tempCount;
     u32   i;
     if(tempHeader.encoding == 1) {
          for(tempLine = 0; tempLine < tempHeight; tempLine++) {
               for(tempPlane = 0; tempPlane < tempHeader.colorPlanes; tempPlane++) {
                    tempX = 0;
                    while(tempX < tempHeader.bytesPerLine) {
                         fileRead(&tempChar, 1, tempFile);
                         if((tempChar & 0xC0) == 0xC0) {
                              tempCount = (tempChar & 0x3F);
                              fileRead(&tempChar, 1, tempFile);
                         } else
                              tempCount = 1;
                         for(i = 0; i < tempCount; i++)
                              tempData[(((tempLine * tempHeader.colorPlanes) + tempPlane) * tempHeader.bytesPerLine) + tempX + i] = tempChar;
                         tempX += tempCount;
                    }
               }
          }
     } else if(tempHeader.encoding == 0) {
          for(tempLine = 0; tempLine < tempHeight; tempLine++) {
               for(tempPlane = 0; tempPlane < tempHeader.colorPlanes; tempPlane++)
                    fileRead(&tempData[((tempLine * tempHeader.colorPlanes) + tempPlane) * tempHeader.bytesPerLine], tempHeader.bytesPerLine, tempFile);
          }
     } else {
          #if FL_DEBUG_ERROR != 0
          debugError("PCX load error (%s).\nEncoding type %i not supported.", inPath, tempHeader.encoding);
          #endif
          fileClose(tempFile);
          return NULL;            
     }
     
     char* tempPalette = tempHeader.palette;
     char tempBigPalette[768];
     if((tempHeader.bpp == 8) && (tempHeader.colorPlanes == 1)) {
          fileSeek(tempFile, -769, FILE_SEEK_END);
          fileRead(&tempChar, 1, tempFile);
          if(tempChar == 12) {
               fileRead(&tempBigPalette[0], 768, tempFile);
               tempPalette = tempBigPalette;
          }
     }
     fileClose(tempFile);
     

     u8 tempPixelFormat = GU_PSM_8888;
     u8 tempRealBPP = (tempHeader.colorPlanes * tempHeader.bpp);
     if(tempRealBPP <= 4) {
          tempPixelFormat = GU_PSM_T4;
     } else if(tempRealBPP == 8) {
          tempPixelFormat = GU_PSM_T8;
     }
     Texture* tempOut = texCreate(tempWidth, tempHeight, tempPixelFormat);
     if(!tempOut) {
          #if FL_DEBUG_ERROR != 0
          debugError("PCX load error (%s).\nCouldn't create texture struct.", inPath);
          #endif
          return NULL;
     }
     #if FL_TEXTURE_PRESERVENONALPHA != 0
     tempOut->texAlpha = false;
     #endif

     
     if(texPalettized(tempOut)) {
          tempOut->texPalette = palCreate((tempPixelFormat == GU_PSM_T4 ? 16 : 256) , GU_PSM_8888, false);
          for(i = 0; i < (1 << tempHeader.bpp); i++) {
               tempOut->texPalette->palData[i << 2] = tempPalette[i * 3];
               tempOut->texPalette->palData[(i << 2) + 1] = tempPalette[(i * 3) + 1];
               tempOut->texPalette->palData[(i << 2) + 2] = tempPalette[(i * 3) + 2];
               tempOut->texPalette->palData[(i << 2) + 3] = 0xFF;
          }
     }
     
     if(tempHeader.colorPlanes <= 1) {
          if(tempHeader.bpp == 1) {
               for(tempLine = 0; tempLine < tempHeight; tempLine++) {
                    for(tempX = 0; tempX < ((tempWidth + 7) >> 3); tempX++) {
                         tempOut->texData[(tempLine * tempOut->texDataWidth) + (tempX << 2)] = ((tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0x80) >> 3) + ((tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0x40) >> 6);
                         tempOut->texData[(tempLine * tempOut->texDataWidth) + (tempX << 2) + 1] = ((tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0x20) >> 1) + ((tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0x10) >> 4);
                         tempOut->texData[(tempLine * tempOut->texDataWidth) + (tempX << 2) + 2] = ((tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0x08) << 1) + ((tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0x04) >> 2);
                         tempOut->texData[(tempLine * tempOut->texDataWidth) + (tempX << 2) + 3] = ((tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0x02) << 3) + (tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0x01);
                    }
               }
          } else if(tempHeader.bpp == 2) {
               for(tempLine = 0; tempLine < tempHeight; tempLine++) {
                    for(tempX = 0; tempX < ((tempWidth + 3) >> 2); tempX++) {
                         tempOut->texData[(tempLine * tempOut->texDataWidth) + (tempX << 1)] = ((tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0xC0) >> 2) + ((tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0x30) >> 4);
                         tempOut->texData[(tempLine * tempOut->texDataWidth) + (tempX << 1) + 1] = ((tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0x0C) << 2) + (tempData[(tempLine * tempHeader.bytesPerLine) + tempX] & 0x03);
                    }
               }
          } else if(tempHeader.bpp == 4) {
               for(tempLine = 0; tempLine < tempHeight; tempLine++) {
                    for(tempX = 0; tempX < ((tempWidth + 1) >> 1); tempX++) {
                         tempOut->texData[(tempLine * tempOut->texDataWidth) + tempX] = tempData[(tempLine * tempHeader.bytesPerLine) + tempX];
                    }
               }               
          } else if(tempHeader.bpp == 8) {
               if(tempPalette == tempHeader.palette) {
                    #if FL_DEBUG_ERROR != 0
                    debugError("PCX load error (%s).\n256 color image doesn't contain a palette.", inPath);
                    #endif
                    memFree(tempData);
                    texFree(tempOut);
                    return NULL;
               }
               for(tempLine = 0; tempLine < tempHeight; tempLine++) {
                    for(tempX = 0; tempX < tempWidth; tempX++) {
                         tempOut->texData[(tempLine * tempOut->texDataWidth) + tempX] =  tempData[(tempLine * tempHeader.bytesPerLine) + tempX];
                    }
               }
          } else {
               #if FL_DEBUG_ERROR != 0
               debugError("PCX load error (%s).\nUnsupported BPP(%i)/ColorPlane(%i) combo.", inPath, tempHeader.bpp, tempHeader.colorPlanes);
               #endif
               memFree(tempData);
               texFree(tempOut);
               return NULL;
          }
     } else if(tempHeader.colorPlanes == 3) {
          if(tempHeader.bpp == 8) {
               for(tempLine = 0; tempLine < tempHeight; tempLine++) {
                    for(tempX = 0; tempX < tempWidth; tempX++) {
                         tempOut->texData[((tempLine * 3) * (tempOut->texDataWidth << 2)) + (tempX << 2)] = tempData[((tempLine * 3) * tempHeader.bytesPerLine) + tempX];
                         tempOut->texData[((tempLine * 3) * (tempOut->texDataWidth << 2)) + (tempX << 2) + 1] = tempData[(((tempLine * 3) + 1) * tempHeader.bytesPerLine) + tempX];
                         tempOut->texData[((tempLine * 3) * (tempOut->texDataWidth << 2)) + (tempX << 2) + 2] = tempData[(((tempLine * 3) + 2) * tempHeader.bytesPerLine) + tempX];
                         tempOut->texData[((tempLine * 3) * (tempOut->texDataWidth << 2)) + (tempX << 2) + 3] = 0xFF;
                    }
               }
          } else {
               #if FL_DEBUG_ERROR != 0
               debugError("PCX load error (%s).\nUnsupported BPP(%i)/ColorPlane(%i) combo.", inPath, tempHeader.bpp, tempHeader.colorPlanes);
               #endif
               memFree(tempData);
               texFree(tempOut);
               return NULL;
          }
     } else {
          #if FL_DEBUG_ERROR != 0
          debugError("PCX load error (%s).\nUnsupported BPP(%i)/ColorPlane(%i) combo.", inPath, tempHeader.bpp, tempHeader.colorPlanes);
          #endif
          memFree(tempData);
          texFree(tempOut);
          return NULL;
     }
     
     memFree(tempData);
     return tempOut;
}

bool texSavePCX(Texture* inTex, char* inPath) {
     #if FL_DEBUG_WARNING != 0
     debugWarning("PCX saving not yet implemented.");
     #endif
     return false;
}

#endif
