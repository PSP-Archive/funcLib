// Funclib TexturePNG v1.0.0
// 
// This module adds PNG texture loading/saving functionality.
// 
// Contributor(s): Flatmush



#ifndef FLTEXTUREPNG_H
#define FLTEXTUREPNG_H

#include "flGlobal.h"

#if FL_TEXTURE_PNG != 0

#include "flTexture.h"

#ifdef __cplusplus
extern "C" {
#endif

extern Texture* texLoadPNG(char* inPath);
extern bool     texSavePNG(Texture* inTex, char* inPath);

#ifdef __cplusplus
}
#endif

#endif

#endif
