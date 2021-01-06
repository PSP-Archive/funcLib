#include "flGlobal.h"
#if FL_TEXTURE_PNG != 0
#include <stdlib.h>
#include <string.h>
#include <pspgu.h>

#if FL_INCLUDE_ALL_C == 0
#include "flTexturePNG.h"
#include "flFile.h"
#include "flMemory.h"

#if FL_DEBUG != 0
#include "flDebug.h"
#endif
#endif

Texture* texLoadPNG(char* inPath) {
     #if FL_FILE != 0
     File* tempFile = fileOpen(inPath, FILE_MODE_READ | FILE_MODE_BINARY);
     #else
     FILE* tempFile = fopen(inPath, "rb");
     #endif
     if(!tempFile) {
          #if FL_DEBUG_WARNING != 0
          char tempString[256];
          sprintf(tempString, "PNG load error (%s).\nFile cannot be opened.", inPath);
          debugWarning(tempString);
          #endif
          return NULL;
     }

     u8* tempPngHeader[8];
     fileRead(tempPngHeader, 8, tempFile);
     if(png_sig_cmp(tempPngHeader, 0, 8);) {
          #if FL_DEBUG_WARNING != 0
          char tempString[256];
          sprintf(tempString, "PNG load error (%s).\nFile is not in png format.", inPath);
          debugWarning(tempString);
          #endif
          return NULL;
     }


     fileClose(tempFile);
     
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
    
     return tempOut;
}

bool texSavePNG(Texture* inTex, char* inPath) {
     #if FL_DEBUG_ERROR != 0
     debugError("PNG saving not yet supported.");
     #endif
     
     return false;
}

#endif
