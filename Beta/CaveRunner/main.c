#include "././Code/funcLib.h"
#include "level.h"
#include "ship.h"
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>

PSP_MODULE_INFO("CaveRunner", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

Timer*  gameTimer = NULL;
float   gameTickRate = (1.0f / 50.0f);
float   gameTick = 0.0f;

Camera* gameCamera = NULL;

Level*  gameLevel = NULL;

Ship*   gameShip = NULL;

int main() {
     flInitialize();

     gameTimer = timerCreate();
     gameCamera = camCreate();
     gameLevel = levelCreate(4096);
     gameShip = shipCreate(gameLevel);
     shipThrust(gameShip, (vect3f){ 0.0f, 0.5f, 0.0f });

     while(flRunning) {
          sceGumLoadIdentity();
          
          shipCamUpdate(gameShip, gameCamera);
          camView(gameCamera);
          levelDraw(gameLevel, gameCamera->camPos, 1024.0f);
          
          graphicsSwapBuffers();
          
          gameTick += timerGetDeltaTime(gameTimer);
          while(gameTick >= gameTickRate) {
               gameTick -= gameTickRate;               
          
               inputPoll();
               if(inputDown(INPUT_BUTTON_CROSS))
                    shipThrust(gameShip, (vect3f){ 0.0f, 0.0f, 0.0625f });
               
               if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN)
                    flTerminate();
               if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
                    debugScreenshot();
               if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN)
                    graphicsFPSShow = !graphicsFPSShow;
                    
               shipTick(gameShip);
          }
     }

     
     flTerminate();
     return 0;
}
