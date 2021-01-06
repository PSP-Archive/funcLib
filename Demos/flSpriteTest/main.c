#include "././Code/funcLib.h"
#include "ship.h"

PSP_MODULE_INFO("flSpriteTest", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

Font*    gameFont     = NULL;
Timer*   gameTimer    = NULL;

Ship*    gameShip     = NULL;
Texture* shipTex      = NULL;
float    gameTickRate = 0.05f;
float    gameTick     = 0.0f;

int main() {
     flInitialize();
     
     gameFont = fontCreateDebug(0, 0, 0xFF00FF00);
     gameTimer = timerCreate();
     
     Texture* shipTex = texLoad("./fighter64x80.tga");
     Sprite* tempShipSprite = spriteCreate(shipTex, shipTex->texWidth, shipTex->texHeight);
     gameShip = shipCreate(tempShipSprite, (SCREEN_WIDTH >> 1), (SCREEN_HEIGHT >> 1));

     while(flRunning) {

          graphics2dDrawRectFilled(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFF7F0000);
          //graphics2dDrawTexture(0, 0, shipTex->texWidth, shipTex->texHeight, shipTex);

          graphicsBeginOrtho();
          shipDraw(gameShip);
          graphicsEndOrtho();

          graphicsSwapBuffers();

          inputPoll();
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
               debugScreenshot();

          gameTick += timerGetDeltaTime(gameTimer);
          while(gameTick > gameTickRate) {
               gameTick -= gameTickRate;
               if(inputDown(INPUT_BUTTON_CROSS))
                    shipThrust(gameShip);
               if(inputDown(INPUT_DPAD_LEFT))
                    shipRotate(gameShip, -1.0f);
               if(inputDown(INPUT_DPAD_RIGHT))
                    shipRotate(gameShip, 1.0f);
                    
               shipTick(gameShip);
          }
     }

     
     flTerminate();
     return 0;
}
