#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspkernel.h>

PSP_MODULE_INFO("flBorderFont", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

Font* appFont = NULL;
Font* appBorderFont = NULL;

int main() {
     flInitialize();

     appFont = fontCreateDebug(0, 0, 0xFFFFFFFF);
     appBorderFont = fontCreateDebug(0, 0, 0xFF000000);
    
     u32 i;
     while(flRunning) {
 
          graphics2dDrawRectFilled(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFF00FF);

          for(i = 0; i < 256; i++) {
               fontDraw2dChar(1 + ((fontCharWidth(appBorderFont, 'A') + 2) * (i & 15)), 0 + ((fontHeight(appBorderFont) + 2) * ((i >> 4) & 15)), appBorderFont, (char)i);
               fontDraw2dChar(1 + ((fontCharWidth(appBorderFont, 'A') + 2) * (i & 15)), 2 + ((fontHeight(appBorderFont) + 2) * ((i >> 4) & 15)), appBorderFont, (char)i);
               fontDraw2dChar(0 + ((fontCharWidth(appBorderFont, 'A') + 2) * (i & 15)), 1 + ((fontHeight(appBorderFont) + 2) * ((i >> 4) & 15)), appBorderFont, (char)i);
               fontDraw2dChar(2 + ((fontCharWidth(appBorderFont, 'A') + 2) * (i & 15)), 1 + ((fontHeight(appBorderFont) + 2) * ((i >> 4) & 15)), appBorderFont, (char)i);
          }
          for(i = 0; i < 256; i++)
               fontDraw2dChar(1 + ((fontCharWidth(appFont, 'A') + 2) * (i & 15)), 1 + ((fontHeight(appFont) + 2) * ((i >> 4) & 15)), appFont, (char)i);

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
