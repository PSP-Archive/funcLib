// Funclib TextureRAW v1.0.0
// 
// This module adds RAW texture loading/saving functionality.
// 
// Contributor(s): Flatmush



#ifndef FLTEXTURERAW_H
#define FLTEXTURERAW_H

#include "flGlobal.h"

#if FL_TEXTURE_RAW != 0

#include "flTexture.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Loads raw image data into a Texture struct.
  * @param inPath Path to the raw image data in memory or file.
  * @param inWidth Width of the raw image data.
  * @param inHeight Height of the raw image data.
  * @param inPixelFormat Pixel format of the raw image data using gu defines (e.g GU_PSM_8888).
  * @return A pointer to the texture struct created to hold the raw data, or NULL on failure.
  */
extern Texture* texLoadRAW(char* inPath, u32 inWidth, u32 inHeight, u8 inPixelFormat);

/**
  * Saves raw image data from a Texture struct.
  * @param inTex The texture to save as a raw data.
  * @param inPath Path to the file to which the raw image data will be written.
  * @return Whether the data was saved successfully or not.
  */
extern bool     texSaveRAW(Texture* inTex, char* inPath);

#ifdef __cplusplus
}
#endif

#endif

#endif
