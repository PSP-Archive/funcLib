// Funclib TextureTGA v1.0.0
// 
// This module adds TGA texture loading and saving functionality.
// 
// Contributor(s): Flatmush



#ifndef FLTEXTURETGA_H
#define FLTEXTURETGA_H

#include "flGlobal.h"

#if FL_TEXTURE_TGA

#include "flTexture.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Loads a tga image into a Texture struct.
  * @param inPath Path to the tga image in memory or file.
  * @return A pointer to the texture struct created to hold the tga, or NULL on failure.
  * @warning Currently 16-bit tga's are not properly supported however all others are, including RLE.
  */
extern Texture* texLoadTGA(char* inPath);

/**
  * Saves a tga image from a Texture struct.
  * @param inTex The texture to save as a tga.
  * @param inPath Path to the file to which the tga image will be written.
  * @return Whether the tga was saved successfully or not.
  * @warning Currently 16-bit tga's are not properly supported however all others are, though not saved with RLE.
  */
extern bool     texSaveTGA(Texture* inTex, char* inPath);

#ifdef __cplusplus
}
#endif

#endif

#endif
