// Funclib TextureBMP v1.0.0
// 
// This module adds BMP texture loading/saving functionality.
// A lot of features are untested as I have no way of generating the files needed
// however any images made using mspaint and GIMP are guaranteed to work, also
// as most features worked the first time I tested them I can be pretty sure that
// other features like RLE and older/newer formats will work.
// Currently bit-fields are unsupported, however most apps don't use them (luckily).
// However most normal alpha representations are supported.
// 
// Contributor(s): Flatmush



#ifndef FLTEXTUREBMP_H
#define FLTEXTUREBMP_H

#include "flGlobal.h"

#if FL_TEXTURE_BMP

#include "flTexture.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Loads a bitmap image into a Texture struct.
  * @param inPath Path to the bitmap image in memory or file.
  * @return A pointer to the texture struct created to hold the bmp, or NULL on failure.
  */
extern Texture* texLoadBMP(char* inPath);

/**
  * Saves a bitmap image from a Texture struct.
  * @param inTex The texture to save as a bmp.
  * @param inPath Path to the file to which the bitmap image will be written.
  * @return Whether the bitmap was saved successfully or not.
  * @warning This will always return false an log an error as it is not yet implemented.
  */
extern bool     texSaveBMP(Texture* inTex, char* inPath);

#ifdef __cplusplus
}
#endif

#endif

#endif
