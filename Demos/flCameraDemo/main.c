#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>

PSP_MODULE_INFO("flCameraDemo", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

Model3dStatic*   modelTest  = NULL;
Camera*          gameCamera = NULL;

int main() {
     flInitialize();

     modelTest = mdl3dStatLoad("./flatFighter.obj");
     gameCamera = camCreate();

     vect3f tempMdlPos = { 0.0f, 0.0f, 0.0f };
     vect3f tempMdlRot = { 0.0f, 0.0f, 0.0f };
     vect3f tempMdlScale = { 1.0f, 1.0f, 1.0f };
     
     while(flRunning) {
 
          camView(gameCamera);

          graphics3dDrawOrigin();
          
          sceGumLoadIdentity();
          sceGuAmbientColor(0xFFFFFFFF);
          sceGumTranslate(&tempMdlPos);
          sceGumRotateXYZ(&tempMdlRot);
          sceGumScale(&tempMdlScale);

          sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
          sceGuTexScale(1.0f, 1.0f);
          sceGuTexOffset(0.0f, 0.0f);

          /*sceGuColor(0xFF7F7F7F);
          graphicsWireframe = false;
          mdl3dStatDraw(modelTest);
          sceGuColor(0xFFFFFFFF);
          graphicsWireframe = true;*/
          mdl3dStatDraw(modelTest);
         
          graphicsSwapBuffers();
          
          inputPoll();
          
          if(inputDown(INPUT_DPAD_UP))
               camMoveForward(gameCamera, 0.125f);
          if(inputDown(INPUT_DPAD_DOWN))
               camMoveForward(gameCamera, -0.125f);
          if(inputDown(INPUT_DPAD_RIGHT))
               camMoveStrafe(gameCamera, 0.125f);
          if(inputDown(INPUT_DPAD_LEFT))
               camMoveStrafe(gameCamera, -0.125f);

          if(inputDown(INPUT_BUTTON_TRIANGLE))
               camRotateYaw(gameCamera, (MATH_PI / 180.0f));
          if(inputDown(INPUT_BUTTON_CROSS))
               camRotateYaw(gameCamera, -(MATH_PI / 180.0f));
          if(inputDown(INPUT_BUTTON_CIRCLE))
               camRotatePitch(gameCamera, (MATH_PI / 180.0f));
          if(inputDown(INPUT_BUTTON_SQUARE))
               camRotatePitch(gameCamera, -(MATH_PI / 180.0f));

          /*if(inputDown(INPUT_BUTTON_RTRIGGER))
               camRotateRoll(gameCamera, (MATH_PI / 180.0f));
          if(inputDown(INPUT_BUTTON_LTRIGGER))
               camRotateRoll(gameCamera, -(MATH_PI / 180.0f));*/

          if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN)
               graphicsFPSShow = !graphicsFPSShow;
          if(inputCheck(INPUT_BUTTON_RTRIGGER) == INPUT_JUST_DOWN)
               mdl3dStatSave("./flatFighterOpt.obj", modelTest);
          
          if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN)
               flTerminate();
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
               debugScreenshot();
     }

     
     flTerminate();
     return 0;
}
