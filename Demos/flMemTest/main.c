#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspkernel.h>

PSP_MODULE_INFO("flMemTest", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_FUNCLIB);

int main() {
     flInitialize();

     char tempString[256];
     u32 tempY;
     while(flRunning) {
          memQalloc(mathRandi(16384) + 1);
          memQalloc(mathRandi(256) + 1);
          
          tempY = 12;

          sprintf(tempString, "Scratchpad Used: %s", (memScratchpadUsed ? "true" : "false"));
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;

          tempY += 12;

          sprintf(tempString, "VMem free: %u b", (unsigned int)vmemFreeSpace());
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "VMem largest free block: %u b", (unsigned int)vmemLargestFreeBlock());
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "VMem fragmention: %4.2f", vmemFragmentation());
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "VMem stride usage: %3.2f%%", (vmemStrideUsage() * 100.0f));
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          
          tempY += 12;
          sprintf(tempString, "Mem free: %u b", (unsigned int)memFreeSpace(1));
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;

          graphicsSwapBuffers();
          
          inputPoll();
          if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN)
               flTerminate();
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
               debugScreenshot();
          if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN)
               graphicsFPSShow = !graphicsFPSShow;
     }

     
     flTerminate();
     return 0;
}
