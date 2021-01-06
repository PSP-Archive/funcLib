// Funclib TexturePCX v1.0.0
// 
// This module adds PCX texture loading functionality.
// 
// Contributor(s): Flatmush



#ifndef FLTEXTUREPCX_H
#define FLTEXTUREPCX_H

#include "flGlobal.h"

#if FL_TEXTURE_PCX

#include "flTexture.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Loads a pcx image into a Texture struct.
  * @param inPath Path to the pcx image in memory or file.
  * @return A pointer to the texture struct created to hold the pcx, or NULL on failure.
  */
extern Texture* texLoadPCX(char* inPath);

/**
  * Saves a pcx image from a Texture struct.
  * @param inTex The texture to save as a pcx.
  * @param inPath Path to the file to which the pcx image will be written.
  * @return Whether the pcx was saved successfully or not.
  * @warning This will always return false an log an error as it is not yet implemented.
  */
extern bool     texSavePCX(Texture* inTex, char* inPath);

#ifdef __cplusplus
}
#endif

#endif

#endif
