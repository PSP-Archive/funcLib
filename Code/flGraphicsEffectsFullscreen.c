#include "flGlobal.h"
#if FL_GRAPHICS_EFFECTS_FULLSCREEN != 0
#include <stdlib.h>
#include <stdio.h>
#include <pspgu.h>
#include <pspkernel.h>

#if FL_INCLUDE_ALL_C == 0
#include "flGraphicsEffectsFullscreen.h"
#include "flGraphics.h"
#include "flMemory.h"
#include "flTexture.h"
#endif

#define blockWidth  64
#define blockHeight 64
#define blockWidthSafe ((i + blockWidth) > SCREEN_WIDTH ? (SCREEN_WIDTH - i) : blockWidth)
#define blockHeightSafe ((j + blockHeight) > SCREEN_HEIGHT ? (SCREEN_HEIGHT - j) : blockHeight)

void graphicsEffectFsGrayscale() {
     #if FL_MEMORY_VMEM_CACHE < 16384
     #if FL_MEMORY_VMEM != 0
     char* tempBuffer = vmemAlloc(16384);
     if(!tempBuffer) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate the 16k of vram required to perform this effect in hardware.\nReverting to slower algorithm.");
          #endif
          graphicsEffectFsGrayscaleSW();
          return;        
     }
     #else
     char* tempBuffer = (void*)0x44015400;
     return;
     #endif
     #else
     char* tempBuffer = (char*)vmemCache;
     #endif
     
     u8* tempPalette = memQalloc(1024);
     if(!tempPalette) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate aligned 1024 byte palette.\nReverting to slower algorithm");
          #endif
          #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
          vmemFree(tempBuffer);
          #endif
          graphicsEffectFsGrayscaleSW();
          return;
     }
     u8* tempPtr = tempPalette;
     int i;
     for(i = 0; i < 256; i++) {
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = 0xFF;
     }

     sceKernelDcacheWritebackInvalidateAll();     
     sceGuClutLoad(32, (void*)tempPalette);

     #if FL_VMEM != 0
     void* tempDrawBuffer = memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
     #else
     void* tempDrawBuffer = (void*)((unsigned int)graphicsDrawBuffer | 0x44000000);
     #endif

     sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
     sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     sceGuTexMode(GU_PSM_T32, 0, 0, GU_FALSE);
     
     sceGuDrawBufferList(GU_PSM_8888, memCachedPtr(vmemRelativePtr(tempBuffer)), blockWidth);
     
     sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00FFFFFF, 0x00FFFFFF);
     
     int j;
     vertTsVs* tempVerts = NULL;
     for(j = 0; j < SCREEN_HEIGHT; j += blockHeight) {
          for(i = 0; i < SCREEN_WIDTH; i += blockWidth) {
               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)((unsigned int)tempDrawBuffer + (((j * SCREEN_PITCH) + i) << 2)));

               sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDisable(GU_BLEND);
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               sceGuEnable(GU_BLEND);
               
               sceGuClutMode(GU_PSM_8888, 8, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuClutMode(GU_PSM_8888, 16, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

               sceGuCopyImage(GU_PSM_8888, 0, 0, blockWidthSafe, blockHeightSafe, blockWidth, (void*)tempBuffer, i, j, SCREEN_PITCH, (void*)tempDrawBuffer);
          }
     }

     sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);     
     sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsDrawBuffer, SCREEN_STRIDE);
     //sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsRenderTarget, graphicsRenderTargetStride);

     memFree(tempPalette);
     
     #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
     vmemFree(tempBuffer);
     #endif
     
     #if FL_TEXTURE
     texBound = NULL;
     palBound = NULL;
     #endif
}

void graphicsEffectFsGrayscaleSW() {
     u32 *tempBuffer = (u32*)memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
     
     int i, j;
     int tempLum = 0;
     for(j = 0; j < SCREEN_HEIGHT; j++) {
          for(i = 0; i < SCREEN_WIDTH; i++, tempBuffer++) {
               tempLum = max(max(color8888_Red(*tempBuffer), color8888_Green(*tempBuffer)), color8888_Blue(*tempBuffer));
               tempLum += min(min(color8888_Red(*tempBuffer), color8888_Green(*tempBuffer)), color8888_Blue(*tempBuffer));
               tempLum >>= 1;
               *tempBuffer = color8888_RGBA(tempLum, tempLum, tempLum, color8888_Alpha(*tempBuffer));
          }
          tempBuffer += (SCREEN_PITCH - SCREEN_WIDTH);
     }
}

void graphicsEffectFsSepia() {
     graphicsEffectFsGrayscale();

     sceGuDisable(GU_TEXTURE_2D);

     vertCVs* tempVerts = (vertCVs*)sceGuGetMemory(sizeof(vertCVs) << 1);

     tempVerts[0].vertColor = 0x3F00FFFF;
     tempVerts[0].vertX = 0;
     tempVerts[0].vertY = 0;
     tempVerts[0].vertZ = 0;

     tempVerts[1].vertColor = 0x3F00FFFF;
     tempVerts[1].vertX = SCREEN_WIDTH;
     tempVerts[1].vertY = SCREEN_HEIGHT;
     tempVerts[1].vertZ = 0;

     sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

     sceGuEnable(GU_TEXTURE_2D);    
}

void graphicsEffectFsNightVision() {
     #if FL_MEMORY_VMEM_CACHE < 16384
     #if FL_MEMORY_VMEM != 0
     char* tempBuffer = vmemAlloc(16384);
     if(!tempBuffer) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate the 16k of vram required to perform this effect in hardware.\nReverting to slower algorithm.");
          #endif
          graphicsEffectFsNightVisionSW();
          return;        
     }
     #else
     char* tempBuffer = (void*)0x44015400;
     return;
     #endif
     #else
     char* tempBuffer = (char*)vmemCache;
     #endif
     
     u8* tempPalette = memQalloc(1024);
     if(!tempPalette) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate aligned 1024 byte palette.\nReverting to slower algorithm");
          #endif
          #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
          vmemFree(tempBuffer);
          #endif
          graphicsEffectFsNightVisionSW();
          return;
     }
     u8* tempPtr = tempPalette;
     int i;
     for(i = 0; i < 256; i++) {
          *(tempPtr++) = 0;
          *(tempPtr++) = (i >> 1);
          *(tempPtr++) = 0;
          *(tempPtr++) = 0xFF;
     }

     sceKernelDcacheWritebackInvalidateAll();
     sceGuClutLoad(32, (void*)tempPalette);

     #if FL_VMEM != 0
     void* tempDrawBuffer = memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
     #else
     void* tempDrawBuffer = (void*)((unsigned int)graphicsDrawBuffer | 0x44000000);
     #endif

     sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
     sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     sceGuTexMode(GU_PSM_T32, 0, 0, GU_FALSE);
     
     sceGuDrawBufferList(GU_PSM_8888, memCachedPtr(vmemRelativePtr(tempBuffer)), blockWidth);
     
     sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00FFFFFF, 0x00FFFFFF);
     
     int j;
     vertTsVs* tempVerts = NULL;
     for(j = 0; j < SCREEN_HEIGHT; j += blockHeight) {
          for(i = 0; i < SCREEN_PITCH; i += blockWidth) {
               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)((unsigned int)tempDrawBuffer + (((j * SCREEN_PITCH) + i) << 2)));

               sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDisable(GU_BLEND);
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               sceGuEnable(GU_BLEND);
               
               sceGuClutMode(GU_PSM_8888, 8, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuClutMode(GU_PSM_8888, 16, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

               sceGuCopyImage(GU_PSM_8888, 0, 0, blockWidthSafe, blockHeightSafe, blockWidth, (void*)tempBuffer, i, j, SCREEN_PITCH, (void*)tempDrawBuffer);
          }
     }

     sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);     
     sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsDrawBuffer, SCREEN_STRIDE);
     //sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsRenderTarget, graphicsRenderTargetStride);
     
     memFree(tempPalette);
     
     #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
     vmemFree(tempBuffer);
     #endif
     
     #if FL_TEXTURE
     texBound = NULL;
     palBound = NULL;
     #endif
}

void graphicsEffectFsNightVisionSW() {
     u32 *tempBuffer = (u32*)memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
     
     int i, j;
     int tempLum = 0;
     for(j = 0; j < SCREEN_HEIGHT; j++) {
          for(i = 0; i < SCREEN_WIDTH; i++, tempBuffer++) {
               tempLum = max(max(color8888_Red(*tempBuffer), color8888_Green(*tempBuffer)), color8888_Blue(*tempBuffer));
               tempLum += min(min(color8888_Red(*tempBuffer), color8888_Green(*tempBuffer)), color8888_Blue(*tempBuffer));
               tempLum >>= 1;
               tempLum = ((tempLum * 255) / 192);
               tempLum = min(255, tempLum);
               *tempBuffer = color8888_RGBA(0, tempLum, 0, color8888_Alpha(*tempBuffer));
          }
          tempBuffer += (SCREEN_PITCH - SCREEN_WIDTH);
     }
}

void graphicsEffectFsThermalVision() {
     #if FL_MEMORY_VMEM_CACHE < 16384
     #if FL_MEMORY_VMEM != 0
     char* tempBuffer = vmemAlloc(16384);
     if(!tempBuffer) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate the 16k of vram required to perform this effect in hardware.\nSkipping algorithm.");
          #endif
          return;        
     }
     #else
     char* tempBuffer = (void*)0x44015400;
     return;
     #endif
     #else
     char* tempBuffer = (char*)vmemCache;
     #endif
     
     u8* tempPalette = memQalloc(2048);
     if(!tempPalette) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate aligned 1024 byte palette.\nSkipping algorithm");
          #endif
          #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
          vmemFree(tempBuffer);
          #endif
          graphicsEffectFsNightVisionSW();
          return;
     }
     u8* tempPtr = tempPalette;
     int i;
     for(i = 0; i < 256; i++) {
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = 0xFF;
     }

     for(i = 0; i < 256; i++) {
          if(i >= 224) {
               *(tempPtr++) = 0xFF;
               *(tempPtr++) = (255 - ((i - 224) << 3));
               *(tempPtr++) = 0;
          } else if(i >= 192) {
               *(tempPtr++) = ((i - 192) << 3);
               *(tempPtr++) = 0xFF;
               *(tempPtr++) = 0;
          } else if(i >= 160) {
               *(tempPtr++) = 0;
               *(tempPtr++) = 0xFF;
               *(tempPtr++) = (255 - ((i - 160) << 3));
          } else if(i >= 128) {
               *(tempPtr++) = 0;
               *(tempPtr++) = ((i - 128) << 3);
               *(tempPtr++) = 0xFF;
          } else {
               *(tempPtr++) = 0;
               *(tempPtr++) = 0;
               *(tempPtr++) = (i << 1);
          }
          *(tempPtr++) = 0xFF;
     }

     sceKernelDcacheWritebackInvalidateAll();

     #if FL_VMEM != 0
     void* tempDrawBuffer = memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
     #else
     void* tempDrawBuffer = (void*)((unsigned int)graphicsDrawBuffer | 0x44000000);
     #endif

     sceGuClutLoad(32, (void*)tempPalette);

     sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
     sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     sceGuTexMode(GU_PSM_T32, 0, 0, GU_FALSE);
     
     sceGuDrawBufferList(GU_PSM_8888, memCachedPtr(vmemRelativePtr(tempBuffer)), blockWidth);
     
     sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00FFFFFF, 0x00FFFFFF);
     
     int j;
     vertTsVs* tempVerts = NULL;
     for(j = 0; j < SCREEN_HEIGHT; j += blockHeight) {
          for(i = 0; i < SCREEN_PITCH; i += blockWidth) {
               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)((unsigned int)tempDrawBuffer + (((j * SCREEN_PITCH) + i) << 2)));

               sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDisable(GU_BLEND);
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               sceGuEnable(GU_BLEND);
               
               sceGuClutMode(GU_PSM_8888, 8, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuClutMode(GU_PSM_8888, 16, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

               sceGuCopyImage(GU_PSM_8888, 0, 0, blockWidthSafe, blockHeightSafe, blockWidth, (void*)tempBuffer, i, j, SCREEN_PITCH, (void*)tempDrawBuffer);
          }
     }

     sceGuClutLoad(32, (void*)((unsigned int)tempPalette + 1024));
     sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);

     sceGuDisable(GU_BLEND);
     for(j = 0; j < SCREEN_HEIGHT; j += blockHeight) {
          for(i = 0; i < SCREEN_PITCH; i += blockWidth) {
               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)((unsigned int)tempDrawBuffer + (((j * SCREEN_PITCH) + i) << 2)));
               
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuCopyImage(GU_PSM_8888, 0, 0, blockWidthSafe, blockHeightSafe, blockWidth, (void*)tempBuffer, i, j, SCREEN_PITCH, (void*)tempDrawBuffer);
          }
     }
     sceGuEnable(GU_BLEND);

     sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);     
     sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsDrawBuffer, SCREEN_STRIDE);
     //sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsRenderTarget, graphicsRenderTargetStride);
     
     memFree(tempPalette);
     
     #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
     vmemFree(tempBuffer);
     #endif
     
     #if FL_TEXTURE
     texBound = NULL;
     palBound = NULL;
     #endif
}

void graphicsEffectFsMonochrome(u32 inColor) {
     #if FL_MEMORY_VMEM_CACHE < 16384
     #if FL_MEMORY_VMEM != 0
     char* tempBuffer = vmemAlloc(16384);
     if(!tempBuffer) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate the 16k of vram required to perform this effect in hardware.\nSkipping algorithm.");
          #endif
          return;        
     }
     #else
     char* tempBuffer = (void*)0x44015400;
     return;
     #endif
     #else
     char* tempBuffer = (char*)vmemCache;
     #endif
     
     u8* tempPalette = memQalloc(1024);
     if(!tempPalette) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate aligned 1024 byte palette.\nSkipping algorithm");
          #endif
          #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
          vmemFree(tempBuffer);
          #endif
          graphicsEffectFsNightVisionSW();
          return;
     }
     u8* tempPtr = tempPalette;
     int i;
     for(i = 0; i < 256; i++) {
          *(tempPtr++) = (((i * (inColor & 0x000000FF)) >> 8) / 3);
          *(tempPtr++) = (((i * ((inColor & 0x0000FF00) >> 8)) >> 8) / 3);
          *(tempPtr++) = (((i * ((inColor & 0x00FF0000) >> 16)) >> 8) / 3);
          *(tempPtr++) = 0xFF;
     }

     sceKernelDcacheWritebackInvalidateAll();
     sceGuClutLoad(32, (void*)tempPalette);

     #if FL_VMEM != 0
     void* tempDrawBuffer = memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
     #else
     void* tempDrawBuffer = (void*)((unsigned int)graphicsDrawBuffer | 0x44000000);
     #endif

     sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
     sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     sceGuTexMode(GU_PSM_T32, 0, 0, GU_FALSE);
     
     sceGuDrawBufferList(GU_PSM_8888, memCachedPtr(vmemRelativePtr(tempBuffer)), blockWidth);
     
     sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00FFFFFF, 0x00FFFFFF);
     
     int j;
     vertTsVs* tempVerts = NULL;
     for(j = 0; j < SCREEN_HEIGHT; j += blockHeight) {
          for(i = 0; i < SCREEN_PITCH; i += blockWidth) {
               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)((unsigned int)tempDrawBuffer + (((j * SCREEN_PITCH) + i) << 2)));

               sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDisable(GU_BLEND);
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               sceGuEnable(GU_BLEND);
               
               sceGuClutMode(GU_PSM_8888, 8, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuClutMode(GU_PSM_8888, 16, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

               sceGuCopyImage(GU_PSM_8888, 0, 0, blockWidthSafe, blockHeightSafe, blockWidth, (void*)tempBuffer, i, j, SCREEN_PITCH, (void*)tempDrawBuffer);
          }
     }

     sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
     sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsDrawBuffer, SCREEN_STRIDE);
     //sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsRenderTarget, graphicsRenderTargetStride);
     
     memFree(tempPalette);
     
     #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
     vmemFree(tempBuffer);
     #endif
     
     #if FL_TEXTURE
     texBound = NULL;
     palBound = NULL;
     #endif
}

void graphicsEffectFsDichrome(u32 inColorLight, u32 inColorDark) {
     #if FL_MEMORY_VMEM_CACHE < 16384
     #if FL_MEMORY_VMEM != 0
     char* tempBuffer = vmemAlloc(16384);
     if(!tempBuffer) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate the 16k of vram required to perform this effect in hardware.\nSkipping algorithm.");
          #endif
          return;        
     }
     #else
     char* tempBuffer = (void*)0x44015400;
     return;
     #endif
     #else
     char* tempBuffer = (char*)vmemCache;
     #endif
     
     u8* tempPalette = memQalloc(2048);
     if(!tempPalette) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate aligned 1024 byte palette.\nSkipping algorithm");
          #endif
          #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
          vmemFree(tempBuffer);
          #endif
          return;
     }
     u8* tempPtr = tempPalette;
     int i;
     for(i = 0; i < 256; i++) {
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = 0xFF;
     }

     for(i = 0; i < 256; i++) {
          *(tempPtr++) = (((i * (inColorLight & 0x000000FF)) + ((255 - i) * (inColorDark & 0x000000FF))) >> 9);
          *(tempPtr++) = (((i * ((inColorLight & 0x0000FF00) >> 8)) + ((255 - i) * ((inColorDark & 0x0000FF00) >> 8))) >> 9);
          *(tempPtr++) = (((i * ((inColorLight & 0x00FF0000) >> 16)) + ((255 - i) * ((inColorDark & 0x00FF0000) >> 16))) >> 9);
          *(tempPtr++) = 0xFF;
     }

     sceKernelDcacheWritebackInvalidateAll();

     #if FL_VMEM != 0
     void* tempDrawBuffer = memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
     #else
     void* tempDrawBuffer = (void*)((unsigned int)graphicsDrawBuffer | 0x44000000);
     #endif

     sceGuClutLoad(32, (void*)tempPalette);

     sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
     sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     sceGuTexMode(GU_PSM_T32, 0, 0, GU_FALSE);
     
     sceGuDrawBufferList(GU_PSM_8888, memCachedPtr(vmemRelativePtr(tempBuffer)), blockWidth);
     
     sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00FFFFFF, 0x00FFFFFF);
     
     int j;
     vertTsVs* tempVerts = NULL;
     for(j = 0; j < SCREEN_HEIGHT; j += blockHeight) {
          for(i = 0; i < SCREEN_PITCH; i += blockWidth) {
               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)((unsigned int)tempDrawBuffer + (((j * SCREEN_PITCH) + i) << 2)));

               sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDisable(GU_BLEND);
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               sceGuEnable(GU_BLEND);
               
               sceGuClutMode(GU_PSM_8888, 8, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuClutMode(GU_PSM_8888, 16, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

               sceGuCopyImage(GU_PSM_8888, 0, 0, blockWidthSafe, blockHeightSafe, blockWidth, (void*)tempBuffer, i, j, SCREEN_PITCH, (void*)tempDrawBuffer);
          }
     }

     sceGuClutLoad(32, (void*)((unsigned int)tempPalette + 1024));
     sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);

     sceGuDisable(GU_BLEND);
     for(j = 0; j < SCREEN_HEIGHT; j += blockHeight) {
          for(i = 0; i < SCREEN_PITCH; i += blockWidth) {
               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)((unsigned int)tempDrawBuffer + (((j * SCREEN_PITCH) + i) << 2)));
               
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuCopyImage(GU_PSM_8888, 0, 0, blockWidthSafe, blockHeightSafe, blockWidth, (void*)tempBuffer, i, j, SCREEN_PITCH, (void*)tempDrawBuffer);
          }
     }
     sceGuEnable(GU_BLEND);

     sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);     
     sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsDrawBuffer, SCREEN_STRIDE);
     //sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsRenderTarget, graphicsRenderTargetStride);
     
     memFree(tempPalette);
     
     #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
     vmemFree(tempBuffer);
     #endif
     
     #if FL_TEXTURE
     texBound = NULL;
     palBound = NULL;
     #endif
}

void graphicsEffectFsColorEnhance() {
     #if FL_MEMORY_VMEM_CACHE < 16384
     #if FL_MEMORY_VMEM != 0
     char* tempBuffer = vmemAlloc(16384);
     if(!tempBuffer) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate the 16k of vram required to perform this effect in hardware.\nSkipping algorithm.");
          #endif
          return;        
     }
     #else
     char* tempBuffer = (void*)0x44015400;
     return;
     #endif
     #else
     char* tempBuffer = (char*)vmemCache;
     #endif
     
     #if FL_VMEM != 0
     void* tempDrawBuffer = memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
     #else
     void* tempDrawBuffer = (void*)((unsigned int)graphicsDrawBuffer | 0x44000000);
     #endif

     sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
     sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
     
     sceGuDrawBufferList(GU_PSM_8888, memCachedPtr(vmemRelativePtr(tempBuffer)), blockWidth);
     
     sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00FFFFFF, 0x00FFFFFF);
     
     int i, j;
     vertTsVs* tempVerts = NULL;
     for(j = 0; j < SCREEN_HEIGHT; j += blockHeight) {
          for(i = 0; i < SCREEN_PITCH; i += blockWidth) {
               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)((unsigned int)tempDrawBuffer + (((j * SCREEN_PITCH) + i) << 2)));
               sceGuCopyImage(GU_PSM_8888, i, j, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)tempDrawBuffer, 0, 0, blockWidth, (void*)tempBuffer);

               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuCopyImage(GU_PSM_8888, 0, 0, blockWidthSafe, blockHeightSafe, blockWidth, (void*)tempBuffer, i, j, SCREEN_PITCH, (void*)tempDrawBuffer);
          }
     }

     sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);     
     sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsDrawBuffer, SCREEN_STRIDE);
     //sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsRenderTarget, graphicsRenderTargetStride);

     #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
     vmemFree(tempBuffer);
     #endif
     
     #if FL_TEXTURE
     texBound = NULL;
     palBound = NULL;
     #endif
}

void graphicsEffectFsLightEnhance() {
     #if FL_MEMORY_VMEM_CACHE < 16384
     #if FL_MEMORY_VMEM != 0
     char* tempBuffer = vmemAlloc(16384);
     if(!tempBuffer) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate the 16k of vram required to perform this effect in hardware.\nSkipping algorithm.");
          #endif
          return;        
     }
     #else
     char* tempBuffer = (void*)0x44015400;
     return;
     #endif
     #else
     char* tempBuffer = (char*)vmemCache;
     #endif
     
     u8* tempPalette = memQalloc(1024);
     if(!tempPalette) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate aligned 1024 byte palette.\nSkipping algorithm");
          #endif
          #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
          vmemFree(tempBuffer);
          #endif
          graphicsEffectFsNightVisionSW();
          return;
     }
     u8* tempPtr = tempPalette;
     int i;
     for(i = 0; i < 256; i++) {
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = (i / 3);
          *(tempPtr++) = 0xFF;
     }

     sceKernelDcacheWritebackInvalidateAll();
     sceGuClutLoad(32, (void*)tempPalette);

     #if FL_VMEM != 0
     void* tempDrawBuffer = memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
     #else
     void* tempDrawBuffer = (void*)((unsigned int)graphicsDrawBuffer | 0x44000000);
     #endif

     sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
     sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     sceGuTexMode(GU_PSM_T32, 0, 0, GU_FALSE);
     
     sceGuDrawBufferList(GU_PSM_8888, memCachedPtr(vmemRelativePtr(tempBuffer)), blockWidth);
     
     sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00FFFFFF, 0x00FFFFFF);
     
     int j;
     vertTsVs* tempVerts = NULL;
     for(j = 0; j < SCREEN_HEIGHT; j += blockHeight) {
          for(i = 0; i < SCREEN_PITCH; i += blockWidth) {
               sceGuCopyImage(GU_PSM_8888, i, j, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)tempDrawBuffer, 0, 0, blockWidth, (void*)tempBuffer);
               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)((unsigned int)tempDrawBuffer + (((j * SCREEN_PITCH) + i) << 2)));

               sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuClutMode(GU_PSM_8888, 8, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuClutMode(GU_PSM_8888, 16, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

               sceGuCopyImage(GU_PSM_8888, 0, 0, blockWidthSafe, blockHeightSafe, blockWidth, (void*)tempBuffer, i, j, SCREEN_PITCH, (void*)tempDrawBuffer);
          }
     }

     sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
     sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsDrawBuffer, SCREEN_STRIDE);
     //sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsRenderTarget, graphicsRenderTargetStride);
     
     memFree(tempPalette);
     
     #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
     vmemFree(tempBuffer);
     #endif
     
     #if FL_TEXTURE
     texBound = NULL;
     palBound = NULL;
     #endif
}

void graphicsEffectFsInvert() {
     #if FL_MEMORY_VMEM_CACHE < 16384
     #if FL_MEMORY_VMEM != 0
     char* tempBuffer = vmemAlloc(16384);
     if(!tempBuffer) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate the 16k of vram required to perform this effect in hardware.\nReverting to slower algorithm.");
          #endif
          graphicsEffectFsNightVisionSW();
          return;        
     }
     #else
     char* tempBuffer = (void*)0x44015400;
     return;
     #endif
     #else
     char* tempBuffer = (char*)vmemCache;
     #endif
     
     u8* tempPalette = memQalloc(3072);
     if(!tempPalette) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate aligned 3072 bytes worth of palettes.\nReverting to slower algorithm");
          #endif
          #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
          vmemFree(tempBuffer);
          #endif
          graphicsEffectFsNightVisionSW();
          return;
     }
     u8* tempPtr = tempPalette;
     int i;
     for(i = 0; i < 256; i++) {
          *(tempPtr++) = (255 - i);
          *(tempPtr++) = 0;
          *(tempPtr++) = 0;
          *(tempPtr++) = 0xFF;
     }
     for(i = 0; i < 256; i++) {
          *(tempPtr++) = 0;
          *(tempPtr++) = (255 - i);
          *(tempPtr++) = 0;
          *(tempPtr++) = 0xFF;
     }
     for(i = 0; i < 256; i++) {
          *(tempPtr++) = 0;
          *(tempPtr++) = 0;
          *(tempPtr++) = (255 - i);
          *(tempPtr++) = 0xFF;
     }
     sceKernelDcacheWritebackInvalidateAll();

     #if FL_VMEM != 0
     void* tempDrawBuffer = memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
     #else
     void* tempDrawBuffer = (void*)((unsigned int)graphicsDrawBuffer | 0x44000000);
     #endif

     sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
     sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     sceGuTexMode(GU_PSM_T32, 0, 0, GU_FALSE);
     
     sceGuDrawBufferList(GU_PSM_8888, memCachedPtr(vmemRelativePtr(tempBuffer)), blockWidth);
     
     sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00FFFFFF, 0x00FFFFFF);
     
     int j;
     vertTsVs* tempVerts = NULL;
     for(j = 0; j < SCREEN_HEIGHT; j += blockHeight) {
          for(i = 0; i < SCREEN_PITCH; i += blockWidth) {
               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)((unsigned int)tempDrawBuffer + (((j * SCREEN_PITCH) + i) << 2)));

               sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);
               sceGuClutLoad(32, (void*)tempPalette);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDisable(GU_BLEND);
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               sceGuEnable(GU_BLEND);
               
               sceGuClutMode(GU_PSM_8888, 8, 0xFF, 0);
               sceGuClutLoad(32, (void*)((unsigned int)tempPalette + 1024));
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuClutMode(GU_PSM_8888, 16, 0xFF, 0);
               sceGuClutLoad(32, (void*)((unsigned int)tempPalette + 2048));
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

               sceGuCopyImage(GU_PSM_8888, 0, 0, blockWidthSafe, blockHeightSafe, blockWidth, (void*)tempBuffer, i, j, SCREEN_PITCH, (void*)tempDrawBuffer);
          }
     }

     sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);     
     sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsDrawBuffer, SCREEN_STRIDE);
     //sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsRenderTarget, graphicsRenderTargetStride);
     
     memFree(tempPalette);
     
     #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
     vmemFree(tempBuffer);
     #endif
     
     #if FL_TEXTURE
     texBound = NULL;
     palBound = NULL;
     #endif
}

void graphicsEffectFsGlow() {
     #if FL_MEMORY_VMEM_CACHE < 16384
     #if FL_MEMORY_VMEM != 0
     char* tempBuffer = vmemAlloc(16384);
     if(!tempBuffer) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate the 16k of vram required to perform this effect in hardware.\nSkipping algorithm.");
          #endif
          return;        
     }
     #else
     char* tempBuffer = (void*)0x44015400;
     return;
     #endif
     #else
     char* tempBuffer = (char*)vmemCache;
     #endif
     
     u8* tempPalette = memQalloc(3072);
     if(!tempPalette) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate aligned 1024 byte palette.\nSkipping algorithm");
          #endif
          #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
          vmemFree(tempBuffer);
          #endif
          graphicsEffectFsNightVisionSW();
          return;
     }
     u8* tempPtr = tempPalette;
     int i; u8 tempColor;
     for(i = 0; i < 256; i++) {
          tempColor = ((i * i) / 255);
          *(tempPtr++) = (tempColor >> 1);
          *(tempPtr++) = (tempColor >> 2);
          *(tempPtr++) = (tempColor >> 1);
          *(tempPtr++) = 0xFF;
     }
     
     for(i = 0; i < 256; i++) {
          tempColor = ((i * i) / 255);
          *(tempPtr++) = (tempColor >> 2);
          *(tempPtr++) = (tempColor >> 1);
          *(tempPtr++) = (tempColor >> 2);
          *(tempPtr++) = 0xFF;
     }
     
     for(i = 0; i < 256; i++) {
          tempColor = ((i * i) / 255);
          *(tempPtr++) = (tempColor >> 2);
          *(tempPtr++) = (tempColor >> 2);
          *(tempPtr++) = (tempColor >> 1);
          *(tempPtr++) = 0xFF;
     }

     sceKernelDcacheWritebackInvalidateAll();

     #if FL_VMEM != 0
     void* tempDrawBuffer = memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
     #else
     void* tempDrawBuffer = (void*)((unsigned int)graphicsDrawBuffer | 0x44000000);
     #endif

     sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00FFFFFF, 0x00FFFFFF);
     
     int j;
     vertTsVs* tempVerts = NULL;
     vertTfVs* tempVertsf = NULL;
     for(j = 0; j < SCREEN_HEIGHT; j += blockHeight) {
          for(i = 0; i < SCREEN_PITCH; i += blockWidth) {
               sceGuDrawBufferList(GU_PSM_8888, memCachedPtr(vmemRelativePtr(tempBuffer)), blockWidth);
                
               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, SCREEN_PITCH, (void*)((unsigned int)tempDrawBuffer + (((j * SCREEN_PITCH) + i) << 2)));
               sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
               sceGuTexFilter(GU_NEAREST, GU_NEAREST);
               sceGuTexMode(GU_PSM_T32, 0, 0, GU_FALSE);
     
               sceGuClutLoad(32, (void*)tempPalette);
               sceGuClutMode(GU_PSM_8888, 0, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDisable(GU_BLEND);
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               sceGuEnable(GU_BLEND);
               
               sceGuClutLoad(32, (void*)((u32)tempPalette + 1024));
               sceGuClutMode(GU_PSM_8888, 8, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
               
               sceGuClutLoad(32, (void*)((u32)tempPalette + 2048));
               sceGuClutMode(GU_PSM_8888, 16, 0xFF, 0);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = 0;
               tempVerts[0].vertY = 0;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 0;
               tempVerts[0].vertV = 0;
               tempVerts[1].vertX = blockWidthSafe;
               tempVerts[1].vertY = blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = blockWidthSafe;
               tempVerts[1].vertV = blockHeightSafe;
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

               sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsDrawBuffer, SCREEN_STRIDE);

               sceGuTexImage(0, blockWidthSafe, blockHeightSafe, blockWidth, (void*)tempBuffer);
               sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
               sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
               sceGuTexFilter(GU_LINEAR, GU_LINEAR);               

               sceGuTexWrap(GU_CLAMP, GU_CLAMP);

               tempVertsf = (vertTfVs*)sceGuGetMemory(sizeof(vertTfVs) << 1);
               tempVertsf[0].vertX = i;
               tempVertsf[0].vertY = j;
               tempVertsf[0].vertZ = 0;
               tempVertsf[0].vertU = -1.0f;
               tempVertsf[0].vertV = -1.0f;
               tempVertsf[1].vertX = i + blockWidthSafe;
               tempVertsf[1].vertY = j + blockHeightSafe;
               tempVertsf[1].vertZ = 0;
               tempVertsf[1].vertU = (float)(blockWidthSafe + 1);
               tempVertsf[1].vertV = (float)(blockHeightSafe + 1);
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);

               sceGuTexWrap(GU_REPEAT, GU_REPEAT);
               tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
               tempVerts[0].vertX = i;
               tempVerts[0].vertY = j;
               tempVerts[0].vertZ = 0;
               tempVerts[0].vertU = 1;
               tempVerts[0].vertV = 1;
               tempVerts[1].vertX = i + blockWidthSafe;
               tempVerts[1].vertY = j + blockHeightSafe;
               tempVerts[1].vertZ = 0;
               tempVerts[1].vertU = (blockWidthSafe - 1);
               tempVerts[1].vertV = (blockHeightSafe - 1);
               sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
           }
     }

     sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
     //sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsRenderTarget, graphicsRenderTargetStride);
     
     memFree(tempPalette);
     
     #if ((FL_MEMORY_VMEM_CACHE < 16384) && FL_MEMORY_VMEM)
     vmemFree(tempBuffer);
     #endif
     
     #if FL_TEXTURE
     texBound = NULL;
     palBound = NULL;
     #endif
}

void graphicsEffectFsDepthFog(u32 inColor, bool inLowBit) {
     u8* tempPalette = memQalloc(1024);
     if(!tempPalette) {
          #if DEBUG_ERROR != 0
          debugWarning("Couldn't allocate aligned 1024 byte palette.");
          #endif
          return;
     }
     u8* tempPtr = tempPalette;
     int i;
     for(i = 0; i < 256; i++) {
          *(tempPtr++) = color8888_Red(inColor);
          *(tempPtr++) = color8888_Green(inColor);
          *(tempPtr++) = color8888_Blue(inColor);
          *(tempPtr++) = ((i * i) / 255);
     }

     sceKernelDcacheWritebackInvalidateAll();
     sceGuClutLoad(32, (void*)tempPalette);
     sceGuClutMode(GU_PSM_8888, (inLowBit ? 0 : 8), 0xFF, 0);

     #if FL_VMEM != 0
     void* tempDepthBuffer = memUncachedPtr(vmemAbsolutePtr(graphicsDepthBuffer));
     #else
     void* tempDepthBuffer = (void*)((unsigned int)graphicsDepthBuffer | 0x44000000);
     #endif

     sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
     sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     sceGuTexMode(GU_PSM_T16, 0, 0, GU_FALSE);
     
     sceGuDisable(GU_DEPTH_TEST);
     
     vertTsVs* tempVerts = NULL;
     for(i = 0; i < SCREEN_WIDTH; i += blockWidth) {
          sceGuTexImage(0, blockWidthSafe, SCREEN_PITCH, SCREEN_PITCH, (void*)((unsigned int)tempDepthBuffer + (i << 1)));
          tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);
          tempVerts[0].vertX = i;
          tempVerts[0].vertY = 0;
          tempVerts[0].vertZ = 0;
          tempVerts[0].vertU = 0;
          tempVerts[0].vertV = 0;
          tempVerts[1].vertX = i + blockWidthSafe;
          tempVerts[1].vertY = SCREEN_HEIGHT;
          tempVerts[1].vertZ = 0;
          tempVerts[1].vertU = blockWidthSafe;
          tempVerts[1].vertV = SCREEN_HEIGHT;
          sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
     }
     
     sceGuEnable(GU_DEPTH_TEST);

     memFree(tempPalette);
     
     #if FL_TEXTURE
     texBound = NULL;
     palBound = NULL;
     #endif
}

#endif
