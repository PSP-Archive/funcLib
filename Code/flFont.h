// Funclib Font v1.0.0
// 
// This module contains functions for dealing with fonts.
// Use it to draw fonts in games and applications.
// This module requires doesn't require anything else, however printing characters
// from a debug font works better with GRAPHICS and better again with TEXTURE and GRAPHICS_2D, loading a texture font requires TEXTURE and
// rendering a texture font requires GRAPHICS, TEXTURE and GRAPHICS_2D.
// 
// Contributor(s): Flatmush



#ifndef FLFONT_H
#define FLFONT_H

#include "flGlobal.h"

#if FL_FONT != 0

#if FL_TEXTURE != 0
#include "flTexture.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define FONT_TYPE_NONE         0
#define FONT_TYPE_TEXTURE      1
#define FONT_TYPE_DEBUG        2
#define FONT_TYPE_TEXTURE_MONO 3

typedef struct {
     u8    fontType;
     void* fontData;
     int   fontHSpace;
     int   fontVSpace;
     u32   fontColor;
     u32   fontBackColor;
     bool  fontFixedWidth;
     bool  fontDataGlobal;
} Font;

extern Font* fontDefault;

extern void  fontInit();
extern Font* fontLoad(char* inPath);
extern bool  fontSave(char* inPath, Font* inFont);
#if FL_TEXTURE != 0
extern Font* fontCreateTexture(Texture* inTex, int inHSpace, int inVSpace, u32 inColor);
#endif
extern Font* fontCreateDebug(int inHSpace, int inVSpace, u32 inColor);
#if FL_GRAPHICS != 0
extern bool  fontCreateDebugTex();
extern void fontDebugSetColor(u32 inColor, u32 inBackColor);
#endif
extern void  fontFree(Font* inFont);

extern int   fontHeight(Font* inFont);
extern int   fontCharWidth(Font* inFont, char inChar);
extern int   fontStringWidth(Font* inFont, char* inString);
extern int   fontStringHeight(Font* inFont, char* inString);

extern bool  fontDraw2dChar(int inX, int inY, Font* inFont, char inChar);
extern void  fontDraw2dString(int inX, int inY, Font* inFont, char* inString);
extern void  fontDraw2dStringCenter(int inY, Font* inFont, char* inString);
//extern void  fontDraw2dStringFromRight(int inX, int inY, Font* inFont, char* inString);
#define fontDraw2dStringFromRight(inX, inY, inFont, inString) fontDraw2dString((inX - fontStringWidth(inFont, inString)), inY, inFont, inString);
   
#ifdef __cplusplus
}
#endif

#endif

#endif
