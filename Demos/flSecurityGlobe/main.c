#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>
#include <psppower.h>
#include <string.h>
#include <stdio.h>

PSP_MODULE_INFO("flSecurityGlobe", PSP_MODULE_USER, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_FUNCLIB);

Texture* earthTex  = NULL;
Camera*  gameCamera = NULL;
Timer*   gameTimer = NULL;
Font*    gameFont = NULL;

float gameTick = 0.0f;
float gameTickCount = 0.0f;
float gameTickRate = (1.0f / 50.0f);

int main() {
     flInitialize();

     earthTex = texLoad("pbp:/earth.tga");
     gameCamera = camCreate();
     gameTimer = timerCreate();
     gameFont = fontLoad("pbp:/font.tga");
     gameFont->fontHSpace = 1;
     gameFont->fontVSpace = 1;
     //ntfyShowAll(1e+32f);
     
     vect3f tempPos = { 0.0f, 1.5f, 0.0f };
     vect3f tempRot = { 0.0f, 0.0f, 0.0f };
     vect3f tempScale = { 1.0f, 1.0f, 1.0f };
     vect3f tempWireScale = { 1.00625f, 1.00625f, 1.00625f };
     vect2f tempTexOffset = { 0.0f, 0.0f };
     vect3f tempGlobeRot = { 0.0f, 0.0f, (MATH_PI * 0.5f) };
     
     while(flRunning) {
          camView(gameCamera);

          sceGumLoadIdentity();
          sceGuAmbientColor(0xFFFFFFFF);
          sceGuTexEnvColor(0xFFFFFFFF);
          
          sceGumTranslate(&tempPos);
          sceGumScale(&tempScale);

          sceGuDisable(GU_CULL_FACE);

          sceGumPushMatrix();
          sceGuTexScale(1.0f, 1.0f);
          sceGuTexOffset(tempTexOffset.x, tempTexOffset.y);
          sceGumRotateXYZ(&tempGlobeRot);
          graphics3dDrawSphereTextured(32, SPHERE_MAP_CYLINDER, earthTex);
          sceGumPopMatrix();
          
          sceGumPushMatrix();
          sceGumScale(&tempWireScale);
          sceGumRotateXYZ(&tempRot);
          sceGuColor(0xFF00FF00);
          graphics3dDrawSphereWireframe(32);
          sceGuColor(0xFFFFFFFF);
          sceGumPopMatrix();

          sceGumLoadIdentity();
          sceGumTranslate(&tempPos);
          sceGumScale(&tempScale);
         
          sceGuColor(0x7F000000);
          sceGuDisable(GU_TEXTURE_2D);
          vertVf* tempVerts = sceGuGetMemory(sizeof(vertVf) << 2);
          tempVerts[0].vertX = -1.0f; tempVerts[0].vertY = 0.0f; tempVerts[0].vertZ = 1.0f;
          tempVerts[1].vertX = 1.0f;  tempVerts[1].vertY = 0.0f; tempVerts[1].vertZ = 1.0f;
          tempVerts[2].vertX = 1.0f;  tempVerts[2].vertY = 0.0f; tempVerts[2].vertZ = -1.0f;
          tempVerts[3].vertX = -1.0f; tempVerts[3].vertY = 0.0f; tempVerts[3].vertZ = -1.0f;
          sceGumDrawArray(GU_TRIANGLE_FAN, GU_VERTEX_32BITF | GU_TRANSFORM_3D, 4, 0, tempVerts);
          sceGuEnable(GU_TEXTURE_2D);          
          sceGuColor(0xFFFFFFFF);

          sceGuEnable(GU_CULL_FACE);

          graphicsSwapBuffers();
          
          gameTick = timerGetDeltaTime(gameTimer);

          tempRot.z += (gameTick * MATH_PI * 0.5f);
          tempRot.z = mathFixRadRange(tempRot.z);
          tempTexOffset.x -= (gameTick * 0.25f);
          if(tempTexOffset.x < 0.0f)
               tempTexOffset.x += 1.0f;
          
          gameTickCount += gameTick;
          while(gameTickCount >= gameTickRate) {
               gameTickCount -= gameTickRate;
          
               inputPoll();
                              
               if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN)
                    graphicsFPSShow = !graphicsFPSShow;
               if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN)
                    flTerminate();
               if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
                    debugScreenshot();
          }
     }

     
     flTerminate();
     return 0;
}
