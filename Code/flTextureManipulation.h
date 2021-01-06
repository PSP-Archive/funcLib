// Funclib TextureManipulation v1.0.0
// 
// This module contains texture manipulation functions.
// It can be used to palettize, convert between bit-depths,
// replace colors and do all kinds of effects.
// 
// Contributor(s): Flatmush



#ifndef FLTEXTUREMANIPULATION_H
#define FLTEXTUREMANIPULATION_H

#include "flGlobal.h"
#include "flTexture.h"

#if FL_TEXTURE_MANIPULATION != 0

#ifdef __cplusplus
extern "C" {
#endif

void texMnpColorReplace(Texture* inTex, u32 inColor, u32 inReplace);

#ifdef __cplusplus
}
#endif

#endif

#endif
