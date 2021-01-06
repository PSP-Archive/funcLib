#include "././Code/funcLib.h"

PSP_MODULE_INFO("flPhysicsDemo", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

Font*    gameFont     = NULL;
Timer*   gameTimer    = NULL;
float    gameTickRate = 0.0125f;
float    gameTick     = 0.0f;

int      gameGroundHeight = (float)(SCREEN_HEIGHT >> 2);
u32      gameGroundColor  = 0xFF007F00;
float    gameThrowAngle   = (MATH_PI * 0.25f);
float    gameThrowPower   = 0.0f;
float    gameGravity      = 0.125f;

typedef struct {
     vect2f  javLoc;
     vect2f  javVel;
     float   javAngle;
     Sprite* javSprite;
} Javelin;

Texture*  gameJavelinTex;
Sprite*   gameJavelinSprite;
Javelin*  gameJavelin;

Javelin* javCreate(float inX, float inY, float inAngle, float inVel) {
     Javelin* tempOut = memAlloc(sizeof(Javelin));
     tempOut->javLoc.x = inX;
     tempOut->javLoc.y = inY;
     tempOut->javAngle = inAngle;
     tempOut->javVel.x = (inVel * mathSinf(inAngle));
     tempOut->javVel.y = (inVel * (0.0f - mathCosf(inAngle)));
     tempOut->javSprite = gameJavelinSprite;
     return tempOut;
}

void javTick(Javelin* inJav) {
     Line2d tempLine;
     tempLine.linePoints[0].x = (inJav->javLoc.x - ((inJav->javSprite->spriteTex->texWidth >> 1) * mathSinf(inJav->javAngle)));
     tempLine.linePoints[1].x = (inJav->javLoc.x + ((inJav->javSprite->spriteTex->texWidth >> 1) * mathSinf(inJav->javAngle)));
     tempLine.linePoints[0].y = (inJav->javLoc.y + ((inJav->javSprite->spriteTex->texWidth >> 1) * mathCosf(inJav->javAngle)));
     tempLine.linePoints[1].y = (inJav->javLoc.y - ((inJav->javSprite->spriteTex->texWidth >> 1) * mathCosf(inJav->javAngle)));
     Line2d tempGndLine;
     tempGndLine.linePoints[0].x = 0.0f;
     tempGndLine.linePoints[1].x = (float)SCREEN_WIDTH;
     tempGndLine.linePoints[0].y = (float)(SCREEN_HEIGHT - gameGroundHeight);
     tempGndLine.linePoints[1].y = (float)(SCREEN_HEIGHT - gameGroundHeight);
     if(!collision2dLineLine(tempLine, tempGndLine)) {
          inJav->javVel.y += gameGravity;
          inJav->javLoc.x += inJav->javVel.x;
          inJav->javLoc.y += inJav->javVel.y;
          if(inJav->javVel.y == 0.0f) {
               inJav->javAngle = (mathSignf(inJav->javVel.x) * (MATH_PI * 0.5f));
          } else {
               if(inJav->javVel.y > 0.0f)
                    inJav->javAngle = ((MATH_PI * 0.5f) - mathATanf(inJav->javVel.x / inJav->javVel.y));
               else
                    inJav->javAngle = (mathATanf(inJav->javVel.x / (0.0f - inJav->javVel.y)) - (MATH_PI * 0.5f));
          }
     }
}

void javDraw(Javelin* inJav) {
     inJav->javSprite->spriteAngle = inJav->javAngle;
     inJav->javSprite->spriteX = inJav->javLoc.x;
     inJav->javSprite->spriteY = inJav->javLoc.y;
     spriteDraw2dFrame(inJav->javSprite, 0);
}

int main() {
     flInitialize();
     
     gameFont = fontCreateDebug(0, 0, 0xFF007F00);
     gameTimer = timerCreate();
     
     gameJavelinTex = texLoad("./javelin.tga");
     gameJavelinSprite = spriteCreate(gameJavelinTex, gameJavelinTex->texWidth, gameJavelinTex->texHeight);
    
     char tempString[256];
     while(flRunning) {

          graphics2dDrawRectFilled(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFFFF7F);
          graphics2dDrawRectFilled(0, (SCREEN_HEIGHT - gameGroundHeight), SCREEN_WIDTH, gameGroundHeight, gameGroundColor);
          
          if(gameJavelin)
               javDraw(gameJavelin);
          
          sprintf(tempString, "Angle: %.2f", mathRadToDeg(gameThrowAngle)); 
          fontDraw2dString(2, 2, gameFont, tempString);
          sprintf(tempString, "Power: %.2f", gameThrowPower); 
          fontDraw2dString(2, 12, gameFont, tempString);

          graphicsSwapBuffers();

          inputPoll();
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
               debugScreenshot();          
          
          gameTick += timerGetDeltaTime(gameTimer);
          while(gameTick > gameTickRate) {
               gameTick -= gameTickRate;
               if(gameJavelin)
                    javTick(gameJavelin);
               if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN) {
                    if(gameJavelin)
                         memFree(gameJavelin);
                    gameJavelin = javCreate(0, (SCREEN_HEIGHT >> 1), gameThrowAngle, gameThrowPower);
               }
               if(inputDown(INPUT_DPAD_LEFT)) {
                    gameThrowPower = max((gameThrowPower - 0.25f), 0);
               } else if(inputDown(INPUT_DPAD_RIGHT)) {
                    gameThrowPower = min((gameThrowPower + 0.25f), 5);
               }
               if(inputDown(INPUT_DPAD_UP)) {
                    gameThrowAngle -= (MATH_PI / 180.0f);
               } else if(inputDown(INPUT_DPAD_DOWN)) {
                    gameThrowAngle += (MATH_PI / 180.0f);
               }
          }
     }

     
     flTerminate();
     return 0;
}
