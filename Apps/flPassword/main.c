#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>
#include <psppower.h>
#include <string.h>
#include <stdio.h>

PSP_MODULE_INFO("flPassword", PSP_MODULE_USER, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_FUNCLIB);

Texture* earthTex  = NULL;
Camera*  gameCamera = NULL;
Timer*   gameTimer = NULL;
Font*    gameFont = NULL;

float gameTick = 0.0f;
float gameTickCount = 0.0f;
float gameTickRate = (1.0f / 50.0f);

char  passString[256];
char  passCorrect[256];
u32   passAttempts = 3;
bool  passNew = false;

int main() {
     flInitialize();

     earthTex = texLoad("pbp:/earth.tga");
     gameCamera = camCreate();
     gameTimer = timerCreate();
     gameFont = fontLoad("pbp:/font.tga");
     gameFont->fontHSpace = 1;
     gameFont->fontVSpace = 1;
     ntfyShowAll(1e+32f);
     
     
     File* tempFile = fileOpen("pbp:/pass.txt", FILE_MODE_READ);
     if(!tempFile)
          debugCriticalError("Couldn't open 'pbp:/pass.txt', EBOOT.PBP must be corrupt.");
     fileRead(passCorrect, 256, tempFile);
     fileClose(tempFile);
     if(!passCorrect[0])
          passNew = true;

     vect3f tempPos = { 0.0f, 1.5f, 0.0f };
     vect3f tempRot = { 0.0f, 0.0f, 0.0f };
     vect3f tempScale = { 1.0f, 1.0f, 1.0f };
     vect3f tempWireScale = { 1.00625f, 1.00625f, 1.00625f };
     char tempString[256];
     
     while(flRunning) {
          camView(gameCamera);

          sceGumLoadIdentity();
          sceGuAmbientColor(0xFFFFFFFF);
          graphics3dDrawOrigin();
          
          sceGumTranslate(&tempPos);
          sceGumRotateXYZ(&tempRot);
          sceGumScale(&tempScale);
          
          sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
          sceGuTexScale(1.0f, 1.0f);
          sceGuTexOffset(0.0f, 0.0f);
          graphics3dDrawSphereTextured(32, SPHERE_MAP_CYLINDER, earthTex);
          
          sceGumPushMatrix();
          sceGumScale(&tempWireScale);
          //graphicsSetLinesAntiAlias(true);
          sceGuColor(0xFF00FF00);
          graphics3dDrawSphereWireframe(32);
          sceGuColor(0xFFFFFFFF);
          //graphicsSetLinesAntiAlias(false);
          sceGumPopMatrix();

          fontDraw2dString(4, 4, gameFont, "flPassword - v1.0.0");
          
          fontDraw2dStringCenter((((SCREEN_HEIGHT - fontHeight(gameFont)) >> 1) - fontHeight(gameFont)), gameFont, passString);
          fontDraw2dStringCenter((((SCREEN_HEIGHT - fontHeight(gameFont)) >> 1) + (fontHeight(gameFont) << 1)), gameFont, "Press <START> to confirm.");
          if(passNew) {
               fontDraw2dStringCenter((((SCREEN_HEIGHT - fontHeight(gameFont)) >> 1) + fontHeight(gameFont)), gameFont, "Type in your new password using the DPAD and BUTTONS.");
               fontDraw2dStringCenter((((SCREEN_HEIGHT - fontHeight(gameFont)) >> 1) + (fontHeight(gameFont) << 1)), gameFont, "Try to remember the combination rather than the number.");
          } else {
               fontDraw2dStringCenter((((SCREEN_HEIGHT - fontHeight(gameFont)) >> 1) + fontHeight(gameFont)), gameFont, "Type in your password using the DPAD and BUTTONS.");
               sprintf(tempString, "You have %u more attempts.", (unsigned int)passAttempts);
               fontDraw2dStringCenter((((SCREEN_HEIGHT - fontHeight(gameFont)) >> 1) + (fontHeight(gameFont) << 2)), gameFont, tempString);
          }

          graphicsSwapBuffers();
          
          gameTick = timerGetDeltaTime(gameTimer);

          tempRot.z += (gameTick * MATH_PI * 0.5f);
          tempRot.z = mathFixRadRange(tempRot.z);
          
          gameTickCount += gameTick;
          while(gameTickCount >= gameTickRate) {
               gameTickCount -= gameTickRate;
          
               inputPoll();

               if(inputCheck(INPUT_DPAD_UP) == INPUT_JUST_DOWN)
                    strcat(passString, "0");
               if(inputCheck(INPUT_DPAD_RIGHT) == INPUT_JUST_DOWN)
                    strcat(passString, "1");
               if(inputCheck(INPUT_DPAD_DOWN) == INPUT_JUST_DOWN)
                    strcat(passString, "2");
               if(inputCheck(INPUT_DPAD_LEFT) == INPUT_JUST_DOWN)
                    strcat(passString, "3");
               if(inputCheck(INPUT_BUTTON_TRIANGLE) == INPUT_JUST_DOWN)
                    strcat(passString, "4");
               if(inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN)
                    strcat(passString, "5");
               if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN)
                    strcat(passString, "6");
               if(inputCheck(INPUT_BUTTON_SQUARE) == INPUT_JUST_DOWN)
                    strcat(passString, "7");
                              
               if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN) {
                    if(passString[0])
                         passString[strlen(passString) - 1] = 0;
               }
               if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN) {
                    if(passNew) {
                         File* tempFile = fileOpen("pbp:/pass.txt", FILE_MODE_WRITE);
                         if(!tempFile)
                              debugCriticalError("Couldn't open 'pbp:/pass.txt', EBOOT.PBP must be corrupt.");
                         fileWrite(passString, 256, tempFile);
                         fileClose(tempFile);
                         flTerminate();
                    } else if(strcmp(passString, passCorrect)) {
                         passAttempts--;
                         if(passAttempts <= 0)
                              scePowerRequestSuspend();
                         passString[0] = 0;
                    } else
                         flTerminate();
               }
               if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
                    debugScreenshot();
          }
     }

     
     flTerminate();
     return 0;
}
