#include "flGlobal.h"
#if FL_SPRITE != 0
#include <stdlib.h>
#include <stdio.h>
#include <pspgu.h>
#include <pspgum.h>

#if FL_INCLUDE_ALL_C == 0
#include "flSprite.h"
#include "flTexture.h"
#include "flMemory.h"
#include "flMath.h"
#endif

Sprite* spriteCreate(Texture* inTex, int inFrameWidth, int inFrameHeight) {
     if(!inTex) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to create sprite from NULL texture.");
          #endif
          return NULL;
     }
     Sprite* tempOut = memAlloc(sizeof(Sprite));
     if(!tempOut) {
          #if FL_DEBUG_ERROR != 0
          debugError("Couldn't create sprite, out of memory.");
          #endif
          return NULL;
     }

     tempOut->spriteTex = inTex;
     tempOut->spriteX = 0;
     tempOut->spriteY = 0;
     tempOut->spriteWidth = inFrameWidth;
     tempOut->spriteHeight = inFrameHeight;
     tempOut->spriteAngle = 0.0f;
     tempOut->spriteScaleX = 1.0f;
     tempOut->spriteScaleY = 1.0f;
     tempOut->spriteFrame = 0;
     tempOut->spriteFrameCount = ((inTex->texWidth / inFrameWidth) * (inTex->texHeight / inFrameHeight));
     
     return tempOut;
}

void spriteFree(Sprite* inSprite) {
     if(!inSprite) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to free a NULL sprite.");
          #endif
          return;
     }
     memFree(inSprite);
}

void spriteRotate(Sprite* inSprite, float inAngle) {
     inSprite->spriteAngle += inAngle;
}

void spriteResize(Sprite* inSprite, float inWidth, float inHeight) {
     inSprite->spriteScaleX = (inWidth / inSprite->spriteWidth);
     inSprite->spriteScaleY = (inHeight / inSprite->spriteHeight);
}

void spriteMove(Sprite* inSprite, int inX, int inY) {
     inSprite->spriteX += inX;
     inSprite->spriteY += inY;
}

void spriteMoveTo(Sprite* inSprite, int inX, int inY) {
     inSprite->spriteX = inX;
     inSprite->spriteY = inY;
}

void spriteDraw2d(Sprite* inSprite) {
     spriteDraw2dFrame(inSprite, inSprite->spriteFrame);
     inSprite->spriteFrame++;
     while(inSprite->spriteFrame >= inSprite->spriteFrameCount)
          inSprite->spriteFrame -= inSprite->spriteFrameCount;
     while(inSprite->spriteFrame < 0)
          inSprite->spriteFrame += inSprite->spriteFrameCount;
}

void spriteDraw2dFrame(Sprite* inSprite, int inFrame) {
     while(inFrame >= inSprite->spriteFrameCount)
          inFrame -= inSprite->spriteFrameCount;
     while(inFrame < 0)
          inFrame += inSprite->spriteFrameCount;
     
     int tempFrameX = (inFrame * inSprite->spriteWidth);
     int tempFrameY = 0;
     while(tempFrameX >= inSprite->spriteTex->texWidth) {
          tempFrameX -= (inSprite->spriteTex->texWidth / inSprite->spriteWidth);
          tempFrameY += inSprite->spriteHeight;
     }
     
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDisable(GU_CULL_FACE);
     
     texBind(inSprite->spriteTex);
     sceGuTexFilter(GU_LINEAR, GU_LINEAR);
     sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
    
     vertTsVf* tempVerts = (vertTsVf*)sceGuGetMemory(sizeof(vertTsVf) << 2);
     
     float tempWidth = inSprite->spriteWidth * inSprite->spriteScaleX;
     float tempHeight = inSprite->spriteHeight * inSprite->spriteScaleY;
     float tempRad = mathPythag2f(((float)tempWidth * 0.5f), ((float)tempHeight * 0.5f));
     float tempAngle = mathATanf(tempWidth / tempHeight);
     
     tempVerts[0].vertX = inSprite->spriteX + (tempRad * mathSinf(inSprite->spriteAngle - tempAngle));
     tempVerts[0].vertY = inSprite->spriteY - (tempRad * mathCosf(inSprite->spriteAngle - tempAngle));
     tempVerts[1].vertX = inSprite->spriteX + (tempRad * mathSinf(inSprite->spriteAngle + tempAngle));
     tempVerts[1].vertY = inSprite->spriteY - (tempRad * mathCosf(inSprite->spriteAngle + tempAngle));
     tempVerts[2].vertX = inSprite->spriteX + (tempRad * mathSinf(inSprite->spriteAngle + MATH_PI - tempAngle));
     tempVerts[2].vertY = inSprite->spriteY - (tempRad * mathCosf(inSprite->spriteAngle + MATH_PI - tempAngle));
     tempVerts[3].vertX = inSprite->spriteX + (tempRad * mathSinf(inSprite->spriteAngle + MATH_PI + tempAngle));
     tempVerts[3].vertY = inSprite->spriteY - (tempRad * mathCosf(inSprite->spriteAngle + MATH_PI + tempAngle));
     
     tempVerts[0].vertZ = 0.0f;
     tempVerts[0].vertU = tempFrameX;
     tempVerts[0].vertV = tempFrameY;
     tempVerts[1].vertZ = 0.0f;
     tempVerts[1].vertU = (tempFrameX + inSprite->spriteWidth);
     tempVerts[1].vertV = tempFrameY;
     tempVerts[2].vertZ = 0.0f;
     tempVerts[2].vertU = (tempFrameX + inSprite->spriteWidth);
     tempVerts[2].vertV = (tempFrameY + inSprite->spriteHeight);
     tempVerts[3].vertZ = 0.0f;
     tempVerts[3].vertU = tempFrameX;
     tempVerts[3].vertV = (tempFrameY + inSprite->spriteHeight);
    
     sceGuDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_16BIT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 4, 0, tempVerts);
    
     sceGuEnable(GU_CULL_FACE);
     sceGuEnable(GU_DEPTH_TEST);
}

#endif
