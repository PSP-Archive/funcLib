#include "flGlobal.h"
#if FL_TEXTURE
#include <string.h>
#include <stdio.h>
#include <pspgu.h>
#include <pspkernel.h>

#if !FL_INCLUDE_ALL_C
#include "flTexture.h"
#include "flMemory.h"
#include "flColor.h"
#include "flMath.h"
#include "flString.h"
#include "flDebug.h"
#include "flGraphics.h"
#include "flTexturePCX.h"
#include "flTextureTGA.h"
#include "flTextureBMP.h"
#include "flTextureRAW.h"
#endif




// Texture tree prototypes
typedef struct {
     char     texNodePath[244];
     Texture* texNodeData;
     void*    texNodeNext[2];
} texTreeNode;

bool         texTreeAdd(char* inPath, Texture* inTex);
Texture*     texTreeFindTexByPath(char* inPath);
texTreeNode* texTreeFindNodeByTexR(Texture* inTex, texTreeNode* inNode);
char*        texTreeFindPathByTex(Texture* inTex);
texTreeNode* texTreeFindNodeByTex(Texture* inTex);
void         texTreeAddR(texTreeNode* inNode);
void         texTreeDelR(texTreeNode* inNode);
bool         texTreeDelNodeByTex(Texture* inTex);

texTreeNode* texTree = NULL;




#if FL_TEXTURE_ERRORTEX
#if FL_TEXTURE_ERRORTEX_COMPILE
#include "flTexError.c"
#endif
Texture* texError = NULL;
#endif

void* texBound = NULL;
void* palBound = NULL;

Palette* palGrayscaleT8Ptr = NULL;
Palette* palGrayscaleT4Ptr = NULL;
Palette* palDefaultColorT4Ptr = NULL;

static int getNextPower2(int inNum) {
     int tempOut = inNum;
     int tempBits;
     for(tempBits = 0; tempOut; tempBits++)
          tempOut >>= 1;
     tempOut = 1 << tempBits;
     if(tempOut == (inNum << 1))
          tempOut = inNum;
     return tempOut;
}

static inline int getPaletteSize(int inEntries) {
     while(inEntries & 7)
          inEntries++;
     return inEntries;
}

#if FL_GRAPHICS
inline u8 texBPP(Texture* inTex) {
     switch(inTex->texPixelFormat) {
          case GU_PSM_8888:
          case GU_PSM_T32:
               return 32;
          case GU_PSM_4444:
          case GU_PSM_5650:
          case GU_PSM_5551:
          case GU_PSM_T16:
               return 16;
          case GU_PSM_DXT3:
          case GU_PSM_DXT5:
          case GU_PSM_T8:
               return 8;
          case GU_PSM_DXT1:
          case GU_PSM_T4:
               return 4;
     }
     return 0;
}
#endif

inline u8 texPalettized(Texture* inTex) {
     return (inTex->texPixelFormat & 4);
}

inline u8 palBPP(Palette* inPal) {
     #if FL_GRAPHICS
     if(inPal->palPixelFormat > GU_PSM_8888) {
          #if FL_DEBUG_WARNING
          debugWarning("Warning, erroneous palette BPP %u.", (unsigned int)inPal->palPixelFormat);
          #endif
          return 0;
     }
     return graphicsPixelFormatBPP(inPal->palPixelFormat);
     #else
     switch(inPal->palPixelFormat) {
          case GU_PSM_8888:
               return 32;
          case GU_PSM_4444:
          case GU_PSM_5650:
          case GU_PSM_5551:
               return 16;
          case GU_PSM_T32:
          case GU_PSM_T16:
          case GU_PSM_DXT3:
          case GU_PSM_DXT5:
          case GU_PSM_T8:
          case GU_PSM_DXT1:
          case GU_PSM_T4:
          #if FL_DEBUG_WARNING
          debugWarning("Warning, erroneous palette BPP %u.", (unsigned int)inPath->palPixelFormat);
          #endif
          return 0;
     }
     return 0;
     #endif
}

inline void palBind(Palette* inPalette) {
     if(palBound && (inPalette == palBound))
          return;
          
     sceGuClutMode(inPalette->palPixelFormat, 0, 0xFF, 0);
     sceGuClutLoad((inPalette->palEntries >> 3), inPalette->palData);
     
     palBound = inPalette->palData;
}

void texInit() {
     #if FL_TEXTURE_ERRORTEX
     #if FL_TEXTURE_ERRORTEX_COMPILE
     texError = &texErrorStruct;
     #else
     texError = texLoad(FL_TEXTURE_ERRORTEX_PATH);
     #endif
     #endif
}

Texture* texLoad(char* inPath) {
     if(!inPath || !inPath[0]) {
          #if FL_DEBUG_WARNING
          debugWarning("Invalid texture file path \"%s\".", inPath);
          #endif
          return NULL;
     }
     
     inPath = filePathValidate(inPath);
     
     Texture* tempOut = texTreeFindTexByPath(inPath);
     if(tempOut) {
          tempOut->texUseCount++;
          return tempOut;
     }
     
     char* tempExt = fileExtension(inPath);
     if(!tempExt || !tempExt[0]) {
          #if FL_DEBUG_WARNING
          debugWarning("Cannot determine file extension for texture \"%s\".", inPath);
          #endif
          return NULL;
     }
     strLCase((u8*)tempExt);
     
     if(!strcmp(tempExt, "pcx") || !strcmp(tempExt, "pcc")){
          #if FL_TEXTURE_PCX
          tempOut = texLoadPCX(inPath);
          #else
          #if FL_DEBUG_ERROR
          debugError("PCX textures not supported in this build.");
          #endif
          #endif
     } else if(!strcmp(tempExt, "tga") || !strcmp(tempExt, "tpic")) {
          #if FL_TEXTURE_TGA
          tempOut = texLoadTGA(inPath);
          #else
          #if FL_DEBUG_ERROR
          debugError("TGA textures not supported in this build.");
          #endif
          #endif
     } else if(!strcmp(tempExt, "bmp") || !strcmp(tempExt, "dib")){
          #if FL_TEXTURE_BMP
          tempOut = texLoadBMP(inPath);
          #else
          #if FL_DEBUG_ERROR
          debugError("BMP textures not supported in this build.");
          #endif
          #endif
     } else if(!strcmp(tempExt, "wal")) {
          #if FL_TEXTURE_WAL
          tempOut = texLoadWAL(inPath);
          #else
          #if FL_DEBUG_ERROR
          debugError("WAL textures not supported in this build.");
          #endif
          #endif
     } else if(!strcmp(tempExt, "ftf")) {
          #if FL_TEXTURE_FTF
          tempOut = texLoadFTF(inPath);
          #else
          #if FL_DEBUG_ERROR
          debugError("FTF textures not supported in this build.");
          #endif
          #endif
     } else {
          #if FL_DEBUG_WARNING
          debugWarning("Texture format \"%s\" not supported.", tempExt);
          #endif
     }
     
     memFree(tempExt);
     
     if(!tempOut)
          return NULL;
     
     #if FL_TEXTURE_AUTOMIPMAP
     #if !FL_TEXTURE_AUTOMIPMAP_PALETTED
     if(!texPalettized(tempOut)) {
     #endif
     texMipMap(tempOut, FL_TEXTURE_AUTOMIPMAP);
     #if !FL_TEXTURE_AUTOMIPMAP_PALETTED
     }
     #endif     
     #endif
     
     #if FL_TEXTURE_AUTOSWIZZLE
     texSwizzle(tempOut);
     #endif
     
     #if (!FL_TEXTURE_AUTOMIPMAP && !FL_TEXTURE_AUTOSWIZZLE)
     if(memIsCachedPointer(tempOut->texData)) {
          if(texDataSize(tempOut) >= 16384)
               sceKernelDcacheWritebackInvalidateAll();
          else
               sceKernelDcacheWritebackInvalidateRange(tempOut->texData, texDataSize(tempOut));
     }
     #endif

     texTreeAdd(inPath, tempOut);

     return tempOut;
}

bool texSave(Texture* inTex, char* inPath) {
     if(!inTex) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to save a NULL texture.");
          #endif
          return false;
     }
     if(!inPath || !inPath[0]) {
          #if FL_DEBUG_WARNING
          debugWarning("Invalid texture save file path \"%s\".", inPath);
          #endif
          return false;
     }
     
     inPath = filePathValidate(inPath);
     
     char* tempExt = fileExtension(inPath);
     if(!tempExt || !tempExt[0]) {
          #if FL_DEBUG_WARNING
          debugWarning("Cannot determine file extension for texture \"%s\".", inPath);
          #endif
          return false;
     }
     strLCase((u8*)tempExt);

     if(!strcmp(tempExt, "pcx") || !strcmp(tempExt, "pcc")){
          #if FL_TEXTURE_PCX
          return texSavePCX(inTex, inPath);
          #else
          #if FL_DEBUG_ERROR
          debugError("PCX textures not supported in this build.");
          #endif
          return false;
          #endif
     } else if(!strcmp(tempExt, "tga") || !strcmp(tempExt, "tpic")) {
          #if FL_TEXTURE_TGA
          return texSaveTGA(inTex, inPath);
          #else
          #if FL_DEBUG_ERROR
          debugError("TGA textures not supported in this build.");
          #endif
          return false;
          #endif
     } else if(!strcmp(tempExt, "bmp") || !strcmp(tempExt, "dib")){
          #if FL_TEXTURE_BMP
          return texSaveBMP(inTex, inPath);
          #else
          #if FL_DEBUG_ERROR
          debugError("BMP textures not supported in this build.");
          #endif
          return false;
          #endif
     } else if(!strcmp(tempExt, "wal")) {
          #if FL_TEXTURE_WAL
          return texSaveWAL(inTex, inPath);
          #else
          #if FL_DEBUG_ERROR
          debugError("WAL textures not supported in this build.");
          #endif
          return false;
          #endif
     } else if(!strcmp(tempExt, "ftf")) {
          #if FL_TEXTURE_FTF
          return texSaveFTF(inTex, inPath);
          #else
          #if FL_DEBUG_ERROR
          debugError("FTF textures not supported in this build.");
          #endif
          return false;
          #endif
     } else if(!strcmp(tempExt, "raw")) {
          #if FL_TEXTURE_FTF
          return texSaveRAW(inTex, inPath);
          #else
          #if FL_DEBUG_ERROR
          debugError("RAW textures not supported in this build.");
          #endif
          return false;
          #endif
     }
     
     #if FL_DEBUG_WARNING
     debugWarning("Texture format \"%s\"not supported.", tempExt);
     #endif
     
     memFree(tempExt);
     
     return false;
}

char* texPath(Texture* inTex) {
     return texTreeFindPathByTex(inTex);
}

Texture* texCreate(u32 inWidth, u32 inHeight, u8 inPixelFormat) {
     Texture* tempOut = (Texture*)memAlloc(sizeof(Texture));
     if(!tempOut) {
          #if FL_DEBUG_ERROR
          debugError("Couldn't create texture struct.\nOut of memory.");
          #endif
          return NULL;
     }
     tempOut->texWidth = inWidth;
     tempOut->texHeight = inHeight;
     tempOut->texPixelFormat = inPixelFormat;
     tempOut->texDataWidth = getNextPower2(inWidth);
     tempOut->texDataHeight = getNextPower2(inHeight);
     tempOut->texStride = tempOut->texDataWidth;
     tempOut->texUseCount = 1;
     tempOut->texSwizzled = false;
     tempOut->texMipMaps = 0;
     #if FL_TEXTURE_PRESERVENONALPHA
     tempOut->texAlpha = true;
     #endif

     #if FL_TEXTURE_UNCACHED
     tempOut->texData = (char*)memAllocUncached(texDataSize(tempOut));
     #else
     tempOut->texData = (char*)memAlign(16, texDataSize(tempOut));
     #endif

     if(!tempOut->texData) {
          #if FL_DEBUG_ERROR
          debugError("Couldn't create texture(%ux%u:%i).\nOut of memory.", (unsigned int)tempOut->texDataWidth, (unsigned int)tempOut->texDataHeight, texBPP(tempOut));
          #endif
          memFree(tempOut);
          return NULL;
     }
     #if (FL_TEXTURE_UNCACHED && !FL_MEMORY)
     tempOut->texData = (char*)((unsigned int)tempOut->texData | 0x40000000);
     sceKernelDcacheWritebackAll();
     #endif
     tempOut->texPalette = NULL;
     memClear(tempOut->texMipData, 8);
     
     if(tempOut->texDataWidth != inWidth) {
          u32 j;
          for(j = 0; j < inHeight; j++)
               memClear(&tempOut->texData[(((j * tempOut->texDataWidth) + inWidth) * texBPP(tempOut)) >> 3], ((tempOut->texDataWidth - inWidth) * texBPP(tempOut)) >> 3);
     }
     if(tempOut->texDataHeight != inHeight)
          memClear(&tempOut->texData[((inHeight * tempOut->texDataWidth) * texBPP(tempOut)) >> 3], ((tempOut->texDataWidth * (tempOut->texDataHeight - inHeight) * texBPP(tempOut)) >> 3));
     
     return tempOut;
}

Texture* texCreateBlank(u32 inWidth, u32 inHeight, u8 inPixelFormat) {
     Texture* tempOut = texCreate(inWidth, inHeight, inPixelFormat);
     if(!tempOut)
          return NULL;
     
     memClear(tempOut->texData, ((tempOut->texDataHeight * tempOut->texDataWidth * texBPP(tempOut)) >> 3));
     
     return tempOut;
}

#if FL_GRAPHICS
bool texSwizzle(Texture* inTex) {
     if(inTex->texSwizzled) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to swizzle a swizzled texture.");
          #endif
          return false;
     }
 
     if(!graphicsSwizzleData((u8*)inTex->texData, inTex->texDataWidth, inTex->texHeight, inTex->texPixelFormat)) {
          #if FL_DEBUG_WARNING
          debugWarning("Error swizzling texture.");
          #endif
          return false;
     }
     
     if(inTex->texMipMaps) {
          u8 i;
          for(i = 0; i < inTex->texMipMaps; i++) {
               if(!inTex->texMipData[i] || !(inTex->texDataWidth >> (i + 1)) || (((inTex->texDataWidth * texBPP(inTex)) >> (i + 4)) & 15) || ((inTex->texDataHeight >> (i + 1)) & 7))
                    break;
               if(!graphicsSwizzleData((u8*)inTex->texMipData[i], (inTex->texDataWidth >> (i + 1)), (inTex->texDataHeight >> (i + 1)), inTex->texPixelFormat)) {
                    #if FL_DEBUG_WARNING
                    debugWarning("Error swizzling texture mipmap %i.", i);
                    #endif
                    break;
               }
          }
     }
     
     inTex->texSwizzled = true;
     return true;
}

bool texUnswizzle(Texture* inTex) {
     if(!inTex->texSwizzled) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to unswizzle an unswizzled texture.");
          #endif
          return false;
     }

     if(!graphicsUnswizzleData((u8*)inTex->texData, inTex->texDataWidth, inTex->texHeight, inTex->texPixelFormat)) {
          #if FL_DEBUG_WARNING
          debugWarning("Error unswizzling texture.");
          #endif
          return false;
     }
     
     if(inTex->texMipMaps) {
          u8 i;
          for(i = 1; i <= inTex->texMipMaps; i++) {
               if(!inTex->texMipData[i] || !(inTex->texDataWidth >> (i + 1)) || (((inTex->texDataWidth * texBPP(inTex)) >> (i + 4)) & 15) || ((inTex->texDataHeight >> (i + 1)) & 7))
                    break;
               if(!graphicsUnswizzleData((u8*)inTex->texMipData[i - 1], (inTex->texDataWidth >> i), (inTex->texDataHeight >> i), inTex->texPixelFormat)) {
                    #if FL_DEBUG_WARNING
                    debugWarning("Error unswizzling texture mipmap %i.", i);
                    #endif
                    return false;
               }
          }
     }

     inTex->texSwizzled = false;
     return true;
}
#endif

void texBind(Texture* inTex) {
     if((!inTex) || (!inTex->texData)) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to bind a NULL texture or a texture with NULL data.");
          #endif
          #if FL_TEXTURE_ERRORTEX
          inTex = texError;
          #else
          return;
          #endif
     } else if(texPalettized(inTex) && (inTex->texPalette == NULL)) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to bind a palettized texture without a palette.");
          #endif
          #if FL_TEXTURE_ERRORTEX
          inTex = texError;
          #else
          return;
          #endif
     }
     
     if(texPalettized(inTex))
          palBind(inTex->texPalette);
     
     if(texBound && (texBound == inTex->texData))
          return;

     sceGuTexMode(inTex->texPixelFormat, inTex->texMipMaps, 0, inTex->texSwizzled);
     sceGuTexImage(0, inTex->texDataWidth, inTex->texDataHeight, inTex->texStride, (void*)inTex->texData);
     if(inTex->texMipMaps) {
          u8 i;
          for(i = 1; i <= inTex->texMipMaps; i++)
               sceGuTexImage(i, (inTex->texDataWidth >> i), (inTex->texDataHeight >> i), (inTex->texStride >> i), (void*)inTex->texMipData[i - 1]);
     }
     
     sceGuTexWrap(GU_REPEAT, GU_REPEAT);

     texBound = inTex->texData;
}

void texFree(void* inTex) {
     Texture* tempTex = (Texture*)inTex;
     
     tempTex->texUseCount--;
     if(tempTex->texUseCount)
          return;
     
     #if FL_MEMORY
     if(tempTex) {
          if(tempTex->texData == texBound)
               texBound = NULL;
          memFree(tempTex->texData);
          if(tempTex->texMipMaps) {
               u8 i;
               for(i = 1; i <= tempTex->texMipMaps; i++)
                    memFree(tempTex->texMipData[i - 1]);
          }
          if(tempTex->texPalette && !tempTex->texPalette->palGlobal)
               palFree(tempTex->texPalette);
          memFree(tempTex);
     }
     #else
     if(tempTex) {
          if(tempTex->texData) {
               free(tempTex->texData);
               if(tempTex->texData == texBound)
                    texBound = NULL;
          }
          if(tempTex->texMipMaps) {
               u8 i;
               for(i = 1; i <= tempTex->texMipMaps; i++) {
                    if(inTex->texMipData[i - 1])
                         memFree(tempTex->texMipData[i - 1]);
               }
          }
          if(tempTex->texPalette && !tempTex->texPalette->palGlobal)
               palFree(tempTex->texPalette);
          free(tempTex);
     }
     #endif
     
     texTreeDelNodeByTex(tempTex);
}

int texMipMap(Texture* inTex, u8 inLevel) {
     if(!inLevel)
          return 0;
     if(!inTex) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to mipmap NULL texture.");
          #endif
          return -1;
     }
     
     inLevel = min(inLevel, 7);
     while(!(inTex->texDataWidth >> inLevel) || !(inTex->texDataHeight >> inLevel))
          inLevel--;
     
     while(inTex->texMipMaps < inLevel) {
          if(!texMipMapAdd(inTex))
               break;
     }
     return inTex->texMipMaps;
}

bool texMipMapAdd(Texture* inTex) {
     if(!inTex) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to mipmap NULL texture.");
          #endif
          return false;
     }
     if(inTex->texMipMaps >= 7) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to mipmap a fully mipmapped texture.");
          #endif
          return false;
     }
     #if !FL_COLOR
     if(texPalettized(inTex) {
          #if FL_DEBUG_DEVWARNING
          debugDevWarning("Mipmapping of paletted textures requires FL_COLOR.");
          #endif
          return false;
     }
     #endif
     
     bool tempSwizzle = false;
     if(inTex->texSwizzled) {
          #if FL_DEBUG_DEVWARNING
          debugDevWarning("Mip-mapping swizzled textures requires them to be unswizzled, mip-mapped, then swizzled again.");
          #endif
          tempSwizzle = true;
          texUnswizzle(inTex);
     }
     
     
     u8 tempLevel = (inTex->texMipMaps + 1);
     u32 tempWidth = inTex->texDataWidth >> tempLevel;
     u32 tempHeight = inTex->texDataHeight >> tempLevel;
     if(!tempWidth || !tempHeight) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to create mipmap smaller than a pixel.");
          #endif
          return false;
     }
     u32 tempDataSize = ((inTex->texDataWidth * inTex->texDataHeight * texBPP(inTex)) >> (3 + (tempLevel << 1)));
     
     u8* tempMipMap = NULL;
     u8* tempLastMip;
     if(tempLevel == 1)
          tempLastMip = (u8*)inTex->texData;
     else
          tempLastMip = (u8*)inTex->texMipData[tempLevel - 2];

     bool tempVMem = false;
     #if FL_MEMORY_VMEM
     tempVMem = vmemPointer(inTex->texData);
     if(tempVMem)
          tempMipMap = vmemAlloc(tempDataSize);
     tempVMem = (tempMipMap != NULL);
     #endif
     if(!tempVMem)
          tempMipMap = memAlign(16, tempDataSize);
     if(!tempMipMap) {
          #if FL_DEBUG_WARNING
          debugWarning("Couldn't allocate mip-map, probably out of memory.");
          #endif
          return false;
     }
     
     int i, j;
     if(inTex->texPixelFormat == GU_PSM_8888) {
          u32* tempMipMap32 = (u32*)tempMipMap;
          u32* tempLastMip32 = (u32*)tempLastMip;
          u32 tempMerge32[4];
          for(j = 0; j < (inTex->texDataHeight >> tempLevel); j++) {
               for(i = 0; i < tempWidth; i++) {
                    tempMerge32[0] = tempLastMip32[(((j << 1) + 0) * (tempWidth << 1)) + ((i << 1) + 0)];
                    tempMerge32[1] = tempLastMip32[(((j << 1) + 0) * (tempWidth << 1)) + ((i << 1) + 1)];
                    tempMerge32[2] = tempLastMip32[(((j << 1) + 1) * (tempWidth << 1)) + ((i << 1) + 0)];
                    tempMerge32[3] = tempLastMip32[(((j << 1) + 1) * (tempWidth << 1)) + ((i << 1) + 1)];
                    tempMipMap32[(j * tempWidth) + i] = color8888_Merge4(tempMerge32[0], tempMerge32[1], tempMerge32[2], tempMerge32[3]);
               }
          }
     } else if(inTex->texPixelFormat < GU_PSM_8888) {
          u16* tempMipMap16 = (u16*)tempMipMap;
          u16* tempLastMip16 = (u16*)tempLastMip;
          u16 tempMerge16[4];
          for(j = 0; j < (inTex->texDataHeight >> tempLevel); j++) {
               for(i = 0; i < tempWidth; i++) {
                    tempMerge16[0] = tempLastMip16[(((j << 1) + 0) * (tempWidth << 1)) + ((i << 1) + 0)];
                    tempMerge16[1] = tempLastMip16[(((j << 1) + 0) * (tempWidth << 1)) + ((i << 1) + 1)];
                    tempMerge16[2] = tempLastMip16[(((j << 1) + 1) * (tempWidth << 1)) + ((i << 1) + 0)];
                    tempMerge16[3] = tempLastMip16[(((j << 1) + 1) * (tempWidth << 1)) + ((i << 1) + 1)];
                    if(inTex->texPixelFormat == GU_PSM_4444)
                         tempMipMap16[(j * tempWidth) + i] = color4444_Merge4(tempMerge16[0], tempMerge16[1], tempMerge16[2], tempMerge16[3]);
                    else if(inTex->texPixelFormat == GU_PSM_5551)
                         tempMipMap16[(j * tempWidth) + i] = color5551_Merge4(tempMerge16[0], tempMerge16[1], tempMerge16[2], tempMerge16[3]);
                    else if(inTex->texPixelFormat == GU_PSM_5650)
                         tempMipMap16[(j * tempWidth) + i] = color5650_Merge4(tempMerge16[0], tempMerge16[1], tempMerge16[2], tempMerge16[3]);
               }
          }
     #if FL_COLOR
     } else if(inTex->texPixelFormat == GU_PSM_T8) { // TODO - Fix mipmapping of clut
          int k, l;
          tempLastMip = (u8*)inTex->texData;
          if(inTex->texPalette->palPixelFormat == GU_PSM_8888) {
               u32* tempPalEntries32 = (u32*)inTex->texPalette->palData;
               u32 tempMerge32[(1 << tempLevel) * (1 << tempLevel)];
               for(j = 0; j < (inTex->texDataHeight >> tempLevel); j++) {
                    for(i = 0; i < tempWidth; i++) {
                         for(k = 0; k < (1 << tempLevel); k++) {
                              for(l = 0; l < (1 << tempLevel); l++) {
                                   tempMerge32[(k * (1 << tempLevel)) + l] = tempPalEntries32[tempLastMip[(((j << tempLevel) + k) * inTex->texWidth) + ((i << tempLevel) + l)]];
                              }
                         }
                         tempMipMap[(j * tempWidth) + i] = palClosestColor(inTex->texPalette, colorMergeN8888(tempMerge32, ((1 << tempLevel) * (1 << tempLevel))));
                    }
               }
          } else {
               u16* tempPalEntries16 = (u16*)inTex->texPalette->palData;
               u16 tempMerge16[(1 << tempLevel) * (1 << tempLevel)];
               for(j = 0; j < (inTex->texDataHeight >> tempLevel); j++) {
                    for(i = 0; i < tempWidth; i++) {
                         for(k = 0; k < (1 << tempLevel); k++) {
                              for(l = 0; l < (1 << tempLevel); l++) {
                                   tempMerge16[(k * (1 << tempLevel)) + l] = tempPalEntries16[tempLastMip[(((j << tempLevel) + k) * inTex->texWidth) + ((i << tempLevel) + l)]];
                              }
                         }
                         if(inTex->texPalette->palPixelFormat == GU_PSM_4444)
                              tempMipMap[(j * tempWidth) + i] = palClosestColor(inTex->texPalette, colorMergeN4444(tempMerge16, ((1 << tempLevel) * (1 << tempLevel))));
                         else if(inTex->texPalette->palPixelFormat == GU_PSM_5551)
                              tempMipMap[(j * tempWidth) + i] = palClosestColor(inTex->texPalette, colorMergeN5551(tempMerge16, ((1 << tempLevel) * (1 << tempLevel))));
                         else if(inTex->texPalette->palPixelFormat == GU_PSM_5650)
                              tempMipMap[(j * tempWidth) + i] = palClosestColor(inTex->texPalette, colorMergeN5650(tempMerge16, ((1 << tempLevel) * (1 << tempLevel))));
                    }
               }
          }
     } else if(inTex->texPixelFormat == GU_PSM_T4) {
          memClear(tempMipMap, tempDataSize);
          if(inTex->texPalette->palPixelFormat == GU_PSM_8888) {
               u32* tempPalEntries32 = (u32*)inTex->texPalette->palData;
               u32 tempMerge32[4];
               for(j = 0; j < (inTex->texDataHeight >> tempLevel); j++) {
                    for(i = 0; i < tempWidth; i++) {
                         tempMerge32[0] = tempPalEntries32[tempLastMip[(((j << 1) + 0) * tempWidth) + i] & 0x0F];
                         tempMerge32[1] = tempPalEntries32[(tempLastMip[(((j << 1) + 0) * tempWidth) + i] & 0xF0) >> 4];
                         tempMerge32[2] = tempPalEntries32[tempLastMip[(((j << 1) + 1) * tempWidth) + i] & 0x0F];
                         tempMerge32[3] = tempPalEntries32[(tempLastMip[(((j << 1) + 1) * tempWidth) + i] & 0xF0) >> 4];
                         if(i & 1) {
                              tempMipMap[(j * tempWidth) + i] += (palClosestColor(inTex->texPalette, color8888_Merge4(tempMerge32[0], tempMerge32[1], tempMerge32[2], tempMerge32[3])) << 4);
                         } else {
                              tempMipMap[(j * tempWidth) + i] = palClosestColor(inTex->texPalette, color8888_Merge4(tempMerge32[0], tempMerge32[1], tempMerge32[2], tempMerge32[3]));
                         }
                    }
               }
          } else {
               u16* tempPalEntries16 = (u16*)inTex->texPalette->palData;
               u16 tempMerge16[4];
               for(j = 0; j < (inTex->texDataHeight >> tempLevel); j++) {
                    for(i = 0; i < tempWidth; i++) {
                         tempMerge16[0] = tempPalEntries16[tempLastMip[(((j << 1) + 0) * tempWidth) + i] & 0x0F];
                         tempMerge16[1] = tempPalEntries16[(tempLastMip[(((j << 1) + 0) * tempWidth) + i] & 0xF0) >> 4];
                         tempMerge16[2] = tempPalEntries16[tempLastMip[(((j << 1) + 1) * tempWidth) + i] & 0x0F];
                         tempMerge16[3] = tempPalEntries16[(tempLastMip[(((j << 1) + 1) * tempWidth) + i] & 0xF0) >> 4];
                         if(inTex->texPalette->palPixelFormat == GU_PSM_4444)
                              tempMipMap[(j * tempWidth) + i] = palClosestColor(inTex->texPalette, color4444_Merge4(tempMerge16[0], tempMerge16[1], tempMerge16[2], tempMerge16[3]));
                         else if(inTex->texPalette->palPixelFormat == GU_PSM_5551)
                              tempMipMap[(j * tempWidth) + i] = palClosestColor(inTex->texPalette, color5551_Merge4(tempMerge16[0], tempMerge16[1], tempMerge16[2], tempMerge16[3]));
                         else if(inTex->texPalette->palPixelFormat == GU_PSM_5650)
                              tempMipMap[(j * tempWidth) + i] = palClosestColor(inTex->texPalette, color5650_Merge4(tempMerge16[0], tempMerge16[1], tempMerge16[2], tempMerge16[3]));
                    }
               }
          }
     #endif
     } else {
          #if FL_DEBUG_WARNING
          debugWarning("Can't mipmap textures with palettes of over 8 bits, or DXT compressed.");
          #endif
          return false;
     }
     
     inTex->texMipMaps = tempLevel;
     inTex->texMipData[tempLevel - 1] = (char*)tempMipMap;
     
     if(tempSwizzle)
          texSwizzle(inTex);

     if(memIsCachedPtr(tempMipMap)) {
          if(tempDataSize > 16384)
               sceKernelDcacheWritebackAll();
          else
               sceKernelDcacheWritebackRange(tempMipMap, tempDataSize);
     }
     
     return true;
}

Palette* palCreate(u32 inEntries, u8 inFormat, bool inGlobal) {
     if(inFormat > GU_PSM_8888) {
          #if FL_DEBUG_ERROR
          debugError("Trying to create a palette with incorrect format.\nDXT and palettized palettes are not supported.");
          #endif
          return NULL;
     }
     if(inEntries > 256) {
          #if FL_DEBUG_ERROR
          debugError("Trying to create a palette with %u entries (Max: 256).", (unsigned int)inEntries);
          #endif
          return NULL;
     }
     #if FL_DEBUG_DEVWARNING
     if(!inEntries)
          debugDevWarning("Creating palette with 0 entries.");
     #endif
     Palette* tempOut = (Palette*)memAlloc(sizeof(Palette));
     if(!tempOut) {
          #if FL_DEBUG_ERROR
          debugError("Couldn't create palette structure.\nOut of memory.");
          #endif
          return NULL;
     }
     tempOut->palEntries = getPaletteSize(inEntries);
     tempOut->palPixelFormat = inFormat;
     tempOut->palGlobal = inGlobal;
     #if FL_MEMORY
     #if FL_TEXTURE_UNCACHED
     tempOut->palData = (char*)memAllocUncached((tempOut->palEntries * palBPP(tempOut)) >> 3);
     #else
     tempOut->palData = (char*)memAlign(16, ((tempOut->palEntries * palBPP(tempOut)) >> 3));
     #endif
     #else
     tempOut->palData = (char*)memalign(16, ((tempOut->palEntries * palBPP(tempOut)) >> 3));
     #endif
     if(!tempOut->palData) {
          #if FL_DEBUG_ERROR
          debugError("Couldn't create palette data.\nOut of memory");
          #endif
          memFree(tempOut);
          return NULL;
     }
     #if (FL_TEXTURE_UNCACHED && !FL_MEMORY)
     tempOut->palData = (char*)((unsigned int)tempOut->palData | 0x40000000);
     sceKernelDcacheWritebackAll();
     #endif
     if(tempOut->palEntries > inEntries)
          memClear(&tempOut->palData[(inEntries * palBPP(tempOut)) >> 3], (((tempOut->palEntries - inEntries) * palBPP(tempOut)) >> 3));
     #if !FL_TEXTURE_UNCACHED
     sceKernelDcacheWritebackAll();
     #endif
     return tempOut;
}

void palFree(Palette* inPalette) {
     #if FL_MEMORY
     memFree(inPalette->palData);
     memFree(inPalette);
     #else
     if(!inPalette)
          return;
     if(inPalette->palData)
          free(inPalette->palData);
     free(inPalette);
     #endif
     if(inPalette == palBound)
          palBound = NULL;
}

s32 palClosestColor(Palette* inPal, u32 inColor) {
     if(!inPal) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to find closest entry in NULL palette.");
          #endif
          return -1;
     }
     if(inPal->palPixelFormat > GU_PSM_8888) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to find closest entry in invalid palette.");
          #endif
          return -2;
     }
     if(!inPal->palEntries) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to find closest entry in palette with 0 entries.");
          #endif
          return -3;
     }
     u8 tempByPP = 4;
     if(inPal->palPixelFormat != GU_PSM_8888) {
          inColor &= 0x0000FFFF;
          tempByPP = 2;
     }
     int i, tempCmp;
     int tempClosest = 0;
     int tempDiff = (1 << 30);
     u32* tempColor32 = (u32*)inPal->palData;
     u16* tempColor16 = (u16*)inPal->palData;
     if(inPal->palPixelFormat == GU_PSM_8888) {
          for(i = 0; i < inPal->palEntries; i++) {
               if(tempColor32[i] == inColor) {
                    return i;
               } else {
                    tempCmp = color8888_Cmp(inColor, tempColor32[i]);
                    if(tempCmp < tempDiff) {
                         tempClosest = i;
                         tempDiff = tempCmp;
                    }
               }
          }
     } else if(inPal->palPixelFormat == GU_PSM_4444) {
          for(i = 0; i < inPal->palEntries; i++) {
               if(tempColor16[i] == inColor) {
                    return i;
               } else {
                    tempCmp = color4444_Cmp(inColor, tempColor16[i]);
                    if(tempCmp < tempDiff) {
                         tempClosest = i;
                         tempDiff = tempCmp;
                    }
               }
          }
     } else if(inPal->palPixelFormat == GU_PSM_5551) {
          for(i = 0; i < inPal->palEntries; i++) {
               if(tempColor16[i] == inColor) {
                    return i;
               } else {
                    tempCmp = color5551_Cmp(inColor, tempColor16[i]);
                    if(tempCmp < tempDiff) {
                         tempClosest = i;
                         tempDiff = tempCmp;
                    }
               }
          }
     } else if(inPal->palPixelFormat == GU_PSM_5650) {
          for(i = 0; i < inPal->palEntries; i++) {
               if(tempColor16[i] == inColor) {
                    return i;
               } else {
                    tempCmp = color5650_Cmp(inColor, tempColor16[i]);
                    if(tempCmp < tempDiff) {
                         tempClosest = i;
                         tempDiff = tempCmp;
                    }
               }
          }
     }
     return tempClosest;
}

Palette* palCreateGrayscaleT8() {
     Palette* tempOut = palCreate(256, GU_PSM_8888, false);
     u8* tempData = (u8*)tempOut->palData;
     int i;
     for(i = 0; i < 256; i++) {
          tempData[i << 2] = i;
          tempData[(i << 2) + 1] = i;
          tempData[(i << 2) + 2] = i;
          tempData[(i << 2) + 3] = 0xFF;
     }
     #if !FL_TEXTURE_UNCACHED
     sceKernelDcacheWritebackInvalidateAll();
     #endif
     return tempOut;
}

Palette* palCreateGrayscaleT4() {
     Palette* tempOut = palCreate(16, GU_PSM_8888, false);
     u8* tempData = (u8*)tempOut->palData;
     int i;
     for(i = 0; i < 16; i++) {
          tempData[i << 2] = ((i * 0xFF) / 15);
          tempData[(i << 2) + 1] = ((i * 0xFF) / 15);
          tempData[(i << 2) + 2] = ((i * 0xFF) / 15);
          tempData[(i << 2) + 3] = 0xFF;
     }
     #if !FL_TEXTURE_UNCACHED
     sceKernelDcacheWritebackInvalidateAll();
     #endif
     return tempOut;
}

Palette* palCreateMonoT4(u32 inForeColor, u32 inBackColor) {
     Palette* tempOut = palCreate(16, GU_PSM_8888, false);
     u8* tempData = (u8*)tempOut->palData;
     int i;
     for(i = 0; i < 16; i++) {
          tempData[i << 2] = (((i * (inForeColor & 0x000000FF)) + ((15 - i) * (inBackColor & 0x000000FF))) / 15);
          tempData[(i << 2) + 1] = (((i * ((inForeColor & 0x0000FF00) >> 8)) + ((15 - i) * ((inBackColor & 0x0000FF00) >> 8))) / 15);
          tempData[(i << 2) + 2] = (((i * ((inForeColor & 0x00FF0000) >> 16)) + ((15 - i) * ((inBackColor & 0x00FF0000) >> 16))) / 15);
          tempData[(i << 2) + 3] = (((i * ((inForeColor & 0xFF000000) >> 24)) + ((15 - i) * ((inBackColor & 0xFF000000) >> 24))) / 15);
     }
     #if !FL_TEXTURE_UNCACHED
     sceKernelDcacheWritebackInvalidateAll();
     #endif
     return tempOut;
}

Palette* palCreateDefaultColorT4() {
     Palette* tempOut = palCreate(16, GU_PSM_8888, false);
     u32* tempPalData = (u32*)tempOut->palData;
     tempPalData[0]  = 0xFF000000; // Black
     tempPalData[1]  = 0xFF7F7F7F; // Med. Gray
     tempPalData[2]  = 0xFFCFCFCF; // Lig. Gray
     tempPalData[3]  = 0xFFFFFFFF; // White
     tempPalData[4]  = 0xFF00007F; // Dark Red
     tempPalData[5]  = 0xFF0000FF; // Red
     tempPalData[6]  = 0xFF007F7F; // Dark Yellow
     tempPalData[7]  = 0xFF00FFFF; // Yellow
     tempPalData[8]  = 0xFF007F00; // Dark Green
     tempPalData[9]  = 0xFF00FF00; // Green
     tempPalData[10] = 0xFF7F7F00; // Dark Turqoise
     tempPalData[11] = 0xFFFFFF00; // Turqoise
     tempPalData[12] = 0xFF7F0000; // Dark Blue
     tempPalData[13] = 0xFFFF0000; // Blue
     tempPalData[14] = 0xFF7F007F; // Dark Purple
     tempPalData[15] = 0xFFFF00FF; // Purple
     #if !FL_TEXTURE_UNCACHED
     sceKernelDcacheWritebackInvalidateAll();
     #endif
     return tempOut;
}

Palette* palGrayscaleT8() {
     if(!palGrayscaleT8Ptr) {
          palGrayscaleT8Ptr = palCreateGrayscaleT8();
          palGrayscaleT8Ptr->palGlobal = true;
     }
     return palGrayscaleT8Ptr;
}

Palette* palGrayscaleT4() {
     if(!palGrayscaleT4Ptr) {
          palGrayscaleT4Ptr = palCreateGrayscaleT4();
          palGrayscaleT4Ptr->palGlobal = true;
     }
     return palGrayscaleT4Ptr;
}

Palette* palDefaultColorT4() {
     if(!palDefaultColorT4Ptr) {
          palDefaultColorT4Ptr = palCreateDefaultColorT4();
          palDefaultColorT4Ptr->palGlobal = true;
     }
     return palDefaultColorT4Ptr;
}

#if FL_GRAPHICS
Texture* texScreenshot() {
     Texture* tempOut = texCreate(SCREEN_WIDTH, SCREEN_HEIGHT, GU_PSM_8888);
     if(!tempOut) {
          #if FL_DEBUG_ERROR
          debugError("Error creating screenshot.\nMost likely out of memory.");
          #endif
          return NULL;
     }
     u32 j;
     #if FL_MEMORY
     char* tempDispBuffer = (char*)vmemAbsolutePtr(graphicsDispBuffer);
     #else
     char* tempDispBuffer = (char*)((unsigned int)graphicsDispBuffer | 0x04000000);
     #endif
     for(j = 0; j < SCREEN_HEIGHT; j++)
          memCopy(&tempOut->texData[(j * tempOut->texDataWidth) << 2], &tempDispBuffer[(j * SCREEN_STRIDE) << 2], (SCREEN_WIDTH << 2));
     #if !FL_TEXTURE_UNCACHED
     sceKernelDcacheWritebackInvalidateAll();
     #endif
     #if FL_TEXTURE_PRESERVENONALPHA
     tempOut->texAlpha = false;
     #endif
     return tempOut;
}
#endif

#if FL_MEMORY_VMEM
bool texMoveVMem(Texture* inTex) {
     int tempSize = texDataSize(inTex);
     void* tempVPtr = vmemAlloc(tempSize);
     if(!tempVPtr) {
          #if DEBUG_WARNING
          debugWarning("Couldn't move texture to vram\nProbably out of video memory.");
          #endif
          return false;
     }
     #if FL_GRAPHICS
     if(!graphicsFrame)
          sceGuStart(GU_DIRECT, graphicsDisplayList);
     sceGuCopyImage(inTex->texPixelFormat, 0, 0, inTex->texDataWidth, inTex->texDataHeight, inTex->texDataWidth, inTex->texData, 0, 0, inTex->texDataWidth, tempVPtr);
     #else
     memCopy(tempVPtr, inTex->texData, tempSize);
     #endif
     memFree(inTex->texData);
     inTex->texData = tempVPtr;
     
     //if(texPalettized(inTex))
     //     palMoveVMem(inTex->texPalette);
     
     u8 i;
     if(inTex->texMipMaps) {
          for(i = 1; i <= inTex->texMipMaps; i++) {
               tempSize >>= 2;
               tempVPtr = vmemAlloc(tempSize);
               if(!tempVPtr) {
                    #if FL_DEBUG_WARNING
                    debugWarning("Couldn't copy all mip-maps to vram.");
                    #endif
                    return true;
               }
               #if FL_GRAPHICS
               sceGuCopyImage(inTex->texPixelFormat, 0, 0, (inTex->texDataWidth >> i), (inTex->texDataHeight >> i), (inTex->texDataWidth >> i), inTex->texMipData[i - 1], 0, 0, (inTex->texDataWidth >> i), tempVPtr);
               #else
               memCopy(tempVPtr, inTex->texMipData[i - 1], tempSize);
               #endif
               memFree(inTex->texMipData[i - 1]);
               inTex->texMipData[i - 1] = tempVPtr;
          }
     }
     
     #if FL_GRAPHICS
     if(!graphicsFrame)
          sceGuFinish();
     #endif
     
     return true;
}

bool palMoveVMem(Palette* inPal) {
     if(!inPal) {
          #if DEBUG_WARNING
          debugWarning("Can't move NULL palette to vmem.");
          #endif                
          return false;
     }
     
     void* tempVPtr = vmemAlloc((inPal->palEntries * palBPP(inPal)) >> 3);
     if(!tempVPtr) {
          #if DEBUG_WARNING
          debugWarning("Couldn't move palette to vram\nProbably out of video memory.");
          #endif
          return false;
     }
     #if FL_GRAPHICS
     if(!graphicsFrame)
          sceGuStart(GU_DIRECT, graphicsDisplayList);
     sceGuCopyImage(inPal->palPixelFormat, 0, 0, 8, (inPal->palEntries >> 3), 8, inPal->palData, 0, 0, 8, tempVPtr);
     if(!graphicsFrame)
          sceGuFinish();
     #else
     memCopy(tempVPtr, inPal->palData, ((inPal->palEntries * palBPP(inPal)) >> 3));
     #endif
     
     memFree(inPal->palData);
     
     inPal->palData = tempVPtr;
     return true;
}
#endif

u32 texPixel(Texture* inTex, int inX, int inY) {
     while(inX < 0)
          inX += inTex->texWidth;
     while(inX > inTex->texWidth)
          inX -= inTex->texWidth;
     while(inY < 0)
          inY += inTex->texHeight;
     while(inY > inTex->texHeight)
          inY -= inTex->texHeight;
     u32 tempOut = 0;
     if(texBPP(inTex) >= 8)
         memCopy(&tempOut, &inTex->texData[(((inY * inTex->texDataWidth) + inX) * texBPP(inTex)) >> 3], (texBPP(inTex) >> 3));
     else
         tempOut= ((inTex->texData[(((inY * inTex->texDataWidth) + inX) * texBPP(inTex)) >> 3] & (0x0F << (inX & 1))) >> (inX & 1));
     return tempOut;
}

/*void texAddMagMap(Texture* inTex) {
     if(!inTex || !inTex->texData) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to magnify a NULL texture or a texture with NULL data.");
          #endif
          return;
     }
     if(((inTex->texDataWidth << 1) > 512) || ((inTex->texDataHeight << 1) > 512)) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to magnify a texture of dimensions (%ux%u:%u), this would overstep the 512x512 boundry.", (unsigned int)inTex->texWidth, (unsigned int)inTex->texHeight, (unsigned int)texBPP(inTex));
          #endif
          return;
     }
     
     u32 tempDataSize = (texDataSize(inTex) << 2);
     u8* tempData = memAlloc(tempDataSize);
     
     // TODO
}*/






// Texture tree functions.

bool texTreeAdd(char* inPath, Texture* inTex) {
     if(!inPath || !inTex || !inPath[0])
          return false;
     texTreeNode* tempNodeAdd = (texTreeNode*)memAlloc(sizeof(texTreeNode));
     if(!tempNodeAdd) {
          #if FL_DEBUG_WARNING
          debugWarning("Unable to add texture to tree.\nProbably out of memory.");
          #endif
          return false;
     }
     
     strcpy(tempNodeAdd->texNodePath, inPath);
     tempNodeAdd->texNodeData = inTex;
     tempNodeAdd->texNodeNext[0] = NULL;
     tempNodeAdd->texNodeNext[1] = NULL;
     
     if(!texTree) {
          texTree = tempNodeAdd;
          return true;
     }
     
     texTreeNode* tempNode = texTree;
     int tempCmp = 0;
     while(flRunning) {
          tempCmp = strcmp(inPath, tempNode->texNodePath);
          if(!tempCmp) {
               #if FL_DEBUG_WARNING
               debugWarning("Trying to add texture to tree, but texture is already in the tree.");
               #endif
               return false;
          }
          tempCmp = (tempCmp > 0 ? 1 : 0);
          if(!tempNode->texNodeNext[tempCmp]) {
               tempNode->texNodeNext[tempCmp] = tempNodeAdd;
               return true;
          }
          tempNode = tempNode->texNodeNext[tempCmp];
     }

     return false;
}

Texture* texTreeFindTexByPath(char* inPath) {
     if(!inPath || !inPath[0] || !texTree)
          return NULL;
     texTreeNode* tempNode = texTree;
     int tempCmp = 0;
     while(flRunning) {
          tempCmp = strcmp(inPath, tempNode->texNodePath);
          if(!tempCmp)
               return tempNode->texNodeData;
          tempCmp = (tempCmp > 0 ? 1 : 0);
          if(!tempNode->texNodeNext[tempCmp])
               return NULL;
          tempNode = tempNode->texNodeNext[tempCmp];
     }
     return NULL;
}

texTreeNode* texTreeFindNodeByTexR(Texture* inTex, texTreeNode* inNode) {
     if(!inNode)
          return NULL;
     if(inTex == inNode->texNodeData)
          return inNode;
     texTreeNode* tempOut = NULL;
     if(inNode->texNodeNext[0]) {
          tempOut = texTreeFindNodeByTexR(inTex, inNode->texNodeNext[0]);
          if(tempOut)
               return tempOut;
     }
     if(inNode->texNodeNext[1])
          tempOut = texTreeFindNodeByTexR(inTex, inNode->texNodeNext[1]);
     return tempOut;
}

char* texTreeFindPathByTex(Texture* inTex) {
     if(!texTree || !inTex)
          return NULL;
     return texTreeFindNodeByTexR(inTex, texTree)->texNodePath;
}

texTreeNode* texTreeFindNodeByTex(Texture* inTex) {
     if(!texTree || !inTex)
          return NULL;
     return texTreeFindNodeByTexR(inTex, texTree);
}

void texTreeAddR(texTreeNode* inNode) {
     texTreeAdd(inNode->texNodePath, inNode->texNodeData);
     if(inNode->texNodeNext[0])
          texTreeAddR(inNode->texNodeNext[0]);
     if(inNode->texNodeNext[1])
          texTreeAddR(inNode->texNodeNext[1]);
}

void texTreeDelR(texTreeNode* inNode) {
     if(inNode->texNodeNext[0])
          texTreeDelR(inNode->texNodeNext[0]);
     if(inNode->texNodeNext[1])
          texTreeDelR(inNode->texNodeNext[1]);
     free(inNode);
}

bool texTreeDelNodeByTex(Texture* inTex) {
     if(!texTree || !inTex)
          return false;
     texTreeNode* tempNodeDel = texTreeFindNodeByTex(inTex);
     if(!tempNodeDel) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to delete non-existant texture node from tree.");
          #endif
          return false;
     }
     if(tempNodeDel->texNodeNext[0])
          texTreeAddR(tempNodeDel->texNodeNext[0]);
     if(tempNodeDel->texNodeNext[1])
          texTreeAddR(tempNodeDel->texNodeNext[1]);
     texTreeDelR(tempNodeDel);
     
     return true;
}

#endif
