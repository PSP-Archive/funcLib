#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>

PSP_MODULE_INFO("flSphere", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_FUNCLIB);

Texture* sphereTex  = NULL;
Camera*  gameCamera = NULL;

int main() {
     flInitialize();

     gameCamera = camCreate();
     sphereTex = texLoad("./earth.tga");
     texMoveVMem(sphereTex);

     vect3f tempPos = { 0.0f, 2.5f, 0.0f };
     vect3f tempRot = { 0.0f, 0.0f, 0.0f };
     vect3f tempScale = { 1.0f, 1.0f, 1.0f };
     while(flRunning) {
          camView(gameCamera);

          sceGumLoadIdentity();
          sceGuAmbientColor(0xFFFFFFFF);
          graphics3dDrawOrigin();
          
          sceGumTranslate(&tempPos);
          sceGumRotateXYZ(&tempRot);
          sceGumScale(&tempScale);

          sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
          sceGuTexEnvColor(0x00FFFF00);
          sceGuTexScale(1.0f, 1.0f);
          sceGuTexOffset(0.0f, 0.0f);
          
          //graphics2dDrawTextureSection(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, sphereTex, 0, 0, sphereTex->texWidth, sphereTex->texHeight);
          graphics3dDrawSphereTextured(64, SPHERE_MAP_CYLINDER, sphereTex);

          graphicsSwapBuffers();
          
          inputPoll();

          if(inputDown(INPUT_DPAD_LEFT))
               tempRot.z -= (MATH_PI / 180.0f);
          if(inputDown(INPUT_DPAD_RIGHT))
               tempRot.z += (MATH_PI / 180.0f);
               
          if(inputDown(INPUT_DPAD_UP))
               tempRot.x -= (MATH_PI / 180.0f);
          if(inputDown(INPUT_DPAD_DOWN))
               tempRot.x += (MATH_PI / 180.0f);
               
          if(inputDown(INPUT_BUTTON_TRIANGLE)) {
               tempScale.x -= (1.0f / 256.0f);
               tempScale.y -= (1.0f / 256.0f);
               tempScale.z -= (1.0f / 256.0f);     
          }
          if(inputDown(INPUT_BUTTON_CROSS)) {
               tempScale.x += (1.0f / 256.0f);
               tempScale.y += (1.0f / 256.0f);
               tempScale.z += (1.0f / 256.0f);
          }

          if(inputCheck(INPUT_BUTTON_RTRIGGER) == INPUT_JUST_DOWN)
               ntfyShowAll(1e+32f);
          if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN)
               graphicsFPSShow = !graphicsFPSShow;
          if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN)
               flTerminate();
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
               debugScreenshot();
     }

     
     flTerminate();
     return 0;
}
