#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>

PSP_MODULE_INFO("flTest3D", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);


vertTfCVf __attribute__((aligned(16))) cubeVerts[36] = {
	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
	{1, 0, 0xff7f0000,-1, 1, 1}, // 4
	{1, 1, 0xff7f0000, 1, 1, 1}, // 5

	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
	{1, 1, 0xff7f0000, 1, 1, 1}, // 5
	{0, 1, 0xff7f0000, 1,-1, 1}, // 1

	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
	{1, 0, 0xff7f0000, 1,-1,-1}, // 2
	{1, 1, 0xff7f0000, 1, 1,-1}, // 6

	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
	{1, 1, 0xff7f0000, 1, 1,-1}, // 6
	{0, 1, 0xff7f0000,-1, 1,-1}, // 7

	{0, 0, 0xff007f00, 1,-1,-1}, // 0
	{1, 0, 0xff007f00, 1,-1, 1}, // 3
	{1, 1, 0xff007f00, 1, 1, 1}, // 7

	{0, 0, 0xff007f00, 1,-1,-1}, // 0
	{1, 1, 0xff007f00, 1, 1, 1}, // 7
	{0, 1, 0xff007f00, 1, 1,-1}, // 4

	{0, 0, 0xff007f00,-1,-1,-1}, // 0
	{1, 0, 0xff007f00,-1, 1,-1}, // 3
	{1, 1, 0xff007f00,-1, 1, 1}, // 7

	{0, 0, 0xff007f00,-1,-1,-1}, // 0
	{1, 1, 0xff007f00,-1, 1, 1}, // 7
	{0, 1, 0xff007f00,-1,-1, 1}, // 4

	{0, 0, 0xff00007f,-1, 1,-1}, // 0
	{1, 0, 0xff00007f, 1, 1,-1}, // 1
	{1, 1, 0xff00007f, 1, 1, 1}, // 2

	{0, 0, 0xff00007f,-1, 1,-1}, // 0
	{1, 1, 0xff00007f, 1, 1, 1}, // 2
	{0, 1, 0xff00007f,-1, 1, 1}, // 3

	{0, 0, 0xff00007f,-1,-1,-1}, // 4
	{1, 0, 0xff00007f,-1,-1, 1}, // 7
	{1, 1, 0xff00007f, 1,-1, 1}, // 6

	{0, 0, 0xff00007f,-1,-1,-1}, // 4
	{1, 1, 0xff00007f, 1,-1, 1}, // 6
	{0, 1, 0xff00007f, 1,-1,-1}, // 5
};

Texture* cubeTex = NULL;

int main() {
     flInitialize();

     cubeTex = texLoad("./funcLib.tga");


     int tempVal = 0;
     while(flRunning) {

          sceGumMatrixMode(GU_MODEL);
          sceGumLoadIdentity();
          vect3f tempPos = { 0, 0, -2.5f };
          vect3f tempRot = { tempVal * 0.79f * (GU_PI / 180.0f), tempVal * 0.98f * (GU_PI / 180.0f), tempVal * 1.32f * (GU_PI / 180.0f) };
          sceGumTranslate(&tempPos);
          sceGumRotateXYZ(&tempRot);

          texBind(cubeTex);
          sceGuTexFunc(GU_TFX_ADD, GU_TCC_RGB);
          sceGuTexEnvColor(0x00FFFF00);
          sceGuTexFilter(GU_LINEAR, GU_LINEAR);
          sceGuTexScale(1.0f, 1.0f);
          sceGuTexOffset(0.0f, 0.0f);
          sceGuAmbientColor(0xFFFFFFFF);
          sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 36, 0, cubeVerts);
         
          inputPoll();
          if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN)
               flTerminate();
          if(inputDown(INPUT_BUTTON_CROSS))
               tempVal++;
          if(inputDown(INPUT_BUTTON_CIRCLE))
               tempVal--;

          graphicsSwapBuffers();
     }

     
     flTerminate();
     return 0;
}
