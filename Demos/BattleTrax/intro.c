#include "intro.h"
#include "././Code/flGraphicsSimple.h"
#include "././Code/flGraphicsSimpleSprite.h"
#include "././Code/flTimer.h"
#include <pspdisplay.h>
#include "main.h"

void introScreen() {
     screenClear(COLOR_BLACK);
     sceDisplayWaitVblankStart();
     graphicsSwapBuffers();
     
     Sprite* introLogo = spriteLoadAuto("./sprites/battleTraxLogo.png", 64, 32);
     Sprite* introIcon = spriteLoadAuto("./sprites/battleTraxIcon.png", 32, 32);
     Sprite* introText = spriteLoadAuto("./sprites/battleTraxIntro.png", 128, 16);
     
     int i;
     for(i = 0; i < 136; i++) {
          screenClear(COLOR_BLACK);
          spriteDrawFrame(introIcon, ((SCREEN_WIDTH / 2) - 64), (i - 32), 0);
          spriteDrawFrame(introLogo, ((SCREEN_WIDTH / 2) - 32), (i - 32), 0);
          spriteDrawFrame(introIcon, ((SCREEN_WIDTH / 2) + 32), (i - 32), 0);
          spriteDrawFrame(introText, ((SCREEN_WIDTH / 2) - 64), ((SCREEN_HEIGHT + 16) - i), 0);
          sceDisplayWaitVblankStart();
          graphicsSwapBuffers();
          timerWait((INTRO_LENGTH / 2) / 136);
     }
     timerWait(INTRO_LENGTH / 2);
     
     spriteFree(introLogo);
     spriteFree(introIcon);
     spriteFree(introText);
     
     gameState = GAMESTATE_MAINMENU;
}
