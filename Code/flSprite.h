// Funclib Sprite v1.0.0
// 
// This module contains functions for using sprites.
// These are very useful in 2d games and games with complex huds.
// This module requires TEXTURE, also sprites must be drawn in ortho.
// 
// Contributor(s): Flatmush



#ifndef FLSPRITE_H
#define FLSPRITE_H

#include "flGlobal.h"

#if FL_SPRITE != 0

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
     Texture* spriteTex;
     float    spriteX, spriteY;
     int      spriteWidth, spriteHeight;
     float    spriteAngle;
     float    spriteScaleX, spriteScaleY;

     int      spriteFrame;
     int      spriteFrameCount;
} Sprite;

extern Sprite* spriteCreate(Texture* inTex, int inFrameWidth, int inFrameHeight);
extern void    spriteFree(Sprite* inSprite);
extern void    spriteRotate(Sprite* inSprite, float inAngle);
extern void    spriteResize(Sprite* inSprite, float inWidth, float inHeight);
extern void    spriteMove(Sprite* inSprite, int inX, int inY);
extern void    spriteMoveTo(Sprite* inSprite, int inX, int inY);
extern void    spriteDraw2d(Sprite* inSprite);
extern void    spriteDraw2dFrame(Sprite* inSprite, int inFrame);

#ifdef __cplusplus
}
#endif

#endif

#endif
