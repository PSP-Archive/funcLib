// Funclib Color v1.0.0
// 
// This module contains color manipulation functions.
// Use it in games/apps where complex colors are required.
// This module doesn't have any module requirements.
// This module works better when used in conjunction with MATH.
// 
// Contributor(s): Flatmush



#ifndef FLCOLOR_H
#define FLCOLOR_H

#include "flGlobal.h"

#if FL_COLOR

#ifdef __cplusplus
extern "C" {
#endif

/*extern u8 colorRed8888(u32 inColor);
extern u8  colorGreen8888(u32 inColor);
extern u8  colorBlue8888(u32 inColor);
extern u8  colorAlpha8888(u32 inColor);
extern u32 colorRGB8888(u8 inRed, u8 inGreen, u8 inBlue);
extern u32 colorRGBA8888(u8 inRed, u8 inGreen, u8 inBlue, u8 inAlpha);*/
#define colorRed8888(inColor) color8888_Red(inColor)
#define colorGreen8888(inColor) color8888_Green(inColor)
#define colorBlue8888(inColor) color8888_Blue(inColor)
#define colorAlpha8888(inColor) color8888_Alpha(inColor)
#define colorRGB8888(inRed, inGreen, inBlue) color8888_RGB(inRed, inGreen, inBlue)
#define colorRGBA8888(inRed, inGreen, inBlue, inAlpha) color8888_RGBA(inRed, inGreen, inBlue, inAlpha)


extern u32 colorHSL(u8 inHue, u8 inSat, u8 inLum);
extern u8  colorHue8888(u32 inColor);
extern u8  colorLum8888(u32 inColor);
extern u8  colorSat8888(u32 inColor);
//extern u32 colorBlend8888(u32 inColor0, u32 inColor1, float inRatio);
#define colorBlend8888(inColor0, inColor1, inRatio) color8888_Blend(inColor0, inColor1, inRatio)
#define colorMerge8888(inColor0, inColor1) color8888_Merge(inColor0, inColor1)
extern u32 colorMergeN8888(u32* inColor, u32 inColorCount);
extern u16 colorMergeN4444(u16* inColor, u16 inColorCount);
extern u16 colorMergeN5551(u16* inColor, u16 inColorCount);
extern u16 colorMergeN565(u16* inColor, u16 inColorCount);
#define colorMergeN5650(inColor, inColorCount) colorMergeN565(inColor, inColorCount)
extern u32 colorRandom8888(u8 inBaseLum, u8 inRandLum, u8 inAlpha);
extern u32 colorLumMult8888(u32 inColor, float inLum);
extern u32 colorAlphaMult8888(u32 inColor, float inMult);

#ifdef __cplusplus
}
#endif

#else

#define colorRed8888(inColor) color8888_Red(inColor)
#define colorGreen8888(inColor) color8888_Green(inColor)
#define colorBlue8888(inColor) color8888_Blue(inColor)
#define colorAlpha8888(inColor) color8888_Alpha(inColor)
#define colorRGB8888(inRed, inGreen, inBlue) color8888_RGB(inRed, inGreen, inBlue)
#define colorRGBA8888(inRed, inGreen, inBlue, inAlpha) color8888_RGBA(inRed, inGreen, inBlue, inAlpha)
#define colorBlend8888(inColor0, inColor1, inRatio) color8888_Blend(inColor0, inColor1, inRatio)
#define colorMerge8888(inColor0, inColor1) color8888_Merge(inColor0, inColor1)

#endif

#define color8888_Red(inColor) ((inColor) & 0x000000FF)
#define color8888_Green(inColor) (((inColor) & 0x0000FF00) >> 8)
#define color8888_Blue(inColor) (((inColor) & 0x00FF0000) >> 16)
#define color8888_Alpha(inColor) (((inColor) & 0xFF0000) >> 24)
#define color8888_SetAlpha(inColor, inAlpha) (((inColor) & 0x00FFFFFF) | (((inAlpha) & 0xFF) << 24))
#define color8888_RGBA(inRed, inGreen, inBlue, inAlpha) (((inRed) + ((inGreen) << 8) + ((inBlue) << 16)) | ((inAlpha) << 24))
#define color8888_RGB(inRed, inGreen, inBlue) (((inRed) + ((inGreen) << 8) + ((inBlue) << 16)) | (0xFF000000))
#define color8888_Merge(inColor0, inColor1) color8888_RGBA(((color8888_Red(inColor0) + color8888_Red(inColor1)) >> 1), ((color8888_Green(inColor0) + color8888_Green(inColor1)) >> 1), ((color8888_Blue(inColor0) + color8888_Blue(inColor1)) >> 1), ((color8888_Alpha(inColor0) + color8888_Alpha(inColor1)) >> 1))
#define color8888_Merge3(inColor0, inColor1, inColor2) color8888_RGBA(((color8888_Red(inColor0) + color8888_Red(inColor1) + color8888_Red(inColor2)) / 3), ((color8888_Green(inColor0) + color8888_Green(inColor1) + color8888_Green(inColor2)) / 3), ((color8888_Blue(inColor0) + color8888_Blue(inColor1) + color8888_Blue(inColor2)) / 3), ((color8888_Alpha(inColor0) + color8888_Alpha(inColor1) + color8888_Alpha(inColor2)) / 3))
#define color8888_Merge4(inColor0, inColor1, inColor2, inColor3) color8888_RGBA(((color8888_Red(inColor0) + color8888_Red(inColor1) + color8888_Red(inColor2) + color8888_Red(inColor3)) >> 2), ((color8888_Green(inColor0) + color8888_Green(inColor1) + color8888_Green(inColor2) + color8888_Green(inColor3)) >> 2), ((color8888_Blue(inColor0) + color8888_Blue(inColor1) + color8888_Blue(inColor2) + color8888_Blue(inColor3)) >> 2), ((color8888_Alpha(inColor0) + color8888_Alpha(inColor1) + color8888_Alpha(inColor2) + color8888_Alpha(inColor3)) >> 2))
#define color8888_Blend(inColor0, inColor1, inFract) color8888_RGBA((int)(((float)color8888_Red(inColor0) * (float)(inFract)) + ((float)color8888_Red(inColor1) * (1.0f - (float)(inFract)))), (int)(((float)color8888_Green(inColor0) * (float)(inFract)) + ((float)color8888_Green(inColor1) * (1.0f - (float)(inFract)))), (int)(((float)color8888_Blue(inColor0) * (float)(inFract)) + ((float)color8888_Blue(inColor1) * (1.0f - (float)(inFract)))), (int)(((float)color8888_Alpha(inColor0) * (float)(inFract)) + ((float)color8888_Alpha(inColor1) * (1.0f - (float)(inFract)))))
#define color8888_CMYK(inCyan, inMagenta, inYellow, inKey) color8888_Merge3(color8888_Blend(0xFFFFFF00, colorRGB((inKey), (inKey), (inKey)), ((float)(inCyan) / 255.0f)), color8888_Blend(0xFFFF00FF, colorRGB((inKey), (inKey), (inKey)), ((float)(inMagenta) / 255.0f)), color8888_Blend(0xFF00FFFF, colorRGB((inKey), (inKey), (inKey)), ((float)(inYellow) / 255.0f)))
#define color8888_CMYKA(inCyan, inMagenta, inYellow, inKey, inAlpha) color8888_SetAlpha(color8888_CMYK(inCyan, inMagenta, inYellow, inKey), inAlpha)
#define color8888_CMY(inCyan, inMagenta, inYellow) color8888_RGB((255 - (inCyan)), (255 - (inMagenta)), (255 - (inYellow)))
#define color8888_CMYA(inCyan, inMagenta, inYellow, inAlpha) color8888_SetAlpha(color8888_CMY(inCyan, inMagenta, inYellow), inAlpha)
#define color8888_Cmp(inColor0, inColor1) (pos((int)color8888_Red(inColor0) - (int)color8888_Red(inColor1)) + pos((int)color8888_Green(inColor0) - (int)color8888_Green(inColor1)) + pos((int)color8888_Blue(inColor0) - (int)color8888_Blue(inColor1)) + pos((int)color8888_Alpha(inColor0) - (int)color8888_Alpha(inColor1)))
#define color8888_Compare(inColor0, inColor1) color8888_Cmp(inColor0, inColor1)

#define color4444_Red(inColor) ((inColor) & 0x000F)
#define color4444_Green(inColor) (((inColor) & 0x00F0) >> 4)
#define color4444_Blue(inColor) (((inColor) & 0x0F00) >> 8)
#define color4444_Alpha(inColor) (((inColor) & 0xF000) >> 12)
#define color4444_RGBA(inRed, inGreen, inBlue, inAlpha) (((inRed) & 0x000F) + (((inGreen) & 0x000F) << 4) + (((inBlue) & 0x000F) << 8) + (((inAlpha) & 0x000F) << 12))
#define color4444_RGB(inRed, inGreen, inBlue) color4444_RGBA(inRed, inGreen, inBlue, 15);
#define color4444_Merge(inColor0, inColor1) color4444_RGBA(((color4444_Red(inColor0) + color4444_Red(inColor1)) >> 1), ((color4444_Green(inColor0) + color4444_Green(inColor1)) >> 1), ((color4444_Blue(inColor0) + color4444_Blue(inColor1)) >> 1), ((color4444_Alpha(inColor0) + color4444_Alpha(inColor1)) >> 1))
#define color4444_Merge4(inColor0, inColor1, inColor2, inColor3) color4444_RGBA(((color4444_Red(inColor0) + color4444_Red(inColor1) + color4444_Red(inColor2) + color4444_Red(inColor3)) >> 2), ((color4444_Green(inColor0) + color4444_Green(inColor1) + color4444_Green(inColor2) + color4444_Green(inColor3)) >> 2), ((color4444_Blue(inColor0) + color4444_Blue(inColor1) + color4444_Blue(inColor2) + color4444_Blue(inColor3)) >> 2), ((color4444_Alpha(inColor0) + color4444_Alpha(inColor1) + color4444_Alpha(inColor2) + color4444_Alpha(inColor3)) >> 2))
#define color4444_Cmp(inColor0, inColor1) (pos((int)color4444_Red(inColor0) - (int)color4444_Red(inColor1)) + pos((int)color4444_Green(inColor0) - (int)color4444_Green(inColor1)) + pos((int)color4444_Blue(inColor0) - (int)color4444_Blue(inColor1)) + pos((int)color4444_Alpha(inColor0) - (int)color4444_Alpha(inColor1)))
#define color4444_Compare(inColor0, inColor1) color4444_Cmp(inColor0, inColor1)

#define color5551_Red(inColor) ((inColor) & 0x001F)
#define color5551_Green(inColor) (((inColor) & 0x03E0) >> 5)
#define color5551_Blue(inColor) (((inColor) & 0x7C00) >> 10)
#define color5551_Alpha(inColor) (((inColor) & 0x8000) >> 15)
#define color5551_RGBA(inRed, inGreen, inBlue, inAlpha) (((inRed) & 0x001F) + (((inGreen) & 0x001F) << 5) + (((inBlue) & 0x001F) << 10) + (((inAlpha) & 0x0001) << 15))
#define color5551_RGB(inRed, inGreen, inBlue) color5551_RGBA(inRed, inGreen, inBlue, 1);
#define color5551_Merge(inColor0, inColor1) color5551_RGBA(((color5551_Red(inColor0) + color5551_Red(inColor1)) >> 1), ((color5551_Green(inColor0) + color5551_Green(inColor1)) >> 1), ((color5551_Blue(inColor0) + color5551_Blue(inColor1)) >> 1), ((color5551_Alpha(inColor0) + color5551_Alpha(inColor1)) >> 1))
#define color5551_Merge4(inColor0, inColor1, inColor2, inColor3) color5551_RGBA(((color5551_Red(inColor0) + color5551_Red(inColor1) + color5551_Red(inColor2) + color5551_Red(inColor3)) >> 2), ((color5551_Green(inColor0) + color5551_Green(inColor1) + color5551_Green(inColor2) + color5551_Green(inColor3)) >> 2), ((color5551_Blue(inColor0) + color5551_Blue(inColor1) + color5551_Blue(inColor2) + color5551_Blue(inColor3)) >> 2), ((color5551_Alpha(inColor0) + color5551_Alpha(inColor1) + color5551_Alpha(inColor2) + color5551_Alpha(inColor3)) >> 2))
#define color5551_Cmp(inColor0, inColor1) (pos((int)color5551_Red(inColor0) - (int)color5551_Red(inColor1)) + pos((int)color5551_Green(inColor0) - (int)color5551_Green(inColor1)) + pos((int)color5551_Blue(inColor0) - (int)color5551_Blue(inColor1)) + pos((int)color5551_Alpha(inColor0) - (int)color5551_Alpha(inColor1)))
#define color5551_Compare(inColor0, inColor1) color5551_Cmp(inColor0, inColor1)

#define color565_Red(inColor) ((inColor) & 0x001F)
#define color565_Green(inColor) (((inColor) & 0x07E0) >> 5)
#define color565_Blue(inColor) (((inColor) & 0xF800) >> 11)
#define color565_RGB(inRed, inGreen, inBlue) (((inRed) & 0x001F) + (((inGreen) & 0x003F) << 5) + (((inBlue) & 0x001F) << 11))
#define color565_Merge(inColor0, inColor1) color565_RGB(((color565_Red(inColor0) + color565_Red(inColor1)) >> 1), ((color565_Green(inColor0) + color565_Green(inColor1)) >> 1), ((color565_Blue(inColor0) + color565_Blue(inColor1)) >> 1))
#define color565_Merge4(inColor0, inColor1, inColor2, inColor3) color565_RGB(((color565_Red(inColor0) + color565_Red(inColor1) + color565_Red(inColor2) + color565_Red(inColor3)) >> 2), ((color565_Green(inColor0) + color565_Green(inColor1) + color565_Green(inColor2) + color565_Green(inColor3)) >> 2), ((color565_Blue(inColor0) + color565_Blue(inColor1) + color565_Blue(inColor2) + color565_Blue(inColor3)) >> 2))
#define color565_Cmp(inColor0, inColor1) (pos((int)color565_Red(inColor0) - (int)color565_Red(inColor1)) + pos((int)color565_Green(inColor0) - (int)color565_Green(inColor1)) + pos((int)color565_Blue(inColor0) - (int)color565_Blue(inColor1)))
#define color565_Compare(inColor0, inColor1) color565_Cmp(inColor0, inColor1)

#define color5650_Red(inColor) color565_Red(inColor)
#define color5650_Green(inColor) color565_Green(inColor)
#define color5650_Blue(inColor) color565_Blue(inColor)
#define color5650_RGB(inRed, inGreen, inBlue) color565_RGB(inRed, inGreen, inBlue)
#define color5650_Merge(inColor0, inColor1) color565_Merge(inColor0, inColor1)
#define color5650_Merge4(inColor0, inColor1, inColor2, inColor3) color565_Merge4(inColor0, inColor1, inColor2, inColor3)
#define color5650_Cmp(inColor0, inColor1) color565_Cmp(inColor0, inColor1)
#define color5650_Compare(inColor0, inColor1) color565_Compare(inColor0, inColor1)

#endif
