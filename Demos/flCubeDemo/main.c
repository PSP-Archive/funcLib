#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>

PSP_MODULE_INFO("flCubeDemo", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_FUNCLIB);

Texture* cubeTex = NULL;

int main() {
     flInitialize();

     cubeTex = texLoad("./funcLib.tga");
     texMoveVMem(cubeTex);
     texMipMap(cubeTex, 5);
     texSwizzle(cubeTex);

     bool tempWire = false;
     vect3f tempPos = { 0, 0, -2.5f };
     vect3f tempRot = { 0, 0, 0 };
     while(flRunning) {
          sceGumLoadIdentity();
          sceGuAmbientColor(0xFFFFFFFF);
          graphics3dDrawOrigin();
          
          sceGumTranslate(&tempPos);
          sceGumRotateXYZ(&tempRot);
          if(!tempWire) {
               sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
               sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR, GU_LINEAR_MIPMAP_LINEAR);
               sceGuTexScale(1.0f, 1.0f);
               sceGuTexOffset(0.0f, 0.0f);
               graphics3dDrawCubeTextured(cubeTex);
          } else {
               graphics3dDrawCubeWireframe(0xFFFFFFFF);
          }
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
               tempWire = !tempWire;
     }

     
     flTerminate();
     return 0;
}
