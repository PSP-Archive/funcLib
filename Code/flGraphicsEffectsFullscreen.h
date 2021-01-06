// Funclib GraphicsEffectsFullscreen v1.0.0
// 
// This module contains fullscreen effects, however be careful a lot of these
// modify vram and/or are per pixel so they are very heavy. Use most of them
// for things like pause menus.
// 
// Contributor(s): Flatmush



#ifndef FLGRAPHICSEFFECTSFULLSCREEN_H
#define FLGRAPHICSEFFECTSFULLSCREEN_H

#include "flGlobal.h"

#if FL_GRAPHICS_EFFECTS_FULLSCREEN != 0

#ifdef __cplusplus
extern "C" {
#endif

#define EFFECT_TYPE_NONE          0
#define EFFECT_TYPE_GRAYSCALE     1
#define EFFECT_TYPE_SEPIA         2
#define EFFECT_TYPE_NIGHTVISION   3
#define EFFECT_TYPE_THERMALVISION 4
#define EFFECT_TYPE_MONOCHROME    5
#define EFFECT_TYPE_DICHROME      6
#define EFFECT_TYPE_COLORENHANCE  7
#define EFFECT_TYPE_LIGHTENHANCE  8
#define EFFECT_TYPE_INVERT        9
#define EFFECT_TYPE_GLOW          10
#define EFFECT_TYPE_DEPTHFOG      11

typedef struct {
     u32   effectType;
     void* effectData;
     void* effectNext;
     u32   effectReserved;
} EffectContext;

extern void graphicsEffectFsGrayscale();
extern void graphicsEffectFsGrayscaleSW();
extern void graphicsEffectFsSepia();
extern void graphicsEffectFsNightVision();
extern void graphicsEffectFsNightVisionSW();
extern void graphicsEffectFsThermalVision();
extern void graphicsEffectFsMonochrome(u32 inColor);
extern void graphicsEffectFsDichrome(u32 inColorLight, u32 inColorDark);
extern void graphicsEffectFsColorEnhance();
extern void graphicsEffectFsLightEnhance();
extern void graphicsEffectFsInvert();
extern void graphicsEffectFsGlow();
extern void graphicsEffectFsDepthFog(u32 inColor, bool inLowBit);

#ifdef __cplusplus
}
#endif

#endif

#endif
