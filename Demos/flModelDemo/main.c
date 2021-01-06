#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>

PSP_MODULE_INFO("flModelTest", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

Model3dStatic* modelTest = NULL;

int main() {
     flInitialize();

     modelTest = mdl3dStatLoad("./funcLib.obj");

     vect3f tempPos = { 0.0f, 0.0f, -2.5f };
     vect3f tempRot = { 0.0f, 0.0f, 0.0f };
     //vect3f tempScale = { (1.0f / 512.0f), (1.0f / 512.0f), (1.0f / 512.0f) };
     vect3f tempScale = { 1.0f, 1.0f, 1.0f };
     while(flRunning) {
          
          sceGumLoadIdentity();
          sceGuAmbientColor(0xFFFFFFFF);
          graphics3dDrawOrigin();
          sceGumTranslate(&tempPos);
          sceGumRotateXYZ(&tempRot);
          sceGumScale(&tempScale);

          sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
          sceGuTexEnvColor(0x00FFFF00);
          sceGuTexScale(1.0f, 1.0f);
          //sceGuTexScale(0.0125f, 0.0125f);
          sceGuTexOffset(0.0f, 0.0f);
          mdl3dStatDraw(modelTest);
         
          graphicsSwapBuffers();
          
          inputPoll();
          if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN)
               flTerminate();
          if(inputDown(INPUT_DPAD_LEFT))
               tempRot.y -= (MATH_PI / 180.0f);
          if(inputDown(INPUT_DPAD_RIGHT))
               tempRot.y += (MATH_PI / 180.0f);
          if(inputDown(INPUT_DPAD_UP))
               tempRot.x -= (MATH_PI / 180.0f);
          if(inputDown(INPUT_DPAD_DOWN))
               tempRot.x += (MATH_PI / 180.0f);
          if(inputDown(INPUT_BUTTON_SQUARE))
               tempPos.x -= 0.0125;
          if(inputDown(INPUT_BUTTON_CIRCLE))
               tempPos.x += 0.0125;
          if(inputDown(INPUT_BUTTON_TRIANGLE))
               tempPos.z -= 0.0125;
          if(inputDown(INPUT_BUTTON_CROSS))
               tempPos.z += 0.0125;
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
               graphicsWireframe = !graphicsWireframe;
          if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN)
               graphicsFPSShow = !graphicsFPSShow;
          if(inputCheck(INPUT_BUTTON_RTRIGGER) == INPUT_JUST_DOWN)
               mdl3dStatSave("./modelSaveTest.obj", modelTest);
     }

     
     flTerminate();
     return 0;
}
