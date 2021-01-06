#include "game.h"
#include "././Code/flGraphicsSimple.h"
#include "././Code/flGraphicsSimpleSprite.h"
#include "././Code/flGraphicsSimpleFont.h"
#include "././Code/flInput.h"
#include <pspdisplay.h>
#include <stdio.h>
#include "player.h"
#include "main.h"

Image*  gameHudPanel = NULL;
Image*  gameHudLogo  = NULL;
Image*  gameHudLogoIcon = NULL;
Sprite* gameHudIcons = NULL;
Font*   gameFont     = NULL;

Player* gamePlayer = NULL;

void gameInitialize() {
     gameHudPanel = imageLoadPNG("./sprites/battleTraxSidebar.png");
     gameHudLogo = imageLoadPNG("./sprites/battleTraxLogo.png");
     gameHudLogoIcon = imageLoadPNG("./sprites/battleTraxIcon.png");
     gameHudIcons = spriteLoadAuto("./sprites/tankHudIcons.png", 16, 16);
     gameFont = fontLoad("./sprites/battleTraxFont.png", -1, 1, COLOR_WHITE);
     gamePlayer = playerCreate();
}

void gameLoop() {
     while(gameState == GAMESTATE_GAME) {
          screenClear(COLOR_BLACK);
          
          inputPoll();
          
          playerInput(gamePlayer);
          playerDraw(gamePlayer, ((SCREEN_WIDTH / 2) + 48), ((SCREEN_HEIGHT / 2) - 16));
          
          gameRenderHud();
          
          sceDisplayWaitVblankStart();
          graphicsSwapBuffers();
     }
}

void gameTerminate() {
     imageFree(gameHudPanel);
     imageFree(gameHudLogo);
     spriteFree(gameHudIcons);
     fontFree(gameFont);
     playerFree(gamePlayer);
}

void gameRenderHud() {
     imageDraw(0, 0, gameHudLogoIcon);
     imageDraw(32, 0, gameHudLogo);
     imageDraw(96, 0, gameHudLogoIcon);
     imageDraw(0, 32, gameHudPanel);
     fontSetColor(COLOR_WHITE);
     char tempString[32];
     spriteDrawFrame(gameHudIcons, 8, 40, 2);
     sprintf(tempString, "%c %i", ASCII_MULTIPLY, gamePlayer->playerLives);
     fontPrint(32, 44, tempString, gameFont);
     spriteDrawFrame(gameHudIcons, 8, 64, 1);
     if(gamePlayer->playerTank->tankHull->tankHullType != TANK_HULLTYPE_NONE) 
          sprintf(tempString, "%i/%i", gamePlayer->playerTank->tankHull->tankHullCur, gamePlayer->playerTank->tankHull->tankHullMax);
     else
          sprintf(tempString, "None");
     fontPrint(32, 68, tempString, gameFont);
     spriteDrawFrame(gameHudIcons, 8, 88, 0);
     if(gamePlayer->playerTank->tankArmor->tankArmorType != TANK_ARMORTYPE_NONE) 
          sprintf(tempString, "%i/%i", gamePlayer->playerTank->tankArmor->tankArmorCur, gamePlayer->playerTank->tankArmor->tankArmorMax);
     else
          sprintf(tempString, "None");
     fontPrint(32, 92, tempString, gameFont);
     spriteDrawFrame(gameHudIcons, 8, 112, 3);
     sprintf(tempString, "%c %i", ASCII_MULTIPLY, gamePlayer->playerTank->tankTurret[0]->tankTurretAmmo);
     fontPrint(32, 116, tempString, gameFont);
     spriteDrawFrame(gameHudIcons, 8, 136, 4);
     sprintf(tempString, "%c %i", ASCII_MULTIPLY, gamePlayer->playerTank->tankTurret[1]->tankTurretAmmo);
     fontPrint(32, 140, tempString, gameFont);
     spriteDrawFrame(gameHudIcons, 8, 160, 5);
     sprintf(tempString, "%c %i", ASCII_MULTIPLY, gamePlayer->playerTank->tankTurret[2]->tankTurretAmmo);
     fontPrint(32, 164, tempString, gameFont);
}
