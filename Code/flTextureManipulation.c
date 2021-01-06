#include "flGlobal.h"
#if FL_TEXTURE_MANIPULATION != 0
#include <stdlib.h>
#include <stdio.h>
#include <pspkernel.h>

#if FL_INCLUDE_ALL_C == 0
#include "flTextureManipulation.h"
#include "flTexture.h"
#endif

void texMnpColorReplace(Texture* inTex, u32 inColor, u32 inReplace) {
     if(inTex->texPixelFormat > GU_PSM_8888) {
          #if DEBUG_WARNING != 0
          debugWarning("Color replacement is currently only supported for true-color textures.");
          #endif
          return;
     }
     int i, j;
     if(inTex->texPixelFormat == GU_PSM_8888) {
          u32* tempData = (u32*)inTex->texData;
          for(j = 0; j < inTex->texHeight; j++) {
               for(i = 0; i < inTex->texWidth; i++) {
                    if(tempData[(j * inTex->texDataWidth) + i] == inColor)
                         tempData[(j * inTex->texDataWidth) + i] = inReplace;
               }
          }
     } else if(inTex->texPixelFormat < GU_PSM_8888) {
          u16* tempData = (u16*)inTex->texData;
          for(j = 0; j < inTex->texHeight; j++) {
               for(i = 0; i < inTex->texWidth; i++) {
                    if(tempData[(j * inTex->texDataWidth) + i] == (inColor & 0x0000FFFF))
                         tempData[(j * inTex->texDataWidth) + i] = (inReplace & 0x0000FFFF);
               }
          }
     }
     #if FL_TEXTURE_UNCACHED == 0
     sceKernelDcacheWritebackInvalidateAll();
     #endif
}

#endif
