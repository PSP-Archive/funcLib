#include "././Code/funcLib.h"
#include "ship.h"
#include "level.h"
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>

PSP_MODULE_INFO("flCavern", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

Camera* gameCamera = NULL;
Ship*   gameShip = NULL;
Level*  gameLevel = NULL;
Timer*  gameTimer = NULL;

bool    gameViewCockpit = true;

float   gameTick = 0.0f;
float   gameTickRate = (1.0f / 50.0f);

int main() {
     flInitialize();

     gameCamera = camCreate();
     gameTimer = timerCreate();
     Mdl3dStat* tempModel = mdl3dStatLoad("./ship0.obj");
     gameLevel = lvlCreate(8192.0f);
     gameShip = shipCreate(tempModel, gameLevel);
     
     while(flRunning) {
          sceGumLoadIdentity();
          
          if(gameViewCockpit)
               shipCockpitCamUpdate(gameShip, gameCamera);
          else
               shipCamUpdate(gameShip, gameCamera);
          camView(gameCamera);

          lvlDraw(gameLevel, gameCamera);
          if(!gameViewCockpit)
               shipDraw(gameShip);
          
          graphicsSwapBuffers();
          
          gameTick += timerGetDeltaTime(gameTimer);
          while(gameTick >= gameTickRate) {
               gameTick -= gameTickRate;
               inputPoll();
               if(inputDown(INPUT_DPAD_UP))
                    shipAngleV(gameShip, (-MATH_PI / 64.0f));
               if(inputDown(INPUT_DPAD_DOWN))
                    shipAngleV(gameShip, (MATH_PI / 64.0f));
               if(inputDown(INPUT_DPAD_RIGHT))
                    shipAngleH(gameShip, (-MATH_PI / 64.0f));
               if(inputDown(INPUT_DPAD_LEFT))
                    shipAngleH(gameShip, (MATH_PI / 64.0f));
               if(inputDown(INPUT_BUTTON_CROSS))
                    shipThrust(gameShip, (1.0f / 16.0f));

               if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN)
                    graphicsFPSShow = !graphicsFPSShow;
               if(inputCheck(INPUT_BUTTON_RTRIGGER) == INPUT_JUST_DOWN)
                    gameViewCockpit = !gameViewCockpit;
          
               if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN)
                    flTerminate();
               if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
                    debugScreenshot();
                    
               //lvlTick(gameLevel);
               shipTick(gameShip);
          }
     }

     
     flTerminate();
     return 0;
}
