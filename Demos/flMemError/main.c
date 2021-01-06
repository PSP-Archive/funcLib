#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspkernel.h>

PSP_MODULE_INFO("flMemError", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_FUNCLIB);

int main() {
     flInitialize();

     char* tempData = memAlloc(512);
     memClear((char*)((u32)tempData - 16), 544);

     while(flRunning) {
          fontDraw2dString(2, 2, fontDefault, "Error should've shown by now!");
          graphicsSwapBuffers();
          
          inputPoll();
          if(inputDown(INPUT_BUTTON_START))
               flTerminate();
     }

     
     flTerminate();
     return 0;
}
