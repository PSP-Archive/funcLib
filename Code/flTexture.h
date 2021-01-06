// Funclib Texture v1.0.0
// 
// This module contains functions for dealing with textures and palettes.
// Required for almost any 2d/3d graphical application.
// This module requires FL_GRAPHICS to be fully functional.
// 
// Contributor(s): Flatmush



#ifndef FLTEXTURE_H
#define FLTEXTURE_H

#include "flGlobal.h"

#if FL_TEXTURE != 0

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
     u32   palEntries;
     char* palData;
     u8    palPixelFormat;
     bool  palGlobal;
} Palette;

typedef struct {
     u32      texWidth;
     u32      texHeight;
     u32      texDataWidth;
     u32      texDataHeight;

     u32      texStride;
     char*    texData;
     char*    texMipData[8];
     Palette* texPalette;
     
     u32      texUseCount;
     u8       texPixelFormat;
     bool     texSwizzled;
     u8       texMipMaps;
     #if FL_TEXTURE_PRESERVENONALPHA != 0
     bool     texAlpha;
     #endif
} Texture;

extern void* texBound;
extern void* palBound;

extern void        texInit();

extern Texture*    texLoad(char* inPath);
extern bool        texSave(Texture* inTex, char* inPath);
extern char*       texPath(Texture* inTex);

extern Texture*    texCreate(u32 inWidth, u32 inHeight, u8 inPixelFormat);
extern Texture*    texCreateBlank(u32 inWidth, u32 inHeight, u8 inPixelFormat);

#if FL_GRAPHICS != 0
extern bool        texSwizzle(Texture* inTex);
extern bool        texUnswizzle(Texture* inTex);
#endif

extern void        texBind(Texture* inTex);
extern void        texFree(void* inTex);

#if FL_GRAPHICS != 0
extern u8          texBPP(Texture* inTex);
#else
#define texBPP(inTex) (graphicsPixelFormatBPP(inTex->texPixelFormat))
#endif
#define texDataSize(inTex) (((inTex)->texDataWidth * (inTex)->texDataHeight * texBPP(inTex)) >> 3)
extern u8          texPalettized(Texture* inTex);

extern int         texMipMap(Texture* inTex, u8 inLevel);
extern bool        texMipMapAdd(Texture* inTex);


extern Palette*    palCreate(u32 inEntries, u8 inFormat, bool inGlobal);
extern void        palBind(Palette* inPalette);
extern void        palFree(Palette* inPalette);
extern u8          palBPP(Palette* inPal);
extern s32         palClosestColor(Palette* inPal, u32 inColor);

extern Palette*    palCreateGrayscaleT8();
extern Palette*    palCreateGrayscaleT4();
extern Palette*    palCreateMonoT4(u32 inForeColor, u32 inBackColor);
extern Palette*    palCreateDefaultColorT4();
extern Palette*    palGrayscaleT8();
extern Palette*    palGrayscaleT4();
extern Palette*    palDefaultColorT4();

#if FL_GRAPHICS != 0
extern Texture*    texScreenshot();
#endif

#if FL_MEMORY_VMEM != 0
extern bool        texMoveVMem(Texture* inTex);
extern bool        palMoveVMem(Palette* inPal);
#endif

extern u32         texPixel(Texture* inTex, int inX, int inY);
//extern void        texAddMagMap(Texture* inTex);


#ifdef __cplusplus
}
#endif

#endif

#endif
