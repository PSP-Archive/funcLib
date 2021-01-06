#include "menu.h"
#include "././Code/flGraphicsSimple.h"
#include "././Code/flGraphicsSimpleFont.h"
#include "././Code/flInput.h"
#include "././Code/flTimer.h"
#include <pspdisplay.h>
#include "main.h"

void menuMain() {
     Image*  menuPanel = imageLoadPNG("./sprites/battleTraxMenu.png");
     Font*   menuFont = fontLoad("./sprites/battleTraxFont.png", -1, 1, COLOR_WHITE);
     int     menuSel = 0;
     
     while(true) {
          screenClear(COLOR_BLACK);
          
          fontSetColor(COLOR_WHITE);
          imageDrawCenter(menuPanel);
          fontPrintCenter(52, "-- MAIN MENU --", menuFont);
          
          int tempY = 88;
          fontPrintCenter(tempY, "NEW GAME", menuFont);
          tempY += 24;
          fontPrintCenter(tempY, "CONTINUE", menuFont);
          tempY += 24;
          fontPrintCenter(tempY, "OPTIONS", menuFont);
          tempY += 24;
          fontPrintCenter(tempY, "HELP", menuFont);
          tempY += 24;
          fontPrintCenter(tempY, "EXIT", menuFont);
          
          fontPrintCenter(88 + (menuSel * 24), ">          <", menuFont);
          
          inputPoll();
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN) {
               switch(menuSel) {
                    case 0:
                    case 1:
                         gameState = GAMESTATE_GAME;
                         return;
                    case 2:
                         return;
                    case 3:
                         return;
                    case 4:
                         gameState = GAMESTATE_END;
                         return;
               }
          } else if(inputCheck(INPUT_DPAD_DOWN) == INPUT_JUST_DOWN) {
               menuSel++;
               if(menuSel > 4)
                    menuSel = 0;
          } else if(inputCheck(INPUT_DPAD_UP) == INPUT_JUST_DOWN) {
               menuSel--;
               if(menuSel < 0)
                    menuSel = 4;
          }
          
          timerWait(0.05);
          
          sceDisplayWaitVblankStart();
          graphicsSwapBuffers();
     }
}
