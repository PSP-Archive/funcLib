#include "flGlobal.h"
#if FL_TEXTURE_RAW != 0
#include <stdlib.h>
#include <string.h>
#include <pspgu.h>

#if FL_INCLUDE_ALL_C == 0
#include "flTextureRAW.h"
#include "flFile.h"

#if FL_DEBUG != 0
#include "flDebug.h"
#endif
#endif

Texture* texLoadRAW(char* inPath, u32 inWidth, u32 inHeight, u8 inPixelFormat) {
     #if FL_FILE != 0
     File* tempFile = fileOpen(inPath, FILE_MODE_READ | FILE_MODE_BINARY);
     #else
     FILE* tempFile = fopen(inPath, "rb");
     #endif
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          char tempString[256];
          sprintf(tempString, "RAW load error (%s).\nFile cannot be opened", inPath);
          debugError(tempString);
          #endif
          return NULL;
     }

     Texture* tempOut = texCreate(inWidth, inHeight, inPixelFormat);
     if(!tempOut) {
          fileClose(tempFile);
          #if FL_DEBUG_ERROR != 0
          char tempString[256];
          sprintf(tempString, "Couldn't create texture struct, while loading \"%s\".\nOut of memory.", inPath);
          debugError(tempString);
          #endif
          return NULL;
     }
     
     u32 i;
     for(i = 0; i < inHeight; i++)
          fileRead(&tempOut->texData[(i * tempOut->texDataWidth * texBPP(tempOut)) >> 3], ((texBPP(tempOut) * inWidth) >> 3), tempFile);

     fileClose(tempFile);
     
     return tempOut;
}

bool texSaveRAW(Texture* inTex, char* inPath) {
     if(texPalettized(inTex) && !inTex->texPalette) {
          #if FL_DEBUG != 0
          debugError("Trying to save palettized texture without palette.");
          #endif
          return false;
     }
     
     #if FL_FILE != 0
     File* tempFile = fileOpen(inPath, FILE_MODE_WRITE | FILE_MODE_BINARY);
     #else
     FILE* tempFile = fopen(inPath, "wb");
     #endif
     if(!tempFile) {
          fileClose(tempFile);
          #if FL_DEBUG_ERROR != 0
          char tempString[256];
          sprintf(tempString, "RAW save error (%s).\nFile cannot be opened", inPath);
          debugError(tempString);
          #endif
          return false;
     }
     
     #if FL_GRAPHICS != 0
     bool tempReswizzle = false;
     if(inTex->texSwizzled) {
          if(texUnswizzle(inTex)) {
               tempReswizzle = true;
          } else {
               #if FL_DEBUG != 0
               debugWarning("Can't un-swizzle texture for saving.");
               #endif
               return false;
          }
     }
     #else
     if(inTex->texSwizzled) {
          #if FL_DEBUG != 0
               debugWarning("Can't un-swizzle texture for saving, because flGraphics isn't compiled in.");
               #endif
          return false;
     }
     #endif
     
     if(texPalettized(inTex))
          fileWrite(tempOut->texPalette->palData, ((tempOut->texPalette->palEntries * palBPP(inTex->texPalette)) >> 3), tempFile);
     
     u32 i;
     for(i = 0; i < inHeight; i++)
          fileWrite(&tempOut->texData[(i * tempOut->texDataWidth * texBPP(tempOut)) >> 3], ((texBPP(tempOut) * inWidth) >> 3), tempFile);
     
     fileClose(tempFile);
     
     #if FL_GRAPHICS != 0
     if(tempReswizzle)
          texSwizzle(inTex);
     #endif
     
     return true;
}

#endif
