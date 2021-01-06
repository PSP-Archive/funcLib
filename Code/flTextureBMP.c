#include "flGlobal.h"
#if FL_TEXTURE_BMP != 0
#include <stdlib.h>
#include <string.h>
#include <pspgu.h>

#if FL_INCLUDE_ALL_C == 0
#include "flTextureBMP.h"
#include "flMemory.h"
#include "flFile.h"

#if FL_DEBUG != 0
#include "flDebug.h"
#endif
#endif

#include "flTextureBMPWinPal.c"

typedef struct {
     u8  bmpSignature[2];
     u32 bmpFileSize;
     s16 bmpReserved[2];
     u32 bmpDataOffset;
} bmpHeader;

/* THESE STRUCTS ARE KEPT HERE FOR REFERENCE

typedef struct {
    u16 Type;
    u16 Width;
    u16 Height;
    u16 bmpByteWidth;
    u8  bmpPlanes;
    u8  bmpBPP;
} bmpInfoHeaderWV1;

typedef struct {
     u32 bmpSize;
     u16 bmpWidth;
     u16 bmpHeight;
     u16 bmpPlanes;
     u16 bmpBPP;
} bmpInfoHeaderWV2;

typedef struct {
     u32 bmpSize;
     s32 bmpWidth;
     s32 bmpHeight;
     u16 bmpPlanes;
     u16 bmpBPP;
     u32 bmpCompression;
     u32 bmpImageSize;
     s32 bmpXResolution;
     s32 bmpYResolution;
     u32 bmpColorsUsed;
     u32 bmpColorsImportant;
} bmpInfoHeaderWV3;

typedef struct {
     u32 bmpSize;
     s32 bmpWidth;
     s32 bmpHeight;
     u16 bmpPlanes;
     u16 bmpBPP;
     u32 bmpCompression;
     u32 bmpSizeImage;
     s32 bmpXResolution;
     s32 bmpYResolution;
     u32 bmpColorsUsed;
     u32 bmpColorsImportant;
     u32 bmpRedMask;
     u32 bmpGreenMask;
     u32 bmpBlueMask;
     u32 bmpAlphaMask;
     u32 bmpCSType;
     s32 bmpEndpoints[9];
     u32 bmpGammaRed;
     u32 bmpGammaGreen;
     u32 bmpGammaBlue;
} bmpInfoHeaderWV4;

typedef struct { 
     u32 bmpSize;
     s32 bmpWidth;
     s32 bmpHeight;
     u16 bmpPlanes;
     u16 bmpBPP;
     u32 bmpCompression;
     u32 bmpImageSize;
     s32 bmpXResolution;
     s32 bmpYResolution;
     u32 bmpColorsUsed;
     u32 bmpColorsImportant;
     u32 bmpRedMask;
     u32 bmpGreenMask;
     u32 bmpBlueMask;
     u32 bmpAlphaMask;
     u32 bmpCSType;
     s32 bmpEndpoints[9];
     u32 bmpGammaRed;
     u32 bmpGammaGreen;
     u32 bmpGammaBlue;
     u32 bmpIntent;
     u32 bmpProfileData;
     u32 bmpProfileSize;
     u32 bmpReserved;
} bmpInfoHeaderWV5;

typedef struct {
     u32 bmpSize;
     u16 bmpWidth;
     u16 bmpHeight;
     u16 bmpPlanes;
     u16 bmpBPP;
} bmpInfoHeaderOS2V1;

typedef struct {
     u32 bmpSize;
     u32 bmpWidth;
     u32 bmpHeight;
     u16 bmpPlanes;
     u16 bmpBPP;
     u32 bmpCompression;
     u32 bmpImageSize;
     u32 bmpXResolution;
     u32 bmpYResolution;
     u32 bmpColorsUsed;
     u32 bmpColorsImportant;
     u16 bmpUnits;
     u16 bmpReserved;
     u16 bmpRecording;
     u16 bmpRendering;
     u32 bmpSize1;
     u32 bmpSize2;
     u32 bmpColorEncoding;
     u32 bmpIdentifier;
} bmpInfoHeaderOS2V2;*/

typedef struct {
     u16 bmpType;
     u32 bmpSize;
     s32 bmpWidth;
     s32 bmpHeight;
     u16 bmpByteWidth;
     u16 bmpPlanes;
     u16 bmpBPP;
     u32 bmpCompression;
     u32 bmpImageSize;
     s32 bmpXResolution;
     s32 bmpYResolution;
     u32 bmpColorsUsed;
     u32 bmpColorsImportant;
     u32 bmpRedMask;
     u32 bmpGreenMask;
     u32 bmpBlueMask;
     u32 bmpAlphaMask;
     u32 bmpCSType;
     s32 bmpEndpoints[9];
     u32 bmpGammaRed;
     u32 bmpGammaGreen;
     u32 bmpGammaBlue;
     u32 bmpIntent;
     u32 bmpProfileData;
     u32 bmpProfileSize;
     u16 bmpUnits;
     u16 bmpReserved;
     u16 bmpRecording;
     u16 bmpRendering;
     u32 bmpSize1;
     u32 bmpSize2;
     u32 bmpColorEncoding;
     u32 bmpIdentifier;
} bmpInfoHeaderGeneric;

#define BMP_WIN_COMPRESSION_RGB       0
#define BMP_WIN_COMPRESSION_RLE8      1
#define BMP_WIN_COMPRESSION_RLE4      2
#define BMP_WIN_COMPRESSION_BITFIELDS 3
#define BMP_WIN_COMPRESSION_JPEG      4
#define BMP_WIN_COMPRESSION_PNG       5

#define BMP_OS2_COMPRESSION_NONE      0
#define BMP_OS2_COMPRESSION_RLE8      1
#define BMP_OS2_COMPRESSION_RLE4      2
#define BMP_OS2_COMPRESSION_HUFF1D    3
#define BMP_OS2_COMPRESSION_RLE24     4

#define BMP_GEN_COMPRESSION_RGB       0
#define BMP_GEN_COMPRESSION_RLE4      1
#define BMP_GEN_COMPRESSION_RLE8      2
#define BMP_GEN_COMPRESSION_HUFF1D    3
#define BMP_GEN_COMPRESSION_RLE24     4
#define BMP_GEN_COMPRESSION_BITFIELDS 5
#define BMP_GEN_COMPRESSION_JPEG      6
#define BMP_GEN_COMPRESSION_PNG       7

#define BMP_HEADER_OS2V1 12
#define BMP_HEADER_OS2V2 64
#define BMP_HEADER_WV1   10
#define BMP_HEADER_WV2   12
#define BMP_HEADER_WV3   40
#define BMP_HEADER_WV4   108
#define BMP_HEADER_WV5   124


Texture* texLoadBMP(char* inPath) {
     #if FL_FILE != 0
     File* tempFile = fileOpen(inPath, FILE_MODE_READ | FILE_MODE_BINARY);
     #else
     FILE* tempFile = fopen(inPath, "rb");
     #endif
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          char tempString[256];
          sprintf(tempString, "BMP load error (%s).\nFile cannot be opened", inPath);
          debugError(tempString);
          #endif
          return NULL;
     }
     
     bmpHeader tempHeader;
     fileRead(tempHeader.bmpSignature, 2, tempFile);
     fileRead(&tempHeader.bmpFileSize, 4, tempFile);
     fileRead(tempHeader.bmpReserved, 4, tempFile);
     fileRead(&tempHeader.bmpDataOffset, 4, tempFile);
     
     if((tempHeader.bmpSignature[0] != 'B') || (tempHeader.bmpSignature[1] != 'M')) {
          fileClose(tempFile);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Only BM type bitmaps are supported, not icons, pointers, arrays, etc.");
          #endif
          return NULL;
     }
     
     int i, j, k;
     
     bmpInfoHeaderGeneric tempInfoHeader;
     fileRead(&tempInfoHeader.bmpSize, 4, tempFile);
     tempInfoHeader.bmpType = 0;
     tempInfoHeader.bmpSize = tempInfoHeader.bmpSize;
     tempInfoHeader.bmpWidth = 0;
     tempInfoHeader.bmpHeight = 0;
     tempInfoHeader.bmpByteWidth = 0;
     tempInfoHeader.bmpPlanes = 1;
     tempInfoHeader.bmpBPP = 24;
     tempInfoHeader.bmpCompression = 0;
     tempInfoHeader.bmpImageSize = 0;
     tempInfoHeader.bmpXResolution = 0;
     tempInfoHeader.bmpYResolution = 0;
     tempInfoHeader.bmpColorsUsed = 0;
     tempInfoHeader.bmpColorsImportant = 0;
     tempInfoHeader.bmpRedMask = 0;
     tempInfoHeader.bmpGreenMask = 0;
     tempInfoHeader.bmpBlueMask = 0;
     tempInfoHeader.bmpAlphaMask = 0xFF000000;
     tempInfoHeader.bmpCSType = 1;
     for(i = 0; i < 9; i++)
          tempInfoHeader.bmpEndpoints[i] = 0;
     tempInfoHeader.bmpGammaRed = 0;
     tempInfoHeader.bmpGammaGreen = 0;
     tempInfoHeader.bmpGammaBlue = 0;
     tempInfoHeader.bmpIntent = 0;
     tempInfoHeader.bmpProfileData = 0;
     tempInfoHeader.bmpProfileSize = 0;
     tempInfoHeader.bmpReserved = 0;
     tempInfoHeader.bmpUnits = 0;
     tempInfoHeader.bmpRecording = 0;
     tempInfoHeader.bmpRendering = 0;
     tempInfoHeader.bmpSize1 = 0;
     tempInfoHeader.bmpSize2 = 0;
     tempInfoHeader.bmpColorEncoding = 0;
     tempInfoHeader.bmpIdentifier = 0;
     
     if(tempInfoHeader.bmpSize == BMP_HEADER_WV1) {
          fileRead(&tempInfoHeader.bmpType, 2, tempFile);
          if(tempInfoHeader.bmpType) {
               fileClose(tempFile);
               #if FL_DEBUG_WARNING != 0
               debugWarning("Windows bitmap type value not supported.");
               #endif
               return NULL;
          }
          fileRead(&tempInfoHeader.bmpWidth, 2, tempFile);
          fileRead(&tempInfoHeader.bmpHeight, 2, tempFile);
          fileRead(&tempInfoHeader.bmpByteWidth, 2, tempFile);
          fileRead(&tempInfoHeader.bmpPlanes, 1, tempFile);
          fileRead(&tempInfoHeader.bmpBPP, 1, tempFile);
     } else if((tempInfoHeader.bmpSize == BMP_HEADER_WV3) || (tempInfoHeader.bmpSize == BMP_HEADER_WV4) || (tempInfoHeader.bmpSize == BMP_HEADER_WV5)) {
          fileRead(&tempInfoHeader.bmpWidth, 4, tempFile);
          fileRead(&tempInfoHeader.bmpHeight, 4, tempFile);
          fileRead(&tempInfoHeader.bmpPlanes, 2, tempFile);
          fileRead(&tempInfoHeader.bmpBPP, 2, tempFile);
          fileRead(&tempInfoHeader.bmpCompression, 4, tempFile);
          fileRead(&tempInfoHeader.bmpImageSize, 4, tempFile);
          fileRead(&tempInfoHeader.bmpXResolution, 4, tempFile);
          fileRead(&tempInfoHeader.bmpYResolution, 4, tempFile);
          fileRead(&tempInfoHeader.bmpColorsUsed, 4, tempFile);
          fileRead(&tempInfoHeader.bmpColorsImportant, 4, tempFile);
          if(tempInfoHeader.bmpSize >= BMP_HEADER_WV4) {
               fileRead(&tempInfoHeader.bmpRedMask, 4, tempFile);
               fileRead(&tempInfoHeader.bmpGreenMask, 4, tempFile);
               fileRead(&tempInfoHeader.bmpBlueMask, 4, tempFile);
               fileRead(&tempInfoHeader.bmpAlphaMask, 4, tempFile);
               fileRead(&tempInfoHeader.bmpCSType, 4, tempFile);
               fileRead(tempInfoHeader.bmpEndpoints, 36, tempFile);
               fileRead(&tempInfoHeader.bmpGammaRed, 4, tempFile);
               fileRead(&tempInfoHeader.bmpGammaGreen, 4, tempFile);
               fileRead(&tempInfoHeader.bmpGammaBlue, 4, tempFile);
               if(tempInfoHeader.bmpSize == BMP_HEADER_WV5) {
                         fileRead(&tempInfoHeader.bmpIntent, 4, tempFile);
                         fileRead(&tempInfoHeader.bmpProfileData, 4, tempFile);
                         fileRead(&tempInfoHeader.bmpProfileSize, 4, tempFile);
                         fileRead(&tempInfoHeader.bmpReserved, 4, tempFile);
               }
          }
          if((tempInfoHeader.bmpCompression == 3) && (tempInfoHeader.bmpSize == BMP_HEADER_WV3)) {
               fileRead(&tempInfoHeader.bmpRedMask, 4, tempFile);
               fileRead(&tempInfoHeader.bmpGreenMask, 4, tempFile);
               fileRead(&tempInfoHeader.bmpBlueMask, 4, tempFile);
               tempInfoHeader.bmpCompression += 2;
          } else if(tempInfoHeader.bmpCompression >= 3) {
               fileClose(tempFile);
               #if FL_DEBUG_WARNING != 0
               switch(tempInfoHeader.bmpCompression) {
                    case BMP_WIN_COMPRESSION_JPEG:
                         debugWarning("JPEG compression not supported.");
                         return NULL;
                    case BMP_WIN_COMPRESSION_PNG:
                         debugWarning("PNG compression not supported within bitmaps.");
                         return NULL;
               }
               debugWarning("Unknown bitmap compression type.");
               #endif
               return NULL;
               //tempInfoHeader.bmpCompression += 2;
          }
     } else if(tempInfoHeader.bmpSize == BMP_HEADER_OS2V1) {
          fileRead(&tempInfoHeader.bmpWidth, 2, tempFile);
          fileRead(&tempInfoHeader.bmpHeight, 2, tempFile);
          fileRead(&tempInfoHeader.bmpPlanes, 2, tempFile);
          fileRead(&tempInfoHeader.bmpBPP, 2, tempFile);
     } else if(tempInfoHeader.bmpSize == BMP_HEADER_OS2V2) {
          fileRead(&tempInfoHeader.bmpWidth, 4, tempFile);
          fileRead(&tempInfoHeader.bmpHeight, 4, tempFile);
          fileRead(&tempInfoHeader.bmpPlanes, 2, tempFile);
          fileRead(&tempInfoHeader.bmpBPP, 2, tempFile);
          fileRead(&tempInfoHeader.bmpCompression, 4, tempFile);
          fileRead(&tempInfoHeader.bmpImageSize, 4, tempFile);
          fileRead(&tempInfoHeader.bmpXResolution, 4, tempFile);
          fileRead(&tempInfoHeader.bmpYResolution, 4, tempFile);
          fileRead(&tempInfoHeader.bmpColorsUsed, 4, tempFile);
          fileRead(&tempInfoHeader.bmpColorsImportant, 4, tempFile);
          fileRead(&tempInfoHeader.bmpUnits, 2, tempFile);
          fileRead(&tempInfoHeader.bmpReserved, 2, tempFile);
          fileRead(&tempInfoHeader.bmpRecording, 2, tempFile);
          fileRead(&tempInfoHeader.bmpRendering, 2, tempFile);
          fileRead(&tempInfoHeader.bmpSize1, 4, tempFile);
          fileRead(&tempInfoHeader.bmpSize2, 4, tempFile);
          fileRead(&tempInfoHeader.bmpColorEncoding,4, tempFile);
          fileRead(&tempInfoHeader.bmpIdentifier, 4, tempFile);
          if(tempInfoHeader.bmpCompression == 3) {
               fileClose(tempFile);
               #if FL_DEBUG_WARNING != 0
               debugWarning("Huffman 1D compression not supported.");
               #endif
               return NULL;
          } else if(tempInfoHeader.bmpCompression >= 5) {
               fileClose(tempFile);
               #if FL_DEBUG_WARNING != 0
               debugWarning("Unknown bitmap compression type.");
               #endif
               return NULL;
          }
     } else {
          fileClose(tempFile);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Bitmap header type invalid.");
          #endif
          return NULL;
     }
     
     if((!tempInfoHeader.bmpWidth) || (!tempInfoHeader.bmpHeight)) {
          fileClose(tempFile);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Bitmap dimensions invalid.");
          #endif
          return NULL;
     }
     
     if((tempInfoHeader.bmpColorEncoding) || (tempInfoHeader.bmpCSType != 1)) {
          fileClose(tempFile);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Invalid color model / color space.");
          #endif
          return NULL;
     }
     if(tempInfoHeader.bmpRecording) {
          fileClose(tempFile);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Invalid recording mode.");
          #endif
          return NULL;
     }
     if(tempInfoHeader.bmpPlanes & 0xFE) {
          fileClose(tempFile);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Multiple color planes not supported.");
          #endif
          return NULL;
     }
     
     bool tempVFlip = !(tempInfoHeader.bmpHeight < 0);
     bool tempHFlip = (tempInfoHeader.bmpWidth < 0);
     tempInfoHeader.bmpWidth = pos(tempInfoHeader.bmpWidth);
     tempInfoHeader.bmpHeight = pos(tempInfoHeader.bmpHeight);

     if(tempInfoHeader.bmpBPP == 1) {
          while(tempInfoHeader.bmpWidth & 7)
               tempInfoHeader.bmpWidth++;
     } else if(tempInfoHeader.bmpBPP == 2) {
          while(tempInfoHeader.bmpWidth & 3)
               tempInfoHeader.bmpWidth++;
     } else if(tempInfoHeader.bmpBPP == 4) {
          if(tempInfoHeader.bmpWidth & 1)
               tempInfoHeader.bmpWidth++;
     }

     if(!tempInfoHeader.bmpByteWidth) {
          if(tempInfoHeader.bmpSize == BMP_HEADER_WV1) {
               tempInfoHeader.bmpByteWidth = (tempInfoHeader.bmpWidth * tempInfoHeader.bmpBPP);
               while(tempInfoHeader.bmpByteWidth & 7)
                    tempInfoHeader.bmpByteWidth++;
               tempInfoHeader.bmpByteWidth >>= 3;
          } else {
               tempInfoHeader.bmpByteWidth = (tempInfoHeader.bmpWidth * tempInfoHeader.bmpBPP);
               while(tempInfoHeader.bmpByteWidth & 7)
                    tempInfoHeader.bmpByteWidth++;
               tempInfoHeader.bmpByteWidth >>= 3;
               while(tempInfoHeader.bmpByteWidth & 3)
                    tempInfoHeader.bmpByteWidth++;
          }
     }
     
     if(!tempInfoHeader.bmpImageSize)
          tempInfoHeader.bmpImageSize = ((tempInfoHeader.bmpWidth * tempInfoHeader.bmpHeight * tempInfoHeader.bmpBPP) >> 3);
     
     int tempPixelFormat;
     #if FL_TEXTURE_PRESERVENONALPHA != 0
     bool tempAlpha = false;
     #endif
     switch(tempInfoHeader.bmpBPP) {
          case 1:
          case 2:
          case 4:
               tempPixelFormat = GU_PSM_T4;
               break;
          case 8:
               tempPixelFormat = GU_PSM_T8;
               break;
          case 16:
               if(tempInfoHeader.bmpSize >= BMP_HEADER_WV4) {
                    if(tempInfoHeader.bmpAlphaMask == 0x0000F000) {
                         tempPixelFormat = GU_PSM_4444;
                         #if FL_TEXTURE_PRESERVENONALPHA != 0
                         tempAlpha = true;
                         #endif
                    } else if(tempInfoHeader.bmpAlphaMask == 0x00008000) {
                         tempPixelFormat = GU_PSM_5551;
                         #if FL_TEXTURE_PRESERVENONALPHA != 0
                         tempAlpha = true;
                         #endif
                    } else {
                         tempPixelFormat = GU_PSM_5650;
                    }
               } else {
                    tempPixelFormat = GU_PSM_5650;
               }
               break;
          case 24:
               tempPixelFormat = GU_PSM_8888;
               break;
          case 32:
               tempPixelFormat = GU_PSM_8888;
               #if FL_TEXTURE_PRESERVENONALPHA != 0
               tempAlpha = true;
               #endif
               break;
          default:
               fileClose(tempFile);
               #if FL_DEBUG_WARNING != 0
               debugWarning("BPP not supported.");
               #endif
               return NULL;
     }
     
     Palette* tempPal = NULL;
     int tempPalEntries = tempInfoHeader.bmpColorsUsed;
     if(!tempPalEntries)
          tempPalEntries = (1 << tempInfoHeader.bmpBPP);
     if(tempPixelFormat > GU_PSM_8888) {
          if((tempInfoHeader.bmpSize >= BMP_HEADER_OS2V1) || (tempInfoHeader.bmpBPP == 8))
               tempPal = palCreate(tempPalEntries, GU_PSM_8888, false);
          else if(tempInfoHeader.bmpBPP == 4)
               tempPal = palCreateDefaultColorT4();
          else
               tempPal = palCreateGrayscaleT4();
          if(!tempPal) {
               fileClose(tempFile);
               #if FL_DEBUG_WARNING != 0
               debugWarning("Couldn't create bmp palette, probably out of memory.");
               #endif
               return NULL;
          }
          if(tempInfoHeader.bmpSize >= BMP_HEADER_OS2V1) {
               for(i = 0; i < tempPalEntries; i++) {
                    fileRead(&tempPal->palData[(i << 2) + 0], 1, tempFile);
                    fileRead(&tempPal->palData[(i << 2) + 1], 1, tempFile);
                    fileRead(&tempPal->palData[(i << 2) + 2], 1, tempFile);
                    if(tempInfoHeader.bmpSize > BMP_HEADER_OS2V1)
                         fileRead(&tempPal->palData[(i << 2) + 3], 1, tempFile);
                    tempPal->palData[(i << 2) + 3] = 0xFF;
               }
          } else {
               memCopy(tempPal->palData, bmpWinPal, 0x300);
          }
     }
     
     if(tempHeader.bmpDataOffset != 0)
          fileSeek(tempFile, tempHeader.bmpDataOffset, FILE_SEEK_SET);

     u8* tempData = (u8*)memQalloc((tempInfoHeader.bmpWidth * tempInfoHeader.bmpHeight * tempInfoHeader.bmpBPP) >> 3);
     if(!tempData) {
          fileClose(tempFile);
          if(tempPal)
               palFree(tempPal);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Couldn't create data buffer while loading bmp.\nProbably out of memory.");
          #endif
          return NULL;
     }

     int tempLineSize = ((tempInfoHeader.bmpWidth * tempInfoHeader.bmpBPP) >> 3);
     u32 tempStride = tempLineSize - tempInfoHeader.bmpByteWidth;     
     if(tempInfoHeader.bmpCompression) {
          i = 0; j = 0; k = 0;
          u8 tempRleBlock[4];
          if(tempInfoHeader.bmpCompression == BMP_GEN_COMPRESSION_RLE4) {
               while(flRunning && (fileRead(tempRleBlock, 2, tempFile) == 2)) {
                    if(!tempRleBlock[0]) {
                         if(!tempRleBlock[1]) {
                              j++;
                              if(j >= tempInfoHeader.bmpHeight)
                                   break;
                              continue;
                         } else if(tempRleBlock[1] == 1) {
                              break;
                         } else if(tempRleBlock[1] == 2) {
                              fileRead(tempRleBlock, 2, tempFile);
                              j -= tempRleBlock[0];
                              i += ((tempRleBlock[1] + 1) >> 1);
                              if(i >= tempInfoHeader.bmpWidth) {
                                   j++;
                                   i -= tempLineSize;
                              }
                              if((j < 0) || (j >= tempInfoHeader.bmpHeight)) {
                                   #if FL_DEBUG_WARNING != 0
                                   debugWarning("BMP RLE4 decoding error.");
                                   #endif
                                   break;
                              }
                         } else {
                              for(k = 0; k < ((tempRleBlock[0] + 1) >> 1); k++) {
                                   if(j >= tempInfoHeader.bmpHeight)
                                        break;
                                   fileRead(&tempData[(j * tempLineSize) + i], 1, tempFile);
                                   if((k == (tempRleBlock[0] >> 1)) && (tempRleBlock[0] & 1))
                                        tempData[(j * tempLineSize) + i] &= 0xF0;
                                   i++;
                                   if(i >= tempInfoHeader.bmpWidth) {
                                        j++;
                                        i -= tempLineSize;
                                   }
                              }
                              if(j >= tempInfoHeader.bmpHeight) {
                                   #if FL_DEBUG_WARNING != 0
                                   debugWarning("BMP RLE4 decoding error.");
                                   #endif
                                   break;
                              }
                         }
                    } else {
                         for(k = 0; k < ((tempRleBlock[0] + 1) >> 1); k++) {
                              if(j >= tempInfoHeader.bmpHeight)
                                   break;
                              tempData[(j * tempLineSize) + i] = tempRleBlock[1];
                              if((k == (tempRleBlock[0] >> 1)) && (tempRleBlock[0] & 1))
                                   tempData[(j * tempLineSize) + i] &= 0xF0;
                              i++;
                              if(i >= tempInfoHeader.bmpWidth) {
                                   j++;
                                   i -= tempLineSize;
                              }
                         }
                         if(j >= tempInfoHeader.bmpHeight) {
                              #if FL_DEBUG_WARNING != 0
                              debugWarning("BMP RLE4 decoding error.");
                              #endif
                              break;
                         }
                    }
               }
          } else if(tempInfoHeader.bmpCompression == BMP_GEN_COMPRESSION_RLE8) {
               while(flRunning && (fileRead(tempRleBlock, 2, tempFile) == 2)) {
                    if(!tempRleBlock[0]) {
                         if(!tempRleBlock[1]) {
                              j++;
                              if(j >= tempInfoHeader.bmpHeight)
                                   break;
                              continue;
                         } else if(tempRleBlock[1] == 1) {
                              break;
                         } else if(tempRleBlock[1] == 2) {
                              fileRead(tempRleBlock, 2, tempFile);
                              j -= tempRleBlock[0];
                              i += tempRleBlock[1];
                              if(i >= tempInfoHeader.bmpWidth) {
                                   j++;
                                   i -= tempLineSize;
                              }
                              if((j < 0) || (j >= tempInfoHeader.bmpHeight)) {
                                   #if FL_DEBUG_WARNING != 0
                                   debugWarning("BMP RLE8 decoding error.");
                                   #endif
                                   break;
                              }
                         } else {
                              for(k = 0; k < tempRleBlock[1]; k++) {
                                   if(j >= tempInfoHeader.bmpHeight)
                                        break;
                                   fileRead(&tempData[(j * tempLineSize) + i], 1, tempFile);
                                   i++;
                                   if(i >= tempInfoHeader.bmpWidth) {
                                        j++;
                                        i -= tempLineSize;
                                   }
                              }
                              if(j >= tempInfoHeader.bmpHeight) {
                                   #if FL_DEBUG_WARNING != 0
                                   debugWarning("BMP RLE8 decoding error.");
                                   #endif
                                   break;
                              }
                         }
                    } else {
                         for(k = 0; k < tempRleBlock[0]; k++) {
                              if(j >= tempInfoHeader.bmpHeight)
                                   break;
                              tempData[(j * tempLineSize) + i] = tempRleBlock[1];
                              i++;
                              if(i >= tempInfoHeader.bmpWidth) {
                                   j++;
                                   i -= tempLineSize;
                              }
                         }
                         if(j >= tempInfoHeader.bmpHeight) {
                              #if FL_DEBUG_WARNING != 0
                              debugWarning("BMP RLE8 decoding error.");
                              #endif
                              break;
                         }
                    }
               }
          } else if(tempInfoHeader.bmpCompression == BMP_GEN_COMPRESSION_RLE24) {
               while(flRunning && (fileRead(tempRleBlock, 2, tempFile) == 2)) {
                    if(!tempRleBlock[0]) {
                         if(!tempRleBlock[1]) {
                              j++;
                              if(j >= tempInfoHeader.bmpHeight)
                                   break;
                              continue;
                         } else if(tempRleBlock[1] == 1) {
                              break;
                         } else if(tempRleBlock[1] == 2) {
                              fileRead(tempRleBlock, 2, tempFile);
                              j -= tempRleBlock[0];
                              i += (tempRleBlock[1] * 3);
                              if(i >= tempLineSize) {
                                   j++;
                                   i -= tempLineSize;
                              }
                              if((j < 0) || (j >= tempInfoHeader.bmpHeight)) {
                                   #if FL_DEBUG_WARNING != 0
                                   debugWarning("BMP RLE24 decoding error.");
                                   #endif
                                   break;
                              }
                         } else {
                              for(k = 0; k < tempRleBlock[1]; k++) {
                                   if(j >= tempInfoHeader.bmpHeight)
                                        break;
                                   fileRead(&tempData[(j * tempLineSize) + (i * 3)], 3, tempFile);
                                   i += 3;
                                   if(i >= tempLineSize) {
                                        j++;
                                        i -= tempLineSize;
                                   }
                              }
                              if(j >= tempInfoHeader.bmpHeight) {
                                   #if FL_DEBUG_WARNING != 0
                                   debugWarning("BMP RLE24 decoding error.");
                                   #endif
                                   break;
                              }
                         }
                    } else {
                         fileRead(&tempRleBlock[2], 2, tempFile);
                         for(k = 0; k < tempRleBlock[0]; k++) {
                              if(j >= tempInfoHeader.bmpHeight)
                                   break;
                              tempData[(j * tempLineSize) + i + 0] = tempRleBlock[1];
                              tempData[(j * tempLineSize) + i + 1] = tempRleBlock[2];
                              tempData[(j * tempLineSize) + i + 2] = tempRleBlock[3];
                              i += 3;
                              if(i >= tempLineSize) {
                                   j++;
                                   i -= tempLineSize;
                              }
                         }
                         if(j >= tempInfoHeader.bmpHeight) {
                              #if FL_DEBUG_WARNING != 0
                              debugWarning("BMP RLE24 decoding error.");
                              #endif
                              break;
                         }
                    }
               }
          }
     } else {
          for(i = (tempInfoHeader.bmpHeight - 1); i >= 0; i--) {
               fileRead(&tempData[i * tempLineSize], tempLineSize, tempFile);
               if(tempStride)
                    fileSeek(tempFile, tempStride, SEEK_CUR);
          }
          tempVFlip = !tempVFlip;
     }

     fileClose(tempFile);

     u8* tempLine = NULL;
     if(tempVFlip) {
          tempLine = memAlloc(tempLineSize);
          if(!tempLine) {
               if(tempPal)
                    palFree(tempPal);
               memFree(tempData);
               #if FL_DEBUG_WARNING != 0
               debugWarning("Couldn't create VFlip buffer while loading bmp.\nProbably out of memory.");
               #endif
               return NULL;
          }
          for(i = 0; i < (tempInfoHeader.bmpHeight >> 1); i++) {
               memCopy(tempLine, &tempData[i * tempLineSize], tempLineSize);
               memCopy(&tempData[i * tempLineSize], &tempData[((tempInfoHeader.bmpHeight - 1) - i) * tempLineSize], tempLineSize);
               memCopy(&tempData[((tempInfoHeader.bmpHeight - 1) - i) * tempLineSize], tempLine, tempLineSize);
          }
          memFree(tempLine);
     }
     if(tempHFlip) {
          u8 tempPixel[4];
          for(j = 0; j < tempInfoHeader.bmpHeight; j++) {
               for(i = 0; i < (tempInfoHeader.bmpWidth >> 1); i++) {
                    memCopy(tempPixel, &tempData[(j * tempLineSize) + (i * (tempInfoHeader.bmpBPP >> 3))], (tempInfoHeader.bmpBPP >> 3));
                    memCopy(&tempData[(j * tempLineSize) + (i * (tempInfoHeader.bmpBPP >> 3))], &tempData[(j * tempLineSize) + (((tempInfoHeader.bmpWidth - 1) - i) * (tempInfoHeader.bmpBPP >> 3))], (tempInfoHeader.bmpBPP >> 3));
                    memCopy(&tempData[(j * tempLineSize) + (((tempInfoHeader.bmpWidth - 1) - i) * (tempInfoHeader.bmpBPP >> 3))], tempPixel, (tempInfoHeader.bmpBPP >> 3));
               }
          }
     }
     
     if(tempInfoHeader.bmpRedMask || tempInfoHeader.bmpGreenMask || tempInfoHeader.bmpBlueMask) {
          // TODO - Decode bit fields if they exist
          #if FL_DEBUG_WARNING != 0
          debugWarning("Bitmap contains bit-fields, which are currently unsupported.\nSkipping may cause corruption of colors.");
          #endif
     }
     
     Texture* tempOut = texCreate(tempInfoHeader.bmpWidth, tempInfoHeader.bmpHeight, tempPixelFormat);
     if(!tempOut) {
          if(tempPal)
               palFree(tempPal);
          memFree(tempData);
          fileClose(tempFile);
          #if FL_DEBUG_WARNING != 0
          char tempString[256];
          sprintf(tempString, "Couldn't create texture struct, while loading \"%s\".\nOut of memory.", inPath);
          debugWarning(tempString);
          #endif
          return NULL;
     }
     tempOut->texPalette = tempPal;
     #if FL_TEXTURE_PRESERVENONALPHA != 0
     tempOut->texAlpha = tempAlpha;
     #endif
     
     if((tempInfoHeader.bmpBPP == 32) || (tempInfoHeader.bmpBPP == 16) || (tempInfoHeader.bmpBPP == 8) || (tempInfoHeader.bmpBPP == 4)) {
          for(i = 0; i < tempInfoHeader.bmpHeight; i++)
               memCopy(&tempOut->texData[i * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)], &tempData[i * tempLineSize], tempLineSize);
     } else if(tempInfoHeader.bmpBPP == 24) {
          for(j = 0; j < tempInfoHeader.bmpHeight; j++) {
               for(i = 0; i < tempInfoHeader.bmpWidth; i++) {
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 0] = tempData[(j * tempLineSize) + (i * 3) + 0];
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 1] = tempData[(j * tempLineSize) + (i * 3) + 1];
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 2] = tempData[(j * tempLineSize) + (i * 3) + 2];
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 3] = 0xFF;
               }
          }
     } else if(tempInfoHeader.bmpBPP == 2) {
          for(j = 0; j < tempInfoHeader.bmpHeight; j++) {
               if(tempInfoHeader.bmpSize == BMP_HEADER_WV1) {
                    for(i = 0; i < (tempInfoHeader.bmpWidth >> 2); i++) {
                         tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 1) + 0] = (((tempData[(j * tempLineSize) + i] & 0x03) << 4) / 3);
                         tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 1) + 0] += ((((tempData[(j * tempLineSize) + i] & 0x0C) << 2) / 3) << 4);
                         tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 1) + 1] = ((tempData[(j * tempLineSize) + i] & 0x30) / 3);
                         tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 1) + 1] += ((((tempData[(j * tempLineSize) + i] & 0xC0) >> 2) / 3) << 4);
                    }
               } else {
                    for(i = 0; i < (tempInfoHeader.bmpWidth >> 2); i++) {
                         tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 1) + 0] = (tempData[(j * tempLineSize) + i] & 0x03);
                         tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 1) + 0] += ((tempData[(j * tempLineSize) + i] & 0x0C) >> 2);
                         tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 1) + 1] = ((tempData[(j * tempLineSize) + i] & 0x30) >> 4);
                         tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 1) + 1] += ((tempData[(j * tempLineSize) + i] & 0xC0) >> 6);
                    }
               }
          }
     } else if(tempInfoHeader.bmpBPP == 1) {
          for(j = 0; j < tempInfoHeader.bmpHeight; j++) {
               for(i = 0; i < (tempInfoHeader.bmpWidth >> 3); i++) {
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 0] = (tempData[(j * tempLineSize) + i] & 0x01);
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 0] += ((tempData[(j * tempLineSize) + i] & 0x02) << 3);
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 1] = ((tempData[(j * tempLineSize) + i] & 0x04) >> 2);
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 1] += ((tempData[(j * tempLineSize) + i] & 0x08) << 1);
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 2] = (tempData[(j * tempLineSize) + i] & 0x10);
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 2] += ((tempData[(j * tempLineSize) + i] & 0x20) >> 1);
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 3] = ((tempData[(j * tempLineSize) + i] & 0x40) >> 6);
                     tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 3] += ((tempData[(j * tempLineSize) + i] & 0x80) >> 3);
                     if(tempInfoHeader.bmpSize == BMP_HEADER_WV1) {
                          tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 0] <<= 3;
                          tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 1] <<= 3;
                          tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 2] <<= 3;
                          tempOut->texData[(j * ((tempOut->texDataWidth * texBPP(tempOut)) >> 3)) + (i << 2) + 3] <<= 3;
                     }
               }
          }
     }

     memFree(tempData);
     return tempOut;
}

bool texSaveBMP(Texture* inTex, char* inPath) {
     #if FL_DEBUG_WARNING != 0
     debugWarning("BMP saving is not yet supported.");
     #endif
     return false;
}

#endif
