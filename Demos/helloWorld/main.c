#include "././Code/funcLib.h"
#include "././Code/flTexture.h"
#include "././Code/flGraphics.h"
#include "././Code/flGraphics2d.h"
#include "././Code/flInput.h"
#include "././Code/flDebug.h"
#include "././Code/flMemory.h"
//#include "././Code/flFont.h"
#include <pspdebug.h>
#include <string.h>

PSP_MODULE_INFO("flHelloWorld", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

int main() {
     flInitialize();
     
     Texture* introTex = texLoad("./funcLib.tga");
     char* tempData = vmemAlloc((introTex->texDataWidth * introTex->texDataHeight * texBPP(introTex)) >> 3);
     memCopy(tempData, introTex->texData, ((introTex->texDataWidth * introTex->texDataHeight * texBPP(introTex)) >> 3));
     memFree(introTex->texData);
     introTex->texData = tempData;
     
     Texture* introTex2 = texLoad("./funcLib.pcx");
     tempData = vmemAlloc((introTex2->texDataWidth * introTex2->texDataHeight * texBPP(introTex2)) >> 3);
     memCopy(tempData, introTex2->texData, ((introTex2->texDataWidth * introTex2->texDataHeight * texBPP(introTex2)) >> 3));
     memFree(introTex2->texData);
     introTex2->texData = tempData;
     
     //Font* introDebugFont = fontCreateDebug(0, 0, 0xFFFFFFFF);
     //Font* introTexFont = fontLoad("./defaultFont.tga");
     
     /*Font* introMarqueeFont = fontCreateDebug(0, 0, 0xFFFFFFFF);
     introMarqueeFont->fontBackColor = 0xFF0000FF;
     int introMarqueeLoc = 0;
     bool introMarqueeRight = true;*/
     
     while(flRunning) {
          pspDebugScreenSetXY(0, 1);
          pspDebugScreenPrintf("Hello funcLib World!");

          graphics2dDrawTexture(0, 24, 64, 64, introTex);
          graphics2dDrawTexture(80, 24, 64, 64, introTex2);
          
          //graphics2dDrawRectFilled(80, 24, 64, 64, 0xFF0000FF);
          //raphics2dDrawRect(80, 24, 64, 64, 0xFFFFFFFF);
          
          //graphics2dDrawEllipseFilled(160, 24, 64, 64, 0xFFFF0000);
          //graphics2dDrawEllipse(160, 24, 64, 64, 0xFFFFFFFF);
          
          graphics2dDrawTextureSection(240, 24, 32, 32, introTex, 16, 16, 32, 32);
          
          //fontDrawString(8, 128, introDebugFont, "This is a debug font test :).");
          
          //graphics2dDrawRectFilled(8, 136, (strlen("This is a texture font test :).") * fontStringWidth(introTexFont, "This is a texture font test :).")), fontHeight(introTexFont), 0xFFFFFFFF);
          //fontDrawString(8, 136, introTexFont, "This is a texture font test :).");
          
          /*if(introMarqueeRight) {
               introMarqueeLoc++;
               if((introMarqueeLoc + fontStringWidth(introMarqueeFont, "FuncLib PWNS!")) > SCREEN_WIDTH) {
                    introMarqueeLoc -= 2;
                    introMarqueeRight = !introMarqueeRight;
               }
          } else {
               introMarqueeLoc--;
               if(introMarqueeLoc < 0) {
                    introMarqueeLoc += 2;
                    introMarqueeRight = !introMarqueeRight;
               }
          }
          introMarqueeFont->fontColor = ~introMarqueeFont->fontColor;
          fontDrawString(introMarqueeLoc, 160, introMarqueeFont, "FuncLib PWNS!");*/
          
          graphicsSwapBuffers();
          
          inputPoll();
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
          }
     }
     
     flTerminate();
     return 0;
}
