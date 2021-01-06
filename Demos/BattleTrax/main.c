#include "main.h"
#include "././Code/flGlobal.h"
#include "./..Code/funcLib.h"
#include "././Code/flTexture.h"
#include "././Code/flGraphics.h"
#include "././Code/flGraphics2d.h"
#include <pspdebug.h>

#include "intro.h"
#include "game.h"
#include "menu.h"

PSP_MODULE_INFO("BattleTrax", 0, 1, 1);

u8 gameState = GAMESTATE_INTRO;

int main() {
     flInitialize();
     
     bool gameStateUndefined;
     while(gameState != GAMESTATE_END) {
          gameStateUndefined = true;
                     
          if(gameState == GAMESTATE_INTRO) {
               introScreen();
               gameStateUndefined = false;
          }
     
          if(gameState == GAMESTATE_GAME) {
               gameInitialize();
               gameLoop();
               gameTerminate();
               gameStateUndefined = false;
          }
          
          if(gameState == GAMESTATE_MAINMENU) {
               menuMain();
               gameStateUndefined = false;
          }
          
          if(gameStateUndefined)
               gameState = GAMESTATE_END;
     }
     
     flTerminate();
     return 0;
}
