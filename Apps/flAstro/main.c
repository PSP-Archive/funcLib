#include "././Code/funcLib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <psputility_sysparam.h>
#include <pspgu.h>
#include <pspkernel.h>
#include <pspimpose_driver.h>

#include "main.h"
#include "mudkipSprite.h"
//#include "flAstroFont.c"

PSP_MODULE_INFO("flAstro", PSP_MODULE_USER, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_FUNCLIB);

Level gameLevel;

int    shipX, shipY;
int    shipLastX, shipLastY;
u32    shipColor;
u32    shipOutlineColor;
float  shipFuel = 0;
int    shipThrust = 0;
int    shipVelocityX;
int    shipVelocityY;
int    shipHealth = 0;
int    shipMaxHealth = 0;

bool    shipUpthrust = false;
bool    shipLeftthrust = false;
bool    shipRightthrust = false;
bool    shipHyperthrust = false;

float   gameClosestGroundHeight;
float   gameLandingTolerance = 0;
int     gameLives = 0;
u8      gameDifficulty = 0;
int     gameScore = 0;
int     gameHiScore = 0;
char    gameHiScoreNames[40][16];
int     gameHiScoreLevels[40];
int     gameHiScores[40];
float   gameTimeLeft = 0.0f;

Font*   gameFont = NULL;
Timer*  gameTimer = NULL;
float   gameTickRate = 0.05f;
float   gameTickTime = 0.0f;
Timer*  gameLevelTimer = NULL;

//bool    gamePaused = false;
Texture* gameMudkipTex = NULL;
bool     gameMudkipUnlocked = false;
bool     gameMudkipMode = false;
bool     gameUltraUnlocked = false;

//int      testEffect = 0;

void gameStartDialog() {
     inputLock(0.5f);
     char tempString[256];
     float tempTimeLeft = 3.0f;
     Timer* tempTimer = timerCreate();
     while(flRunning) {
          gameRenderLoop();
          
          tempTimeLeft -= timerGetDeltaTime(tempTimer);
          if(tempTimeLeft < 0.0f)
               tempTimeLeft = 0.0f;
          
          sprintf(tempString, "Game Start in %1.0f.", tempTimeLeft);
          fontDraw2dStringCenter(127, gameFont, tempString);
          fontDraw2dStringCenter(137, gameFont, "[X/O] Skip");
          graphicsSwapBuffers();
          
          if(tempTimeLeft <= 0.0f)
               break;

          inputPoll();
          if((inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN) || (inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN))
               break;
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, true);
          }
          if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN) {
               timerFree(tempTimer);
               timerGetDeltaTime(gameTimer);
               gamePauseDialog();
               return;
          }
     }
     timerFree(tempTimer);
     timerGetDeltaTime(gameTimer);
}

bool gameConfirmDialog(char* inMessage, u32 inColor, bool inGame) {
     int tempX, tempY;
     int tempWidth, tempHeight;
     tempWidth = max((fontStringWidth(gameFont, inMessage) + 16), (fontStringWidth(gameFont, "[X]OK   [O]Cancel") + 16));
     tempHeight = ((fontHeight(gameFont) * 3) + 16);
     tempX = (SCREEN_WIDTH - tempWidth) / 2;
     tempY = (SCREEN_HEIGHT - tempHeight) / 2;
     inputLock(0.5f);
     while(flRunning) {
          if(inGame)
               gameRenderLoop();
          else
               levelDraw();
          
          graphics2dDrawRectFilled(tempX, tempY, tempWidth, tempHeight, 0xFF7F7F7F);
          graphics2dDrawRect(tempX, tempY, tempWidth, tempHeight, 0xFFFFFFFF);
          graphics2dDrawRect((tempX + 2), (tempY + 2), (tempWidth - 4), (tempHeight - 4), 0xFFFFFFFF);
          
          gameFont->fontColor = inColor;
          fontDraw2dStringCenter((tempY + 8), gameFont, inMessage);
          fontDraw2dStringCenter((tempY + (2 * fontHeight(gameFont)) + 8), gameFont, "[X]OK   [O]Cancel");
          
          if(!inGame) {
               char tempString[256];
               sprintf(tempString, "HiScore: %i", gameHiScore);
               fontDraw2dString((SCREEN_WIDTH - 130), 2, gameFont, tempString);
          }
          
          graphicsSwapBuffers();

          inputPoll();
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN) {
               //timerGetDeltaTime(gameTimer);
               return true;
          }
          if(inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN) {
               //timerGetDeltaTime(gameTimer);
               return false;
          }
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, inGame);
          }
     }
     //timerGetDeltaTime(gameTimer);
     return false;
}

bool gameSaveDelete(int inSlot) {
     char tempString[256];
     sprintf(tempString, "./saveData%i.dat", inSlot);
     if(!fileExists(tempString))
          return false;
     sprintf(tempString, "Really delete savedata in slot %i.", (inSlot + 1));
     if(!gameConfirmDialog(tempString, 0xFFFFFFFF, false))
          return false;
     sprintf(tempString, "./saveData%i.dat", inSlot);          
     fileDelete(tempString);
     return true;
}

bool gameSave(int inSlot) {
     if(inSlot < 0)
          return false;
     char tempString[256];
     sprintf(tempString, "./saveData%i.dat", inSlot);
     if(fileExists(tempString)) {
          sprintf(tempString, "Really overwrite savedata in slot %i.", (inSlot + 1));
          if(!gameConfirmDialog(tempString, 0xFFFFFFFF, true))
               return false;
     }
     sprintf(tempString, "./saveData%i.dat", inSlot);
     FILE* tempFile = fopen(tempString, "wb");
     if(!tempFile) {
          debugError("Couldn't save game.");
          fclose(tempFile);
          return false;
     }

     int tempStructSize = sizeof(Level);
     fwrite(&tempStructSize, 4, 1, tempFile);

     fwrite(&gameLives, 4, 1, tempFile);
     fwrite(&gameDifficulty, 1, 1, tempFile);
     fwrite(&gameScore, 4, 1, tempFile);


     fwrite(&gameLevel, tempStructSize, 1, tempFile);
     fwrite(gameLevel.lvlPoints, 4, (gameLevel.lvlPointsCount * 2), tempFile);
     fwrite(gameLevel.lvlBackPoints, 4, (gameLevel.lvlPointsCount * 2), tempFile);
     fwrite(gameLevel.lvlStars, 4, (gameLevel.lvlStarsCount * 2), tempFile);

     fwrite(&shipX, 4, 1, tempFile);
     fwrite(&shipY, 4, 1, tempFile);
     fwrite(&shipLastX, 4, 1, tempFile);
     fwrite(&shipLastY, 4, 1, tempFile);
     fwrite(&shipColor, 4, 1, tempFile);
     fwrite(&shipOutlineColor, 4, 1, tempFile);
     fwrite(&shipFuel, sizeof(float), 1, tempFile);
     fwrite(&shipThrust, 4, 1, tempFile);
     fwrite(&shipVelocityX, 4, 1, tempFile);
     fwrite(&shipVelocityY, 4, 1, tempFile);

     fwrite(&shipHealth, 4, 1, tempFile);
     fwrite(&shipMaxHealth, 4, 1, tempFile);

     fwrite(&shipUpthrust, 1, 1, tempFile);
     fwrite(&shipLeftthrust, 1, 1, tempFile);
     fwrite(&shipRightthrust, 1, 1, tempFile);
     fwrite(&shipHyperthrust, 1, 1, tempFile);

     fwrite(&gameClosestGroundHeight, sizeof(float), 1, tempFile);
     fwrite(&gameLandingTolerance, sizeof(float), 1, tempFile);
     fwrite(&gameTimeLeft, sizeof(float), 1, tempFile);
     
     fclose(tempFile);
     
     return true;
}

bool gameLoad(int inSlot) {
     if(inSlot < 0)
          return false;
     char tempString[256];
     sprintf(tempString, "./saveData%i.dat", inSlot);
     FILE* tempFile = fopen(tempString, "rb");
     if(!tempFile) {
          debugError("Couldn't load game.");
          return false;
     }

     int tempStructSize = 0;
     fread(&tempStructSize, 4, 1, tempFile);
     if(tempStructSize != sizeof(Level)) {
          //debugError("Level struct size wrong, level format is outdated.");
          gameDialog("Game save outdated.", 0xFFFFFFFF, false);
          fclose(tempFile);
          gameSaveDelete(inSlot);
          return false;
     }

     fread(&gameLives, 4, 1, tempFile);
     fread(&gameDifficulty, 1, 1, tempFile);
     fread(&gameScore, 4, 1, tempFile);
     
     gameHiScore = gameHiScores[gameDifficulty * 10];

     memFree(gameLevel.lvlPoints);
     memFree(gameLevel.lvlBackPoints);     
     memFree(gameLevel.lvlStars);
     fread(&gameLevel, tempStructSize, 1, tempFile);
     gameLevel.lvlPoints = memAlloc((gameLevel.lvlPointsCount * 2) << 2);
     if(!gameLevel.lvlPoints) {
          debugError("Allocation error while loading level.");
          fclose(tempFile);
          return false;
     }
     fread(gameLevel.lvlPoints, 4, (gameLevel.lvlPointsCount * 2), tempFile);
     gameLevel.lvlBackPoints = memAlloc((gameLevel.lvlPointsCount * 2) << 2);
     if(!gameLevel.lvlBackPoints) {
          memFree(gameLevel.lvlPoints);
          debugError("Allocation error while loading level.");
          fclose(tempFile);
          return false;
     }
     fread(gameLevel.lvlBackPoints, 4, (gameLevel.lvlPointsCount * 2), tempFile);

     gameLevel.lvlStars = memAlloc((gameLevel.lvlStarsCount * 2) << 2);
     if(!gameLevel.lvlStars) {
          memFree(gameLevel.lvlPoints);
          memFree(gameLevel.lvlBackPoints);
          debugError("Allocation error while loading level.");
          fclose(tempFile);
          return false;
     }
     fread(gameLevel.lvlStars, 4, (gameLevel.lvlStarsCount * 2), tempFile);

     fread(&shipX, 4, 1, tempFile);
     fread(&shipY, 4, 1, tempFile);
     fread(&shipLastX, 4, 1, tempFile);
     fread(&shipLastY, 4, 1, tempFile);
     fread(&shipColor, 4, 1, tempFile);
     fread(&shipOutlineColor, 4, 1, tempFile);
     fread(&shipFuel, sizeof(float), 1, tempFile);
     fread(&shipThrust, 4, 1, tempFile);
     fread(&shipVelocityX, 4, 1, tempFile);
     fread(&shipVelocityY, 4, 1, tempFile);

     fread(&shipHealth, 4, 1, tempFile);
     fread(&shipMaxHealth, 4, 1, tempFile);

     fread(&shipUpthrust, 1, 1, tempFile);
     fread(&shipLeftthrust, 1, 1, tempFile);
     fread(&shipRightthrust, 1, 1, tempFile);
     fread(&shipHyperthrust, 1, 1, tempFile);

     fread(&gameClosestGroundHeight, sizeof(float), 1, tempFile);
     fread(&gameLandingTolerance, sizeof(float), 1, tempFile);
     fread(&gameTimeLeft, sizeof(float), 1, tempFile);
     
     if((gameDifficulty == 1) && (gameLevel.lvlNumber > 50) && !gameMudkipUnlocked) {
          gameDialog("Mudkip mode unlocked!", 0xFFFFFFFF, true);
          gameMudkipUnlocked = true;
     }
     if((gameDifficulty == 2) && (gameLevel.lvlNumber > 30) && !gameUltraUnlocked) {
          gameDialog("Ultra difficulty unlocked!", 0xFFFFFFFF, true);
          gameUltraUnlocked = true;
     }
     
     fclose(tempFile);

     return true;
}

void gameHiScoresClear() {
     int i = 0, j = 0;
     for(j = 0; j < 4; j++) {
          for(i = 0; i < 10; i++) {
               gameHiScores[(j * 10) + i] = ((10 - i) * 1000);
               gameHiScoreLevels[(j * 10) + i] = (20 - i);
               if(j == 0)
                    sprintf(gameHiScoreNames[(j * 10) + i], "EasyPlayer%i", (i + 1));
               else if(j == 1)
                    sprintf(gameHiScoreNames[(j * 10) + i], "MediumPlayer%i", (i + 1));
               else if(j == 2)
                    sprintf(gameHiScoreNames[(j * 10) + i], "HardPlayer%i", (i + 1));
               else
                    sprintf(gameHiScoreNames[(j * 10) + i], "UltraPlayer%i", (i + 1));
          }
     }
     gameHiScore = gameHiScores[gameDifficulty * 10];
     gameMudkipUnlocked = false;
     gameMudkipMode = false;
     gameUltraUnlocked = false;
     if(gameDifficulty >= 3)
          gameDifficulty = 2;
     gameHiScoresSave();
}

void gameHiScoresLoad() {
     FILE* tempFile = fopen("./hiScores.dat", "rb");
     if(!tempFile) {
          gameHiScoresClear();
     } else {
          int i = 0;
          for(i = 0; i < 40; i++) {
               fread(&gameHiScoreNames[i], 1, 16, tempFile);
               fread(&gameHiScoreLevels[i], 4, 1, tempFile);
               fread(&gameHiScores[i], 4, 1, tempFile);
          }
          fclose(tempFile);
     }

     if(gameHiScoreLevels[10] >  50)
          gameMudkipUnlocked = true;
     if(gameHiScoreLevels[20] >  30)
          gameUltraUnlocked = true;
     gameHiScore = gameHiScores[gameDifficulty * 10];
}

void gameHiScoresSave() {
     FILE* tempFile = fopen("./hiScores.dat", "wb");
     if(!tempFile) {
          debugError("Couldn't save hi scores.");
          return;
     }
     int i = 0;
     for(i = 0; i < 40; i++) {
          fwrite(&gameHiScoreNames[i], 1, 16, tempFile);
          fwrite(&gameHiScoreLevels[i], 4, 1, tempFile);
          fwrite(&gameHiScores[i], 4, 1, tempFile);
     }
     fclose(tempFile);
}

int gameHiScoresUpdate() {
     int i, j;
     for(i = 0; i < 10; i++) {
          if(gameHiScores[(gameDifficulty * 10) + i] <= gameScore) {
               if(i < 9) {
                    for(j = 9; j > i; j--) {
                         gameHiScores[(gameDifficulty * 10) + j] = gameHiScores[(gameDifficulty * 10) + (j - 1)];
                         gameHiScoreLevels[(gameDifficulty * 10) + j] = gameHiScoreLevels[(gameDifficulty * 10) + (j - 1)];
                         strcpy(gameHiScoreNames[(gameDifficulty * 10) + j], gameHiScoreNames[(gameDifficulty * 10) + (j - 1)]);
                    }
               }
               gameHiScores[(gameDifficulty * 10) + i] = gameScore;
               gameHiScoreLevels[(gameDifficulty * 10) + i] = gameLevel.lvlNumber;
               sceUtilityGetSystemParamString(PSP_SYSTEMPARAM_ID_STRING_NICKNAME, gameHiScoreNames[(gameDifficulty * 10) + i], 16);
               gameHiScoresSave();
               return i;
          }
     }
     return -1;
}

int levelPointsGet(int inX, int inY) {
     if(((inX * 2) + inY) < (gameLevel.lvlPointsCount * 2))
          return gameLevel.lvlPoints[(inX * 2) + inY];
     return gameLevel.lvlPoints[(gameLevel.lvlPointsCount - 2) + inY];
}

void shipDraw() {
     int tempPoints[3][2];
     //if(!gamePaused) {
          tempPoints[0][0] = shipX;
          tempPoints[0][1] = 0;
          tempPoints[1][0] = (shipX - 3);
          tempPoints[1][1] = (shipY + 8);
          tempPoints[2][0] = (shipX + 3);
          tempPoints[2][1] = (shipY + 8);
          int tempRed = mathRandi(128) + 64;
          u32 tempColor = 0xFF000000 + (tempRed + 64) + (mathRandi(tempRed / 2) << 8);
          u32 tempOutlineColor = 0xFF000000 + ((((tempColor & 0x0000FF00) / 3) * 4) & 0x0000FF00) + (((tempColor & 0x000000FF) / 3) * 4);
          if(gameMudkipMode) {
               tempColor = ((tempColor & 0x000000FF) << 16) + (tempColor & 0x0000FF00) + 0xFF000000;
               tempOutlineColor = ((tempOutlineColor & 0x000000FF) << 16) + (tempOutlineColor & 0x0000FF00) + 0xFF000000;
          }
          if(shipUpthrust && (shipFuel >= (((float)shipThrust * SHIP_FUEL_DRAINFACTOR) / 100.0f))) {
               tempPoints[0][1] = (shipY + 16);
               graphics2dDrawTriangleFilled(tempPoints[0][0], tempPoints[0][1], tempPoints[1][0], tempPoints[1][1], tempPoints[2][0], tempPoints[2][1], tempColor);
               graphics2dDrawTriangle(tempPoints[0][0], tempPoints[0][1], tempPoints[1][0], tempPoints[1][1], tempPoints[2][0], tempPoints[2][1], tempOutlineColor);
          } else if(shipHyperthrust && (shipFuel >= (((float)shipThrust * SHIP_FUEL_DRAINFACTOR) / 15.0f))) {
               tempPoints[0][1] = (shipY + 40);
               graphics2dDrawTriangleFilled(tempPoints[0][0], tempPoints[0][1], tempPoints[1][0], tempPoints[1][1], tempPoints[2][0], tempPoints[2][1], tempColor);
               graphics2dDrawTriangle(tempPoints[0][0], tempPoints[0][1], tempPoints[1][0], tempPoints[1][1], tempPoints[2][0], tempPoints[2][1], tempOutlineColor);
          }
     //}
 
     if(!gameMudkipMode) {
          tempPoints[0][0] = shipX;
          tempPoints[0][1] = (shipY - 8);
          tempPoints[1][0] = (shipX - 4);
          tempPoints[1][1] = (shipY + 8);
          tempPoints[2][0] = (shipX + 4);
          tempPoints[2][1] = (shipY + 8);
          //graphics2dDrawTriangleFilled(tempPoints[0][0], tempPoints[0][1], tempPoints[1][0], tempPoints[1][1], tempPoints[2][0], tempPoints[2][1], shipColor);
          graphics2dDrawTriangleFilledGrad(tempPoints[0][0], tempPoints[0][1], colorLumMult8888(shipColor, 0.5f), tempPoints[1][0], tempPoints[1][1], shipColor, tempPoints[2][0], tempPoints[2][1], shipColor);
          graphics2dDrawTriangle(tempPoints[0][0], tempPoints[0][1], tempPoints[1][0], tempPoints[1][1], tempPoints[2][0], tempPoints[2][1], shipOutlineColor);
     } else {
          graphics2dDrawTexture((shipX - (mudkipSpriteWidth / 2)), (shipY - (mudkipSpriteHeight - 8)), mudkipSpriteWidth, mudkipSpriteHeight, gameMudkipTex);
     }
}

void levelDraw() {
     float tempLevelTick = timerGetDeltaTime(gameLevelTimer);
     
     int i;
     //Draw stars
     u32 tempLevelStarsColor = 0x00000000;
     for(i = 0; i < gameLevel.lvlStarsCount; i++) {
          tempLevelStarsColor = (mathRandi(192) + 63) + ((mathRandi(192) + 63) << 8) + ((mathRandi(192) + 63) << 16) + 0xFF000000;
          graphics2dDrawPoint(gameLevel.lvlStars[i * 2], gameLevel.lvlStars[(i * 2) + 1], tempLevelStarsColor);
     }
     // Draw planetoids/suns
     u32 tempColor = 0;
     for(i = 0; i < 4; i++) {
          if(i < gameLevel.lvlSunCount) {
               tempColor = colorBlend8888(gameLevel.lvlSuns[i][3], gameLevel.lvlSuns[i][4], 0.5f);
               graphics2dDrawEllipseFilledGrad((gameLevel.lvlSuns[i][0] - (gameLevel.lvlSuns[i][2] >> 1)), (gameLevel.lvlSuns[i][1] - (gameLevel.lvlSuns[i][2] >> 1)), (gameLevel.lvlSuns[i][2] << 1), (gameLevel.lvlSuns[i][2] << 1), tempColor, (tempColor & 0x00FFFFFF));
               graphics2dDrawEllipseFilledGrad(gameLevel.lvlSuns[i][0], gameLevel.lvlSuns[i][1], gameLevel.lvlSuns[i][2], gameLevel.lvlSuns[i][2], gameLevel.lvlSuns[i][3], gameLevel.lvlSuns[i][4]);
          }
          if(i < gameLevel.lvlPlanetCount) {
               if(gameLevel.lvlPlanets[i].pntAlive) {
                    graphics2dDrawSpheroidShaded(gameLevel.lvlPlanets[i].pntX, gameLevel.lvlPlanets[i].pntY, gameLevel.lvlPlanets[i].pntDiam, gameLevel.lvlPlanets[i].pntDiam, gameLevel.lvlPlanets[i].pntColor, (MATH_PI * 1.5f));
               } else if(gameLevel.lvlPlanets[i].pntExplodeTime > 0.0f) {
                    gameLevel.lvlPlanets[i].pntExplodeTime -= tempLevelTick;
                    if(gameLevel.lvlPlanets[i].pntExplodeTime > 0.0f) {
                         int tempSize = 0;
                         if(gameLevel.lvlPlanets[i].pntExplodeTime > PLANET_EXPLODE_TIME)
                              tempSize = gameLevel.lvlPlanets[i].pntDiam + (int)(((1.0f - ((gameLevel.lvlPlanets[i].pntExplodeTime - PLANET_EXPLODE_TIME) / PLANET_EXPAND_TIME)) * (float)gameLevel.lvlPlanets[i].pntDiam) * 2.0f);
                         else
                              tempSize = (int)((float)gameLevel.lvlPlanets[i].pntDiam * ((gameLevel.lvlPlanets[i].pntExplodeTime / PLANET_EXPLODE_TIME) * 3.0f));
                         int tempX = (gameLevel.lvlPlanets[i].pntX + (gameLevel.lvlPlanets[i].pntDiam / 2)) - (tempSize / 2);
                         int tempY = (gameLevel.lvlPlanets[i].pntY + (gameLevel.lvlPlanets[i].pntDiam / 2)) - (tempSize / 2);
                         graphics2dDrawEllipseFilledGrad(tempX, tempY, tempSize, tempSize, colorRGBA8888((mathRandi(64) + 192), (mathRandi(128) + 64), 0, 255), colorRGBA8888((mathRandi(64) + 192), (mathRandi(128) + 64), 0, 0));
                    }
               }
          }
     }
     
     if(gameLevel.lvlDeathStar.dsActive) {
          int tempLaserOriginX = ((gameLevel.lvlDeathStar.dsX + (SCREEN_WIDTH >> 4)) + (SCREEN_WIDTH >> 5)) - 2;
          int tempLaserOriginY = ((gameLevel.lvlDeathStar.dsY + (SCREEN_WIDTH >> 4)) - (SCREEN_WIDTH >> 6)) - 2;
          graphics2dDrawSpheroidShaded(gameLevel.lvlDeathStar.dsX, gameLevel.lvlDeathStar.dsY, (SCREEN_WIDTH >> 3), (SCREEN_WIDTH >> 3), 0xFF7F7F7F, (MATH_PI * 1.5f));
          graphics2dDrawLineGrad(gameLevel.lvlDeathStar.dsX, (gameLevel.lvlDeathStar.dsY + (SCREEN_WIDTH >> 4) - 1), 0xFF000000, (gameLevel.lvlDeathStar.dsX + (SCREEN_WIDTH >> 3)), (gameLevel.lvlDeathStar.dsY + (SCREEN_WIDTH >> 4) - 1), 0xFF7F7F7F);
          graphics2dDrawLineGrad(gameLevel.lvlDeathStar.dsX, (gameLevel.lvlDeathStar.dsY + (SCREEN_WIDTH >> 4)), 0xFF000000, (gameLevel.lvlDeathStar.dsX + (SCREEN_WIDTH >> 3)), (gameLevel.lvlDeathStar.dsY + (SCREEN_WIDTH >> 4)), 0xFF3F3F3F);
          graphics2dDrawLineGrad(gameLevel.lvlDeathStar.dsX, (gameLevel.lvlDeathStar.dsY + (SCREEN_WIDTH >> 4) + 1), 0xFF000000, (gameLevel.lvlDeathStar.dsX + (SCREEN_WIDTH >> 3)), (gameLevel.lvlDeathStar.dsY + (SCREEN_WIDTH >> 4) + 1), 0xFF7F7F7F);
          graphics2dDrawSpheroidShaded((gameLevel.lvlDeathStar.dsX + (SCREEN_WIDTH >> 4) + (SCREEN_WIDTH >> 6)), ((gameLevel.lvlDeathStar.dsY + (SCREEN_WIDTH >> 4)) - (SCREEN_WIDTH >> 5)), (SCREEN_WIDTH >> 5), (SCREEN_WIDTH >> 5), 0xFF7F7F7F, (MATH_PI * 0.5f));
          graphics2dDrawPoint(tempLaserOriginX, tempLaserOriginY, 0xFF000000);
          if(gameLevel.lvlDeathStar.dsCooldownTime <= 0.0f) {
               gameLevel.lvlDeathStar.dsWarmupTime -= tempLevelTick;
               if(gameLevel.lvlDeathStar.dsWarmupTime <= 0.0f) {
                    gameLevel.lvlDeathStar.dsWarmupTime = DEATHSTAR_WARMUP_MIN + (mathRandFractf() * DEATHSTAR_WARMUP_VARIANCE);
                    int tempCount = 0;
                    for(i = 0; i < gameLevel.lvlPlanetCount; i++) {
                         if(gameLevel.lvlPlanets[i].pntAlive)
                              tempCount++;
                    }
                    if(tempCount > 0) {
                         gameLevel.lvlDeathStar.dsTarget = mathRandi(gameLevel.lvlPlanetCount);
                         while(!gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntAlive)
                              gameLevel.lvlDeathStar.dsTarget = mathRandi(gameLevel.lvlPlanetCount);
                         gameLevel.lvlDeathStar.dsCooldownTime = DEATHSTAR_COOLDOWN_TIME;
                    }
               }
          } else {
               gameLevel.lvlDeathStar.dsCooldownTime -= tempLevelTick;
               if(gameLevel.lvlDeathStar.dsCooldownTime <= 0.0f) {
                    gameLevel.lvlDeathStar.dsWarmupTime += gameLevel.lvlDeathStar.dsCooldownTime;
                    gameLevel.lvlDeathStar.dsCooldownTime = 0.0f;
                    gameLevel.lvlDeathStar.dsTarget = -1;
               } else {
                    if(gameLevel.lvlDeathStar.dsTarget >= 0) {
                         int tempGatherPointX = tempLaserOriginX + (((gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntX + (gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntDiam / 2)) - tempLaserOriginX) / 6);
                         int tempGatherPointY = tempLaserOriginY + (((gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntY + (gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntDiam / 2)) - tempLaserOriginY) / 6);
                         graphicsSetLinesAntiAlias(true);
                         if(gameLevel.lvlDeathStar.dsCooldownTime > (DEATHSTAR_COOLDOWN_TIME - DEATHSTAR_GATHER_TIME)) {
                              if(gameLevel.lvlDeathStar.dsCooldownTime <= (DEATHSTAR_COOLDOWN_TIME - ((DEATHSTAR_GATHER_TIME * 0) / 7)))
                                   graphics2dDrawLine((tempLaserOriginX - 6), tempLaserOriginY, tempGatherPointX, tempGatherPointY, 0xFF00FF00);
                              if(gameLevel.lvlDeathStar.dsCooldownTime <= (DEATHSTAR_COOLDOWN_TIME - ((DEATHSTAR_GATHER_TIME * 1) / 7)))
                                   graphics2dDrawLine((tempLaserOriginX - 4), (tempLaserOriginY - 4), tempGatherPointX, tempGatherPointY, 0xFF00FF00);
                              if(gameLevel.lvlDeathStar.dsCooldownTime <= (DEATHSTAR_COOLDOWN_TIME - ((DEATHSTAR_GATHER_TIME * 2) / 7)))
                                   graphics2dDrawLine(tempLaserOriginX, (tempLaserOriginY - 6), tempGatherPointX, tempGatherPointY, 0xFF00FF00);
                              if(gameLevel.lvlDeathStar.dsCooldownTime <= (DEATHSTAR_COOLDOWN_TIME - ((DEATHSTAR_GATHER_TIME * 3) / 7)))
                                   graphics2dDrawLine((tempLaserOriginX + 4), (tempLaserOriginY - 6), tempGatherPointX, tempGatherPointY, 0xFF00FF00);
                              if(gameLevel.lvlDeathStar.dsCooldownTime <= (DEATHSTAR_COOLDOWN_TIME - ((DEATHSTAR_GATHER_TIME * 4) / 7)))
                                   graphics2dDrawLine((tempLaserOriginX + 6), tempLaserOriginY, tempGatherPointX, tempGatherPointY, 0xFF00FF00);
                              if(gameLevel.lvlDeathStar.dsCooldownTime <= (DEATHSTAR_COOLDOWN_TIME - ((DEATHSTAR_GATHER_TIME * 5) / 7)))
                                   graphics2dDrawLine((tempLaserOriginX + 4), (tempLaserOriginY + 4), tempGatherPointX, tempGatherPointY, 0xFF00FF00);
                              if(gameLevel.lvlDeathStar.dsCooldownTime <= (DEATHSTAR_COOLDOWN_TIME - ((DEATHSTAR_GATHER_TIME * 6) / 7)))
                                   graphics2dDrawLine(tempLaserOriginX, (tempLaserOriginY + 6), tempGatherPointX, tempGatherPointY, 0xFF00FF00);
                         } else {
                              if(gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntAlive) {
                                   gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntAlive = false;                                                                   
                                   gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntExplodeTime = (PLANET_EXPLODE_TIME + PLANET_EXPAND_TIME);
                              }
                              graphics2dDrawLine(tempGatherPointX, tempGatherPointY, (gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntX + (gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntDiam / 2)), (gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntY + (gameLevel.lvlPlanets[gameLevel.lvlDeathStar.dsTarget].pntDiam / 2)), colorLumMult8888(0xFF00FF00, (gameLevel.lvlDeathStar.dsCooldownTime / (DEATHSTAR_COOLDOWN_TIME - DEATHSTAR_GATHER_TIME)))); //((((u32)((255.0f * gameLevel.lvlDeathStar.dsCooldownTime) / DEATHSTAR_COOLDOWN_TIME)) << 8) | 0xFF000000));
                              graphics2dDrawLine((tempLaserOriginX - 6), tempLaserOriginY, tempGatherPointX, tempGatherPointY, colorLumMult8888(0xFF00FF00, (gameLevel.lvlDeathStar.dsCooldownTime / (DEATHSTAR_COOLDOWN_TIME - DEATHSTAR_GATHER_TIME)))); // , 0xFF00FF00);
                              graphics2dDrawLine((tempLaserOriginX - 4), (tempLaserOriginY - 4), tempGatherPointX, tempGatherPointY, colorLumMult8888(0xFF00FF00, (gameLevel.lvlDeathStar.dsCooldownTime / (DEATHSTAR_COOLDOWN_TIME - DEATHSTAR_GATHER_TIME)))); // , 0xFF00FF00);
                              graphics2dDrawLine(tempLaserOriginX, (tempLaserOriginY - 6), tempGatherPointX, tempGatherPointY, colorLumMult8888(0xFF00FF00, (gameLevel.lvlDeathStar.dsCooldownTime / (DEATHSTAR_COOLDOWN_TIME - DEATHSTAR_GATHER_TIME)))); // ,  0xFF00FF00);
                              graphics2dDrawLine((tempLaserOriginX + 4), (tempLaserOriginY - 6), tempGatherPointX, tempGatherPointY, colorLumMult8888(0xFF00FF00, (gameLevel.lvlDeathStar.dsCooldownTime / (DEATHSTAR_COOLDOWN_TIME - DEATHSTAR_GATHER_TIME)))); // , 0xFF00FF00);
                              graphics2dDrawLine((tempLaserOriginX + 6), tempLaserOriginY, tempGatherPointX, tempGatherPointY, colorLumMult8888(0xFF00FF00, (gameLevel.lvlDeathStar.dsCooldownTime / (DEATHSTAR_COOLDOWN_TIME - DEATHSTAR_GATHER_TIME)))); // , 0xFF00FF00);
                              graphics2dDrawLine((tempLaserOriginX + 4), (tempLaserOriginY + 4), tempGatherPointX, tempGatherPointY, colorLumMult8888(0xFF00FF00, (gameLevel.lvlDeathStar.dsCooldownTime / (DEATHSTAR_COOLDOWN_TIME - DEATHSTAR_GATHER_TIME)))); // , 0xFF00FF00);
                              graphics2dDrawLine(tempLaserOriginX, (tempLaserOriginY + 6), tempGatherPointX, tempGatherPointY, colorLumMult8888(0xFF00FF00, (gameLevel.lvlDeathStar.dsCooldownTime / (DEATHSTAR_COOLDOWN_TIME - DEATHSTAR_GATHER_TIME)))); // , 0xFF00FF00);
                              graphics2dDrawLine((tempLaserOriginX - 4), (tempLaserOriginY + 4), tempGatherPointX, tempGatherPointY, colorLumMult8888(0xFF00FF00, (gameLevel.lvlDeathStar.dsCooldownTime / (DEATHSTAR_COOLDOWN_TIME - DEATHSTAR_GATHER_TIME)))); // , 0xFF00FF00);
                         }
                         graphicsSetLinesAntiAlias(false);
                    }
               }
          }
     }

     // Draw atmosphere 
     graphics2dDrawQuadFilledGrad(0, 0, gameLevel.lvlBackColor[0], SCREEN_WIDTH, 0, gameLevel.lvlBackColor[1], SCREEN_WIDTH, SCREEN_HEIGHT, gameLevel.lvlBackColor[2], 0, SCREEN_HEIGHT, gameLevel.lvlBackColor[3]);
     //graphics2dDrawQuadFilledGrad(0, 0, gameLevel.lvlBackColor[0], SCREEN_WIDTH, 0, gameLevel.lvlBackColor[1], SCREEN_WIDTH, SCREEN_HEIGHT, gameLevel.lvlBackColor[2], 0, SCREEN_HEIGHT, colorLumMult8888(gameLevel.lvlBackColor[3], 0.75f));
     
     // Draw level background rock
     u32 templevelBackColor = colorLumMult8888(gameLevel.lvlColor, 0.5f);
     u32 templevelBackGroundColor = colorLumMult8888(gameLevel.lvlGroundColor, 0.5f);
     u32 templevelBackGroundDarkColor = colorLumMult8888(gameLevel.lvlGroundColor, 0.25f);
     for(i = 1; i < gameLevel.lvlPointsCount; i++) {
          graphics2dDrawTriangleFilledGrad(gameLevel.lvlBackPoints[(i - 1) * 2], SCREEN_HEIGHT, templevelBackGroundDarkColor, gameLevel.lvlBackPoints[(i - 1) * 2], gameLevel.lvlBackPoints[((i - 1) * 2) + 1], templevelBackGroundColor, gameLevel.lvlBackPoints[i * 2], SCREEN_HEIGHT, templevelBackGroundDarkColor);
          graphics2dDrawTriangleFilledGrad(gameLevel.lvlBackPoints[(i - 1) * 2], gameLevel.lvlBackPoints[((i - 1) * 2) + 1], templevelBackGroundColor, gameLevel.lvlBackPoints[i * 2], gameLevel.lvlBackPoints[(i * 2) + 1], templevelBackGroundColor, gameLevel.lvlBackPoints[i * 2], SCREEN_HEIGHT, templevelBackGroundDarkColor);
          graphicsSetLinesAntiAlias(true);
          graphics2dDrawLine(gameLevel.lvlBackPoints[(i - 1) * 2], gameLevel.lvlBackPoints[((i - 1) * 2) + 1], gameLevel.lvlBackPoints[i * 2], gameLevel.lvlBackPoints[(i * 2) + 1], templevelBackColor);
          graphicsSetLinesAntiAlias(false);
     }
     // Draw atmosphere
     //graphics2dDrawQuadFilledGrad(0, 0, gameLevel.lvlBackColor[0], SCREEN_WIDTH, 0, gameLevel.lvlBackColor[1], SCREEN_WIDTH, SCREEN_HEIGHT, gameLevel.lvlBackColor[2], 0, SCREEN_HEIGHT, colorLumMult8888(gameLevel.lvlBackColor[3], 0.25f));
     // Draw level 
     for(i = 1; i < gameLevel.lvlPointsCount; i++) {
          graphics2dDrawTriangleFilledGrad(gameLevel.lvlPoints[(i - 1) * 2], SCREEN_HEIGHT, templevelBackGroundColor, gameLevel.lvlPoints[(i - 1) * 2], gameLevel.lvlPoints[((i - 1) * 2) + 1], gameLevel.lvlGroundColor, gameLevel.lvlPoints[i * 2], SCREEN_HEIGHT, templevelBackGroundColor);
          graphics2dDrawTriangleFilledGrad(gameLevel.lvlPoints[(i - 1) * 2], gameLevel.lvlPoints[((i - 1) * 2) + 1], gameLevel.lvlGroundColor, gameLevel.lvlPoints[i * 2], gameLevel.lvlPoints[(i * 2) + 1], gameLevel.lvlGroundColor, gameLevel.lvlPoints[i * 2], SCREEN_HEIGHT, templevelBackGroundColor);
          graphicsSetLinesAntiAlias(true);
          graphics2dDrawLine(gameLevel.lvlPoints[(i - 1) * 2], gameLevel.lvlPoints[((i - 1) * 2) + 1], gameLevel.lvlPoints[i * 2], gameLevel.lvlPoints[(i * 2) + 1], gameLevel.lvlColor);
          graphicsSetLinesAntiAlias(false);
          if(i == (gameLevel.lvlLandingPoint + 1)) {
               graphics2dDrawQuadFilledGrad(gameLevel.lvlPoints[(i - 1) * 2], gameLevel.lvlPoints[((i - 1) * 2) + 1], gameLevel.lvlExitBorderColor, gameLevel.lvlPoints[i * 2], gameLevel.lvlPoints[((i - 1) * 2) + 1], gameLevel.lvlExitBorderColor, gameLevel.lvlPoints[i * 2], (gameLevel.lvlPoints[((i - 1) * 2) + 1] + 8), gameLevel.lvlExitColor, gameLevel.lvlPoints[(i - 1) * 2], (gameLevel.lvlPoints[((i - 1) * 2) + 1] + 8), gameLevel.lvlExitColor);
               graphics2dDrawRect(gameLevel.lvlPoints[(i - 1) * 2], gameLevel.lvlPoints[((i - 1) * 2) + 1], (gameLevel.lvlPoints[i * 2] - gameLevel.lvlPoints[(i - 1) * 2]), 8, gameLevel.lvlExitBorderColor);
          }
     }
}

void gameRenderLoop() {
     char tempString[256];

     levelDraw();
     shipDraw();

     int tempBarWidth, tempBarLeft;

     tempBarWidth = (int)((shipFuel * 128.0f) / gameLevel.lvlFuel);
     tempBarLeft = 2;
     if(shipFuel < 1.0f) {
          gameFont->fontColor = 0xFF00007F;
          graphics2dDrawRectFilled(tempBarLeft, 2, 128, 12, 0xFF000000);
     } else {
          gameFont->fontColor = 0xFFFFFFFF;
          graphics2dDrawQuadFilledGrad(tempBarLeft, 2, 0xFF0000BF, 130, 2, 0xFF00BFBF, 130, 14, 0xFF00BFBF, 2, 14, 0xFF0000BF);
          if((shipFuel < gameLevel.lvlFuel) && (gameLevel.lvlFuel > 0)) {
               graphics2dDrawRectFilled((tempBarLeft +  tempBarWidth), 2, (128 - tempBarWidth), 12, 0xFF000000);
          }
     }
     graphics2dDrawRect(2, 2, 128, 12, 0xFFFFFFFF);
     sprintf(tempString, "Fuel: %i", (int)shipFuel);
     fontDraw2dString((66 - (fontStringWidth(gameFont, tempString) / 2)), 4, gameFont, tempString);
     gameFont->fontColor = 0xFFFFFFFF;

     tempBarLeft = 146;
     tempBarWidth = ((shipHealth << 7) / shipMaxHealth);
     graphics2dDrawQuadFilledGrad(tempBarLeft, 2, 0xFF0000BF, (tempBarLeft + 128), 2, 0xFFBF0000, (tempBarLeft + 128), 14, 0xFFBF0000, tempBarLeft, 14, 0xFF0000BF);
     if(shipHealth < shipMaxHealth) {
          graphics2dDrawRectFilled((tempBarLeft + tempBarWidth), 2, (128 - tempBarWidth), 12, 0xFF000000);
     }
     graphics2dDrawRect(146, 2, 128, 12, 0xFFFFFFFF);
     sprintf(tempString, "Health: %3.0f%%", (((float)shipHealth / (float)shipMaxHealth) * 100.0f));
     fontDraw2dString((210 - (fontStringWidth(gameFont, tempString) / 2)), 4, gameFont, tempString);
     
     tempBarLeft = (SCREEN_WIDTH - 130);
     graphics2dDrawQuadFilledGrad(tempBarLeft, 2, 0xFF0000BF, (tempBarLeft + 64), 2, 0xFFFFFF00, (tempBarLeft + 64), 14, 0xFFFFFF00, tempBarLeft, 14, 0xFF0000BF);
     graphics2dDrawQuadFilledGrad((tempBarLeft + 64), 2, 0xFFFFFF00, (tempBarLeft + 128), 2, 0xFF0000BF, (tempBarLeft + 128), 14, 0xFF0000BF, (tempBarLeft + 64), 14, 0xFFFFFF00);
     if(gameLevel.lvlWind != 0) {
          if(gameDifficulty == 3) {
               tempBarWidth = (64 - pos(gameLevel.lvlWind << 5));
          } else {
                tempBarWidth = (64 - pos(gameLevel.lvlWind << 6));
          }
          if(gameLevel.lvlWind < 0) {
               if(tempBarWidth > 0)
                    graphics2dDrawRectFilled(tempBarLeft, 2, tempBarWidth, 12, 0xFF000000);
               graphics2dDrawRectFilled((tempBarLeft + 64), 2, 64, 12, 0xFF000000);
          } else {
               if(tempBarWidth > 0)
                    graphics2dDrawRectFilled(((tempBarLeft + 128) - tempBarWidth), 2, tempBarWidth, 12, 0xFF000000);
               graphics2dDrawRectFilled(tempBarLeft, 2, 64, 12, 0xFF000000);
          }
     } else {
          graphics2dDrawRectFilled(tempBarLeft, 2, 128, 12, 0xFF000000);
     }
     graphics2dDrawRect((SCREEN_WIDTH - 130), 2, 128, 12, 0xFFFFFFFF);
     sprintf(tempString, "Wind: %i", gameLevel.lvlWind);
     fontDraw2dString(((SCREEN_WIDTH - 64)  - (fontStringWidth(gameFont, tempString) / 2)), 4, gameFont, tempString);

     gameFont->fontColor = 0xFFFFFFFF;
     sprintf(tempString, "HiScore: %i", gameHiScore);
     fontDraw2dString((SCREEN_WIDTH - 130), 16, gameFont, tempString);
     sprintf(tempString, "Score: %i", gameScore);
     fontDraw2dString((SCREEN_WIDTH - 130), 26, gameFont, tempString);

     sprintf(tempString, "Time");
     fontDraw2dString(296, 4, gameFont, tempString);
     sprintf(tempString, "%02.1f", gameTimeLeft);
     fontDraw2dString(296, 14, gameFont, tempString);
     
     if(gameDifficulty == 0) {
          gameFont->fontColor = 0xFFBF0000;
          fontDraw2dString(296, 26, gameFont, "EASY");
     } else if(gameDifficulty == 1) {
          gameFont->fontColor = 0xFF00BF00;
          fontDraw2dString(300, 26, gameFont, "MED");
     } else if(gameDifficulty == 2) {
          gameFont->fontColor = 0xFF0000BF;
          fontDraw2dString(296, 26, gameFont, "HARD");
     } else {
          gameFont->fontColor = 0xFF00007F;
          fontDraw2dString(296, 26, gameFont, "ULTRA");
     }
     
     gameFont->fontColor = 0xFFFFFFFF;

     sprintf(tempString, "Level: %i", gameLevel.lvlNumber);
     fontDraw2dString(2, 16, gameFont, tempString);

     sprintf(tempString, "Lives: ");
     fontDraw2dString(2, 26, gameFont, tempString);
     if(gameLives > 0) {
          int tempOffset = 2 + fontStringWidth(gameFont, tempString);
          int i;
          for(i = 0; i < gameLives; i++) {
               graphics2dDrawTriangleFilled(((i * 8) + tempOffset), 34, ((i * 8) + 4 + tempOffset), 34, ((i * 8) + 2 + tempOffset), 26, shipColor);
               graphics2dDrawTriangle(((i * 8) + tempOffset), 34, ((i * 8) + 4 + tempOffset), 34, ((i * 8) + 2 + tempOffset), 26, shipOutlineColor);
          }
     }

     sprintf(tempString, "Gravity: %i", gameLevel.lvlGravity);
     fontDraw2dString(146, 16, gameFont, tempString);
     sprintf(tempString, "Velocity: %.1f", mathPythag2f(shipVelocityX, shipVelocityY));
     fontDraw2dString(146, 26, gameFont, tempString);

     /*if(testEffect != 0) {
        if(testEffect == 1)
             graphicsEffectFsGrayscale();
        else if(testEffect == 2)
             graphicsEffectFsSepia();
        else if(testEffect == 3)
             graphicsEffectFsNightVision();
        else if(testEffect == 4)
             graphicsEffectFsThermalVision();
        else if(testEffect == 5)
             graphicsEffectFsLightEnhance();
        else if(testEffect == 6) 
             graphicsEffectFsMonochrome(0x00FF0000);
        else
             graphicsEffectFsDichrome(0x000000FF, 0x00FF0000);
     }*/
}

void gameDialog(char* inMessage, u32 inColor, bool inGame) {
     int tempX, tempY;
     int tempWidth, tempHeight;
     tempWidth = (fontStringWidth(gameFont, inMessage) + 16);
     tempHeight = ((fontHeight(gameFont) * 3) + 16);
     tempX = (SCREEN_WIDTH - tempWidth) / 2;
     tempY = (SCREEN_HEIGHT - tempHeight) / 2;
     inputLock(0.5f);
     while(flRunning) {
          if(inGame)
               gameRenderLoop();
          else
               levelDraw();
          
          graphics2dDrawRectFilled(tempX, tempY, tempWidth, tempHeight, 0xFF7F7F7F);
          graphics2dDrawRect(tempX, tempY, tempWidth, tempHeight, 0xFFFFFFFF);
          graphics2dDrawRect((tempX + 2), (tempY + 2), (tempWidth - 4), (tempHeight - 4), 0xFFFFFFFF);
          
          gameFont->fontColor = inColor;
          fontDraw2dStringCenter((tempY + 8), gameFont, inMessage);
          fontDraw2dStringCenter((tempY + (2 * fontHeight(gameFont)) + 8), gameFont, "Press [X/O]");
          
          if(!inGame) {
               char tempString[256];
               sprintf(tempString, "HiScore: %i", gameHiScore);
               fontDraw2dString((SCREEN_WIDTH - 130), 2, gameFont, tempString);
          }
          
          graphicsSwapBuffers();

          inputPoll();
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN)
               break;
          if(inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN)
               break;
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, inGame);
          }
     }
     //timerGetDeltaTime(gameTimer);
}

void gameHiScoreDialog(int inEntry, bool inGame) {
     int tempX, tempY;
     int tempWidth, tempHeight;
     int i;
     int tempDifficulty = gameDifficulty;
     char tempString[256];
     tempWidth = 288;
     tempHeight = 154;
     tempX = (SCREEN_WIDTH - tempWidth) / 2;
     tempY = (SCREEN_HEIGHT - tempHeight) / 2;
     inputLock(0.5f);
     while(flRunning) {
          if(inGame)
               gameRenderLoop();
          else
               levelDraw();
          
          graphics2dDrawRectFilled(tempX, tempY, tempWidth, tempHeight, 0xFF7F7F7F);
          if(inEntry >= 0)
               graphics2dDrawRectFilled((tempX + 3), ((tempY + 28) + (inEntry * 10)), (tempWidth - 6), 8, 0xFF3F3F3F);
          graphics2dDrawRect(tempX, tempY, tempWidth, tempHeight, 0xFFFFFFFF);
          graphics2dDrawRect((tempX + 2), (tempY + 2), (tempWidth - 4), (tempHeight - 4), 0xFFFFFFFF);
          
          if(tempDifficulty == 0)
               gameFont->fontColor = 0xFFBF0000;
          else if(tempDifficulty == 1)
               gameFont->fontColor = 0xFF00BF00;
          else if(tempDifficulty == 2)
               gameFont->fontColor = 0xFF0000BF;
          else
               gameFont->fontColor = 0xFF00007F;

          fontDraw2dStringCenter((tempY + 8), gameFont, "HI SCORES");
          
          for(i = 0; i < 10; i++) {
               strcpy(tempString, gameHiScoreNames[(tempDifficulty * 10) + i]);
               while(strlen(tempString) < 16)
                    sprintf(tempString, "%s ", tempString);
               sprintf(tempString, "%s  Lv.%03i  %08i", tempString, gameHiScoreLevels[(tempDifficulty * 10) + i] ,gameHiScores[(tempDifficulty * 10) + i]);
               fontDraw2dStringCenter(((tempY + 28) + (i * 10)), gameFont, tempString);
          }

          fontDraw2dStringCenter((tempY + 138), gameFont, "[X/O] Close   [</>] Difficulty");
          
          if(!inGame) {
               gameFont->fontColor = 0xFFFFFFFF;
               char tempString[256];
               sprintf(tempString, "HiScore: %i", gameHiScores[tempDifficulty * 10]);
               fontDraw2dString((SCREEN_WIDTH - 130), 2, gameFont, tempString);
          }
          
          graphicsSwapBuffers();

          inputPoll();
          if((inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN) || (inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN))
               break;
          if(inputCheck(INPUT_DPAD_RIGHT) == INPUT_JUST_DOWN) {
               tempDifficulty++;
               if(gameUltraUnlocked) {
                    if(tempDifficulty > 3)
                         tempDifficulty = 0;
               } else if(tempDifficulty > 2) {
                    tempDifficulty = 0;
               }
          }
          if(inputCheck(INPUT_DPAD_LEFT) == INPUT_JUST_DOWN) {
               tempDifficulty--;
               if(gameUltraUnlocked) {
                    if(tempDifficulty < 0)
                         tempDifficulty = 3;
               } else if(tempDifficulty < 0) {
                    tempDifficulty = 2;
               }
          }
          
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, inGame);
          }
     }
     //timerGetDeltaTime(gameTimer);
}

void gameLevelNextDialog(float inLandingScore, int inScore) {
     int tempX, tempY;
     int tempWidth, tempHeight;
     tempWidth = 216;
     tempHeight = 124;
     tempX = (SCREEN_WIDTH - tempWidth) / 2;
     tempY = (SCREEN_HEIGHT - tempHeight) / 2;
     inputLock(0.5f);
     while(flRunning) {
          gameRenderLoop();
          
          graphics2dDrawRectFilled(tempX, tempY, tempWidth, tempHeight, 0xFF7F7F7F);
          graphics2dDrawRect(tempX, tempY, tempWidth, tempHeight, 0xFFFFFFFF);
          graphics2dDrawRect((tempX + 2), (tempY + 2), (tempWidth - 4), (tempHeight - 4), 0xFFFFFFFF);
          
          if(gameDifficulty == 0)
               gameFont->fontColor = 0xFFBF0000;
          else if(gameDifficulty == 1)
               gameFont->fontColor = 0xFF00BF00;
          else if(gameDifficulty == 2)
               gameFont->fontColor = 0xFF0000BF;
          else
               gameFont->fontColor = 0xFF00007F;

          char tempString[256];
          sprintf(tempString, "LEVEL %i COMPLETE!", gameLevel.lvlNumber);
          fontDraw2dStringCenter((tempY + 8), gameFont, tempString);
          
          sprintf(tempString, "Base Score: %i", (gameLevel.lvlNumber * 100));
          fontDraw2dStringCenter((tempY + 28), gameFont, tempString);
          sprintf(tempString, "Landing Multiplier: %03.1f%%", (inLandingScore * 100.0f));
          fontDraw2dStringCenter((tempY + 38), gameFont, tempString);
          sprintf(tempString, "Fuel Multiplier: %03.1f%%", (((float)shipFuel / (float)gameLevel.lvlFuel) * 100.0f));
          fontDraw2dStringCenter((tempY + 48), gameFont, tempString);
          sprintf(tempString, "Difficulty Bonus: X%i", ((gameDifficulty * gameDifficulty) + 1));
          fontDraw2dStringCenter((tempY + 58), gameFont, tempString);
          sprintf(tempString, "Time Bounus: +%i", (int)((gameTimeLeft / gameLevel.lvlTime) * ((float)gameLevel.lvlNumber * 50.0f)));
          fontDraw2dStringCenter((tempY + 68), gameFont, tempString);
          sprintf(tempString, "-------------------");
          fontDraw2dStringCenter((tempY + 77), gameFont, tempString);
          sprintf(tempString, "Level Score: %i", inScore);
          fontDraw2dStringCenter((tempY + 86), gameFont, tempString);
          
          fontDraw2dStringCenter((tempY + 108), gameFont, "Press [X/O]");
          
          graphicsSwapBuffers();

          inputPoll();
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN)
               break;
          if(inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN)
               break;
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, true);
          }
     }
     //timerGetDeltaTime(gameTimer);
}

int gameCalcClosestGround(int inX, int inWidth) {
     int tempSectionWidth = (SCREEN_WIDTH / (gameLevel.lvlPointsCount - 1));
     float tempClosestGround[2];
     float tempClosestGroundHeight;
     float tempClosestFraction;
     int tempOut = SCREEN_HEIGHT;
     int i;
     for(i = 0; i < inWidth; i++) {
          tempClosestGround[0] = levelPointsGet(((((inX + i) * (gameLevel.lvlPointsCount - 1)) / SCREEN_WIDTH) + 0), 1);
          tempClosestGround[1] = levelPointsGet(((((inX + i) * (gameLevel.lvlPointsCount - 1)) / SCREEN_WIDTH) + 1), 1);
          tempClosestFraction = ((inX + i) - levelPointsGet(((((inX + i) * (gameLevel.lvlPointsCount - 1)) / SCREEN_WIDTH) + 0), 0));
          tempClosestGroundHeight = ((tempClosestGround[1] * tempClosestFraction) + (tempClosestGround[0] * (tempSectionWidth - tempClosestFraction))) / tempSectionWidth;
          if(tempClosestGroundHeight < tempOut)
               tempOut = tempClosestGroundHeight;
     }
     return tempOut;
}

void levelCreateRandom(int inPoints, float inFraction, float inDiversity) {
     int i, j = 0;
     //gameLevel.lvlGroundColor = 0xFF000000 + (mathRandi(128) + 63) + ((mathRandi(128) + 63) << 8) + ((mathRandi(128) + 63) << 16);
     gameLevel.lvlGroundColor = colorRandom8888(64, 128, 255);
     //gameLevel.lvlColor = gameLevel.lvlGroundColor + 0x00404040;
     gameLevel.lvlColor = colorLumMult8888(gameLevel.lvlGroundColor, 1.5f);
     //gameLevel.lvlBackColor = (mathRandi(64) << 24) + mathRandi(255) + (mathRandi(255) << 8) + (mathRandi(255) << 16);
     if(!mathRandi(8)) {
          for(i = 0; i < 4; i++) {
               if(!mathRandi(4))
                    gameLevel.lvlBackColor[i] = colorRandom8888(0, 255, mathRandi(256));
               else
                    gameLevel.lvlBackColor[i] = colorRandom8888(0, 255, mathRandi(128));
          }
     } else {
          for(i = 0; i < 4; i++)
               gameLevel.lvlBackColor[i] = colorRandom8888(0, 255, mathRandi(256));
     }
     gameLevel.lvlSunCount = mathRandi(3);
     gameLevel.lvlPlanetCount = mathRandi(5);
     u8 tempYellow, tempRed;
     for(i = 0; i < 4; i++) {
          if(i < gameLevel.lvlSunCount) {
               gameLevel.lvlSuns[i][2] = (mathRandi(SCREEN_WIDTH >> 2) + 4);
               gameLevel.lvlSuns[i][0] = (mathRandi(SCREEN_WIDTH) - (gameLevel.lvlSuns[i][2] / 2));
               gameLevel.lvlSuns[i][1] = (mathRandi((SCREEN_HEIGHT * 3) / 4) - (gameLevel.lvlSuns[i][2] / 2));
               tempRed = mathRandi(64) + 192;
               tempYellow = mathRandi(tempRed + 1);
               if(tempYellow < 64)
                    tempYellow = 64;
               gameLevel.lvlSuns[i][3] = colorRGB8888(tempRed, tempYellow, (mathRandi(tempYellow + 1) >> 1));
               tempRed = mathRandi(128) + 64;
               tempYellow = mathRandi(tempRed + 1);
               if(tempYellow < 32)
                    tempYellow = 32;
               gameLevel.lvlSuns[i][4] = colorRGB8888(tempRed, tempYellow, (mathRandi((tempYellow + 1) >> 1) >> 1));
               for(j = 0;((j < 4) && (colorLum8888(gameLevel.lvlSuns[i][4]) > colorLum8888(gameLevel.lvlSuns[i][3]))); j++)
                    colorLumMult8888(gameLevel.lvlSuns[i][4], 0.75f);
          }
          if(i < gameLevel.lvlPlanetCount) {
               gameLevel.lvlPlanets[i].pntAlive = true;
               gameLevel.lvlPlanets[i].pntDiam = (mathRandi(SCREEN_WIDTH >> 3) + 4);
               gameLevel.lvlPlanets[i].pntX = (mathRandi(SCREEN_WIDTH) - (gameLevel.lvlPlanets[i].pntDiam / 2));
               gameLevel.lvlPlanets[i].pntY = (mathRandi((SCREEN_HEIGHT * 3) / 4) - (gameLevel.lvlPlanets[i].pntDiam / 2));
               gameLevel.lvlPlanets[i].pntColor = colorRandom8888(64, 128, 255);
               gameLevel.lvlPlanets[i].pntExplodeTime = 0.0f;
          }
     }
     if(gameLevel.lvlPlanetCount > 0) {
          gameLevel.lvlDeathStar.dsActive = (mathRandi(8) == 0);
          if(gameLevel.lvlDeathStar.dsActive) {
               gameLevel.lvlDeathStar.dsX = mathRandi(SCREEN_WIDTH) - (SCREEN_WIDTH >> 4);
               gameLevel.lvlDeathStar.dsY = mathRandi(SCREEN_HEIGHT / 2) - (SCREEN_WIDTH >> 4);
               gameLevel.lvlDeathStar.dsWarmupTime = DEATHSTAR_WARMUP_MIN + (mathRandFractf() * DEATHSTAR_WARMUP_VARIANCE);
               gameLevel.lvlDeathStar.dsTarget = -1;
               gameLevel.lvlDeathStar.dsCooldownTime = 0.0f;
          }
     } else {
          gameLevel.lvlDeathStar.dsActive = false;
     }
     
     
     if(gameDifficulty == 0) {
          gameLevel.lvlExitColor = 0xFF7F0000;
          gameLevel.lvlExitBorderColor = 0xFFFF0000;
     } else if(gameDifficulty == 1) {
          gameLevel.lvlExitColor = 0xFF007F00;
          gameLevel.lvlExitBorderColor = 0xFF00FF00;
     } else if(gameDifficulty == 2){
          gameLevel.lvlExitColor = 0xFF00007F;
          gameLevel.lvlExitBorderColor = 0xFF0000FF;
     } else {
          gameLevel.lvlExitColor = 0xFF00003F;
          gameLevel.lvlExitBorderColor = 0xFF00007F;
     }

     gameLevel.lvlPointsCount = (inPoints + 2);
     memFree(gameLevel.lvlPoints);
     gameLevel.lvlPoints = memAlloc((gameLevel.lvlPointsCount * 2) << 2);
     if(!gameLevel.lvlPoints)
          debugError("Level points couldn't be allocated.");
     memFree(gameLevel.lvlBackPoints);
     gameLevel.lvlBackPoints = memAlloc((gameLevel.lvlPointsCount * 2) << 2);
     if(!gameLevel.lvlBackPoints)
          debugError("Level background points couldn't be allocated.");
     
     gameLevel.lvlLandingPoint = mathRandi(inPoints - 1) + 1;
     gameLevel.lvlPoints[0] = 0;
     gameLevel.lvlPoints[1] = SCREEN_HEIGHT - (mathRandi(SCREEN_HEIGHT * (inFraction / 2)) + (SCREEN_HEIGHT / 4));
     bool tempIgnoreDiversity = false;
     for(i = 1; i < gameLevel.lvlPointsCount; i++) {
          tempIgnoreDiversity = ((mathRandi(9) + 1) <= 3);
          gameLevel.lvlPoints[i * 2] = ((SCREEN_WIDTH * i) / (gameLevel.lvlPointsCount - 1));
          gameLevel.lvlPoints[(i * 2) + 1] = SCREEN_HEIGHT - (mathRandi(SCREEN_HEIGHT * (inFraction / 2)) + (SCREEN_HEIGHT / 4));
          while((pos(gameLevel.lvlPoints[(i * 2) + 1] - gameLevel.lvlPoints[((i - 1) * 2) + 1]) > ((SCREEN_HEIGHT * (inFraction / 2)) * inDiversity)) && !tempIgnoreDiversity)
               gameLevel.lvlPoints[(i * 2) + 1] = SCREEN_HEIGHT - (mathRandi(SCREEN_HEIGHT * (inFraction / 2)) + (SCREEN_HEIGHT / 4));
          if(i == gameLevel.lvlLandingPoint) {
               gameLevel.lvlPoints[(i + 1) * 2] = ((SCREEN_WIDTH * (i + 1)) / (gameLevel.lvlPointsCount - 1));
               gameLevel.lvlPoints[((i + 1) * 2) + 1] = gameLevel.lvlPoints[(i * 2) + 1];
               i++;
          }
     }
     
     gameLevel.lvlBackPoints[0] = 0;
     gameLevel.lvlBackPoints[1] = SCREEN_HEIGHT - (mathRandi(SCREEN_HEIGHT * (inFraction / 2)) + (SCREEN_HEIGHT / 4));
     for(i = 1; i < gameLevel.lvlPointsCount; i++) {
          tempIgnoreDiversity = ((mathRandi(9) + 1) <= 3);
          gameLevel.lvlBackPoints[i * 2] = ((SCREEN_WIDTH * i) / (gameLevel.lvlPointsCount - 1));
          gameLevel.lvlBackPoints[(i * 2) + 1] = SCREEN_HEIGHT - (mathRandi(SCREEN_HEIGHT * (inFraction / 2)) + (SCREEN_HEIGHT / 3));
     }
     
     gameLevel.lvlStarsCount = ((mathRandi(inPoints) + 1) * 5);
     memFree(gameLevel.lvlStars);
     gameLevel.lvlStars = memAlloc((gameLevel.lvlStarsCount * 2) << 2);
     if(!gameLevel.lvlStars)
          debugError("Stars couldn't be allocated.");
     for(i = 0; i < gameLevel.lvlStarsCount; i++) {
          gameLevel.lvlStars[i * 2] = mathRandi(SCREEN_WIDTH - 1);
          gameLevel.lvlStars[(i * 2) + 1] = mathRandi((SCREEN_HEIGHT * 3) / 4);
     }
}

void levelRestart() {
     shipX = (SCREEN_WIDTH / 2);
     shipLastX = shipX;
     gameClosestGroundHeight = gameCalcClosestGround((shipX - 4), 8);
     shipY = (gameClosestGroundHeight - (SCREEN_HEIGHT / 4));
     shipLastY = shipY;
     shipColor = 0xFFBFBFBF;
     shipOutlineColor = shipColor + 0x00404040;
     shipUpthrust = false;
     shipLeftthrust = false;
     shipRightthrust = false;
     shipHyperthrust = false;
     shipVelocityX = 0;
     shipVelocityY = 0;
     shipFuel = gameLevel.lvlFuel;
     gameTimeLeft = gameLevel.lvlTime;
     gameStartDialog();
}

void levelNext() {
     gameLevel.lvlNumber++;

     if(gameDifficulty == 0) {
          gameLandingTolerance = 40 + (35 / gameLevel.lvlNumber);
          gameLevel.lvlFuel = 100.0f + (250.0f / (float)gameLevel.lvlNumber);
          gameLevel.lvlWind = 0;
          gameLevel.lvlGravity = 5 + (gameLevel.lvlNumber / 8);
          //gameLevel.lvlFriction = 0.5f;
          gameLevel.lvlFriction = 0.05f;
          levelCreateRandom(7 + gameLevel.lvlNumber, ((float)gameLevel.lvlNumber / ((float)gameLevel.lvlNumber + 5)), (2.0f / (float)gameLevel.lvlNumber));
          shipThrust = (int)(((float)gameLevel.lvlGravity * 1.5f) + (40.0f / (float)gameLevel.lvlNumber));
          shipHealth = min(shipMaxHealth, (shipHealth + 30));
          gameLevel.lvlTime = 20.0f;
          gameTimeLeft = gameLevel.lvlTime;
     } else if(gameDifficulty == 1) {
          gameLandingTolerance = 25 + (20 / gameLevel.lvlNumber);
          gameLevel.lvlFuel = 75.0f + (200.0f / (float)gameLevel.lvlNumber);
          gameLevel.lvlWind = 0;
          gameLevel.lvlGravity = 5 + (gameLevel.lvlNumber / 6);
          //gameLevel.lvlFriction = 0.5f;
          gameLevel.lvlFriction = 0.05f;
          levelCreateRandom(14 + gameLevel.lvlNumber, ((float)gameLevel.lvlNumber / ((float)gameLevel.lvlNumber + 4)), (1.5f / (float)gameLevel.lvlNumber));
          shipThrust = (int)(((float)gameLevel.lvlGravity * 1.25f) + (20.0f / (float)gameLevel.lvlNumber));
          shipHealth = min(shipMaxHealth, (shipHealth + 25));
          gameLevel.lvlTime = 15.0f;
          gameTimeLeft = gameLevel.lvlTime;
          if((gameLevel.lvlNumber > 50) && !gameMudkipUnlocked) {
               gameDialog("Mudkip mode unlocked!", 0xFFFFFFFF, true);
               gameMudkipUnlocked = true;
          }
     } else if(gameDifficulty == 2) {
          gameLandingTolerance = 15 + (10 / gameLevel.lvlNumber);
          gameLevel.lvlFuel = 50.0f + (150.0f / (float)gameLevel.lvlNumber);
          gameLevel.lvlWind = (mathRandi(3) - 1);
          gameLevel.lvlGravity = 5 + (gameLevel.lvlNumber / 4);
          //gameLevel.lvlFriction = 0.25f;
          gameLevel.lvlFriction = 0.025f;
          levelCreateRandom(28 + gameLevel.lvlNumber, ((float)gameLevel.lvlNumber / ((float)gameLevel.lvlNumber + 3)), (1.0f / (float)gameLevel.lvlNumber));
          shipThrust = (int)(((float)gameLevel.lvlGravity * 1.0f) + (10.0f / (float)gameLevel.lvlNumber));
          shipHealth = min(shipMaxHealth, (shipHealth + 20));
          gameLevel.lvlTime = 10.0f;
          gameTimeLeft = gameLevel.lvlTime;
          if((gameLevel.lvlNumber > 30)  && !gameUltraUnlocked){
               gameDialog("Ultra difficulty unlocked!", 0xFFFFFFFF, true);
               gameUltraUnlocked = true;
          }
     }  else {
          gameLandingTolerance = 15 + (10 / gameLevel.lvlNumber);
          gameLevel.lvlFuel = 15.0f + (100.0f / (float)gameLevel.lvlNumber);
          gameLevel.lvlWind = (mathRandi(5) - 2);
          gameLevel.lvlGravity = 5 + (gameLevel.lvlNumber / 4);
          //gameLevel.lvlFriction = 0.125f;
          gameLevel.lvlFriction = 0.0125f;
          levelCreateRandom(35 + gameLevel.lvlNumber, ((float)gameLevel.lvlNumber / ((float)gameLevel.lvlNumber + 2)), (0.75f / (float)gameLevel.lvlNumber));
          shipThrust = (int)(((float)gameLevel.lvlGravity * 0.75f) + (5.0f / (float)gameLevel.lvlNumber));
          shipHealth = min(shipMaxHealth, (shipHealth + 10));
          gameLevel.lvlTime = 5.0f;
          gameTimeLeft = gameLevel.lvlTime;
     }
     
     levelRestart();
}

void gameRestart() {
     gameLevel.lvlNumber = 0;
     gameLives = (5 - gameDifficulty);
     gameScore = 0;
     shipMaxHealth = 100;
     shipHealth = shipMaxHealth;
     levelNext();   
}

void gameInit() {
     gameFont = fontCreateDebug(0, 0, 0xFFFFFFFF);
     //gameFont = fontLoad("./flAstroFont.bmp");
     /*char tempString[256];
     sprintf(tempString, "mem:/%u:%i.tga", (unsigned int)flAstroFont, size_flAstroFont);
     gameFont = fontLoad(tempString);
     gameFont->fontHSpace = 1;
     gameFont->fontVSpace = 1;*/

     gameTimer = timerCreate();
     gameLevelTimer = timerCreate();
     
     //gameLevel = (Level*)memAlloc(sizeof(Level));
     //if(!gameLevel)
     //     debugCriticalError("Couldn't create Level struct.");
     
     gameMudkipTex = texCreate(mudkipSpriteWidth, mudkipSpriteHeight, GU_PSM_8888);
     int i, j;
     char* tempDataPtr = mudkipSpriteData;
     char* tempTexData = gameMudkipTex->texData;
     u8 tempPixel[3];
     for(j = 0; j < mudkipSpriteHeight; j++) {
          for(i = 0; i < mudkipSpriteWidth; i++) {
               MUDKIP_SPRITE_PIXEL(tempPixel, tempDataPtr);
               tempTexData[(((j * gameMudkipTex->texDataWidth * texBPP(gameMudkipTex)) >> 3) + (i << 2)) + 0] = tempPixel[0];
               tempTexData[(((j * gameMudkipTex->texDataWidth * texBPP(gameMudkipTex)) >> 3) + (i << 2)) + 1] = tempPixel[1];
               tempTexData[(((j * gameMudkipTex->texDataWidth * texBPP(gameMudkipTex)) >> 3) + (i << 2)) + 2] = tempPixel[2];
               if((tempPixel[0] == 0xFF) && (tempPixel[1] == 0xFF) && (tempPixel[2] == 0xFF))
                    tempTexData[(((j * gameMudkipTex->texDataWidth * texBPP(gameMudkipTex)) >> 3) + (i << 2)) + 3] = 0x00;
               else
                    tempTexData[(((j * gameMudkipTex->texDataWidth * texBPP(gameMudkipTex)) >> 3) + (i << 2)) + 3] = 0xFF;
          }
     }
     sceKernelDcacheWritebackAll();
     
     gameDifficulty = 1;
     gameHiScoresLoad();
}

void gameInstructionsDialog(bool inGame) {
     gameFont->fontColor = 0xFFFFFFFF;
     int tempX;
     while(flRunning) {
          if(inGame)
               gameRenderLoop();
          else
               levelDraw();
          
          tempX = 32;
          fontDraw2dStringCenter(tempX, gameFont, "- Instructions -"); tempX += 8; tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "The aim of this game is to land your spaceship  "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "as close to the center of the red, green, or blue"); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "colored landing pads. Bonus points are awarded  "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "for landing quickly, landing softly, landing    "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "close to the center and using fuel efficiently. "); tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "As the levels progress gravity starts to become "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "more of an issue, as does the rocky terrain and "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "damage recieved from bumps.                     "); tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "There are 3 difficulty settings:                "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "Easy:   This is more of a training mode, skilled"); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "        players will quickly tire of this.      "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "Medium: Most players should play this mode.     "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "Hard:   This mode adds more to all areas of     "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "        difficulty, aswell as adding in wind.   "); tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "Controls:                                       "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "DPAD LEFT/RIGHT - Thrust left/right.            "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "CROSS           - Normal thrust.                "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "CIRCLE          - Hyper thrust.                 "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "START           - Pause menu.                   "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "SELECT          - Take screenshot.              "); tempX += 8;
          tempX += 8;          
          fontDraw2dStringCenter(tempX, gameFont, "Press [X/O] to continue.                        ");

          if(!inGame) {
               char tempString[256];                      
               sprintf(tempString, "HiScore: %i", gameHiScore);
               fontDraw2dString((SCREEN_WIDTH - 130), 2, gameFont, tempString);
          }

          graphicsSwapBuffers();
          
          inputPoll();
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN)
               break;
          if(inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN)
               break;
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, false);
          }
     }
     
     //timerGetDeltaTime(gameTimer);
}

void gameCreditsDialog(bool inGame) {
     gameFont->fontColor = 0xFFFFFFFF;
     int tempX;
     char tempString[256];
     while(flRunning) {
          if(inGame)
               gameRenderLoop();
          else
               levelDraw();
          
          tempX = 32;
          fontDraw2dStringCenter(tempX, gameFont, "- Credits -"); tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "Programmer: Flatmush"); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "Unlock Graphics: Ryalla"); tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "The following have helped me and kept me from going mad:"); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "(or have they)"); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "Sleepy, InsertWittyName, Raphael, PSPJunkie, Xfacter,"); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "Zetablade, a_noob, filiph, Tomaz, mazor55, jsharrad,"); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "peterM, and anyone else who helped to test my game."); tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "Also a big thankyou to all the people at:"); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "http://ps2dev.org"); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "http://www.psp-programming.com"); tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "And all the other contributors to psp homebrew."); tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "And thankyou for playing :)"); tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "If apologize if I have left anyone out."); tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "The source for this program has not been released"); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "with it for a reason I can't disclose, however I "); tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "can promise that it will be released soon.       "); tempX += 8;
          tempX += 8;
          fontDraw2dStringCenter(tempX, gameFont, "Press [X/O] to continue.");
          
          sprintf(tempString, "HiScore: %i", gameHiScore);
          fontDraw2dString((SCREEN_WIDTH - 130), 2, gameFont, tempString);

          graphicsSwapBuffers();
          
          inputPoll();
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN)
               break;
          if(inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN)
               break;
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, false);
          }
     }
     
     //timerGetDeltaTime(gameTimer);
}

int gameLoadSaveDialog(bool inLoad, char* inOperation, bool inGame) {
     int tempMenuItem = 0;
     int tempX, tempY;
     int tempWidth, tempHeight;
     int i = 0;
     char tempString[256];
     tempWidth = fontStringWidth(gameFont, "Difficulty: Mudkip!") + 8;
     tempHeight = 156;
     tempX = (SCREEN_WIDTH - tempWidth) / 2;
     tempY = (SCREEN_HEIGHT - tempHeight) / 2;
     gameFont->fontColor = 0xFFFFFFFF;
     while(flRunning) {
          if(inGame)
               gameRenderLoop();
          else
               levelDraw();

          graphics2dDrawRectFilled(tempX, tempY, tempWidth, tempHeight, 0xFF7F7F7F);
          graphics2dDrawRect(tempX, tempY, tempWidth, tempHeight, 0xFFFFFFFF);
          graphics2dDrawRect((tempX + 2), (tempY + 2), (tempWidth - 4), (tempHeight - 4), 0xFFFFFFFF);
          graphics2dDrawRectFilled((tempX + 3), ((tempY + 20) + (tempMenuItem * 12) + (tempMenuItem == 10 ? 4 : 0)), (tempWidth - 6), 8, 0xFF3F3F3F);

          sprintf(tempString, "- %s -", inOperation);
          fontDraw2dStringCenter((tempY + 4), gameFont, tempString);

          for(i = 0; i < 10; i++) {
               sprintf(tempString, "./saveData%i.dat", i);
               if(fileExists(tempString)) {
                    sceUtilityGetSystemParamString(PSP_SYSTEMPARAM_ID_STRING_NICKNAME, tempString, 256);
                    sprintf(tempString, "%s %02i", tempString, (i + 1));
               } else {
                    sprintf(tempString, "<Empty%02i>", (i + 1));
               }
               fontDraw2dStringCenter(((tempY + 20) + (i * 12)), gameFont, tempString);
          }
          
          sprintf(tempString, "Cancel %s", inOperation);
          fontDraw2dStringCenter((tempY + 144), gameFont, tempString);
          
          if(!inGame) {
               char tempString[256];
               sprintf(tempString, "HiScore: %i", gameHiScore);
               fontDraw2dString((SCREEN_WIDTH - 130), 2, gameFont, tempString);
          }
          
          graphicsSwapBuffers();
          
          inputPoll();
          if(inputCheck(INPUT_DPAD_DOWN) == INPUT_JUST_DOWN) {
               tempMenuItem++;
               if(tempMenuItem > 10)
                    tempMenuItem = 0;
          } else if(inputCheck(INPUT_DPAD_UP) == INPUT_JUST_DOWN) {
               tempMenuItem--;
               if(tempMenuItem < 0)
                    tempMenuItem = 10;
          }
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN) {
               //timerGetDeltaTime(gameTimer);
               if(tempMenuItem == 10)
                    return -1;
               if(inLoad) {
                    sprintf(tempString, "./saveData%i.dat", tempMenuItem);
                    if(!fileExists(tempString))
                         return -1;
               }
               return tempMenuItem;
          }
          if(inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN)
               return -1;
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, inGame);
          }
     }
     
     //timerGetDeltaTime(gameTimer);
     return -1;
}

void gameOptionsDialog() {
     int tempMenuItem = 0;
     int tempX, tempY;
     int tempWidth, tempHeight;
     tempWidth = fontStringWidth(gameFont, "Difficulty: Mudkip!") + 8;
     tempHeight = 80;
     tempX = (SCREEN_WIDTH - tempWidth) / 2;
     tempY = (SCREEN_HEIGHT - tempHeight) / 2;
     gameFont->fontColor = 0xFFFFFFFF;
     while(flRunning) {
          levelDraw();

          graphics2dDrawRectFilled(tempX, tempY, tempWidth, tempHeight, 0xFF7F7F7F);
          graphics2dDrawRect(tempX, tempY, tempWidth, tempHeight, 0xFFFFFFFF);
          graphics2dDrawRect((tempX + 2), (tempY + 2), (tempWidth - 4), (tempHeight - 4), 0xFFFFFFFF);
          
          graphics2dDrawRectFilled((tempX + 3), ((tempY + 20) + (tempMenuItem * 12)), (tempWidth - 6), 8, 0xFF3F3F3F);

          gameFont->fontColor = 0xFFFFFFFF;
          fontDraw2dStringCenter((tempY + 4), gameFont, "- Options -");
          fontDraw2dStringCenter((tempY + 20), gameFont, "View HiScores");
          fontDraw2dStringCenter((tempY + 32), gameFont, "Clear HiScores");
          fontDraw2dStringCenter((tempY + 44), gameFont, "Delete Savegame");
          if(!gameMudkipUnlocked) {
               gameFont->fontColor = 0xFFBFBFBF;
               fontDraw2dStringCenter((tempY + 56), gameFont, "<Locked>");
               gameFont->fontColor = 0xFFFFFFFF;
          } else {
               if(gameMudkipMode)
                    fontDraw2dStringCenter((tempY + 56), gameFont, "Mudkip Mode: On");
               else
                    fontDraw2dStringCenter((tempY + 56), gameFont, "Mudkip Mode: Off");
          }
          fontDraw2dStringCenter((tempY + 68), gameFont, "Back");
          
          char tempString[256];
          sprintf(tempString, "HiScore: %i", gameHiScore);
          fontDraw2dString((SCREEN_WIDTH - 130), 2, gameFont, tempString);
          
          graphicsSwapBuffers();
          
          inputPoll();
          if(inputCheck(INPUT_DPAD_DOWN) == INPUT_JUST_DOWN) {
               tempMenuItem++;
               if(tempMenuItem > 4)
                    tempMenuItem = 0;
          } else if(inputCheck(INPUT_DPAD_UP) == INPUT_JUST_DOWN) {
               tempMenuItem--;
               if(tempMenuItem < 0)
                    tempMenuItem = 4;
          }
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN) {
               if(tempMenuItem == 0) {
                    gameHiScoreDialog(-1, false);
               } else if(tempMenuItem == 1) {
                    if(gameConfirmDialog("Clear HiScores & Unlocks?", 0xFFFFFFFF, false)) {
                         gameHiScoresClear();
                         gameDialog("HiScores Cleared.", 0xFFFFFFFF, false);
                    }
               } else if(tempMenuItem == 2) {
                    if(gameSaveDelete(gameLoadSaveDialog(true, "Delete", false)))
                         gameDialog("Save Deleted.", 0xFFFFFFFF, false);
               } else if(tempMenuItem == 3) {
                    if(gameMudkipUnlocked)
                         gameMudkipMode = !gameMudkipMode;
               } else {
                    break;
               }
          }
          if(inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN)
               break;
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, false);
          }
     }
     
     //timerGetDeltaTime(gameTimer);
}

bool gameMenu() {
     int tempMenuItem = 0;
     int tempX, tempY;
     int tempWidth, tempHeight;
     tempWidth = fontStringWidth(gameFont, "Difficulty: Mudkip!") + 8;
     tempHeight = 104;
     tempX = (SCREEN_WIDTH - tempWidth) / 2;
     tempY = (SCREEN_HEIGHT - tempHeight) / 2;
     levelCreateRandom(40, (26.0f / 40.0f), (1.5f / 26.0f));
     gameFont->fontColor = 0xFFFFFFFF;
     while(flRunning) {
          levelDraw();

          graphics2dDrawRectFilled(tempX, tempY, tempWidth, tempHeight, 0xFF7F7F7F);
          graphics2dDrawRect(tempX, tempY, tempWidth, tempHeight, 0xFFFFFFFF);
          graphics2dDrawRect((tempX + 2), (tempY + 2), (tempWidth - 4), (tempHeight - 4), 0xFFFFFFFF);
          
          graphics2dDrawRectFilled((tempX + 3), ((tempY + 20) + (tempMenuItem * 12)), (tempWidth - 6), 8, 0xFF3F3F3F);

          fontDraw2dStringCenter((tempY + 4), gameFont, "- flAstro -");
          fontDraw2dStringCenter((tempY + 20), gameFont, "New Game");
          if(gameDifficulty == 0) {
               gameFont->fontColor = 0xFFFF0000;
               fontDraw2dStringCenter((tempY + 32), gameFont, "Difficulty: Easy");
          } else if(gameDifficulty == 1) {
               gameFont->fontColor = 0xFF00FF00;
               fontDraw2dStringCenter((tempY + 32), gameFont, "Difficulty: Medium");
          } else if(gameDifficulty == 2) {
               gameFont->fontColor = 0xFF0000FF;
               fontDraw2dStringCenter((tempY + 32), gameFont, "Difficulty: Hard");
          } else {
               gameFont->fontColor = 0xFF00007F;
               fontDraw2dStringCenter((tempY + 32), gameFont, "Difficulty: ULTRA");
          }
          gameFont->fontColor = 0xFFFFFFFF;
          fontDraw2dStringCenter((tempY + 44), gameFont, "Load game");
          fontDraw2dStringCenter((tempY + 56), gameFont, "Instructions");
          fontDraw2dStringCenter((tempY + 68), gameFont, "Options");
          fontDraw2dStringCenter((tempY + 80), gameFont, "Credits");
          fontDraw2dStringCenter((tempY + 92), gameFont, "Quit");
          
          char tempString[256];
          sprintf(tempString, "HiScore: %i", gameHiScore);
          fontDraw2dString((SCREEN_WIDTH - 130), 2, gameFont, tempString);
          
          graphicsSwapBuffers();
          
          inputPoll();
          if(inputCheck(INPUT_DPAD_DOWN) == INPUT_JUST_DOWN) {
               tempMenuItem++;
               if(tempMenuItem > 6)
                    tempMenuItem = 0;
          } else if(inputCheck(INPUT_DPAD_UP) == INPUT_JUST_DOWN) {
               tempMenuItem--;
               if(tempMenuItem < 0)
                    tempMenuItem = 6;
          }
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN) {
               if(tempMenuItem == 0) {
                    break;
               } else if(tempMenuItem == 1) {
                     gameDifficulty++;
                     if(gameUltraUnlocked) {
                          if(gameDifficulty > 3)
                               gameDifficulty = 0;
                     } else if(gameDifficulty > 2) {
                          gameDifficulty = 0;
                     }
                     gameHiScore = gameHiScores[gameDifficulty * 10];                          
               } else if(tempMenuItem == 2) {
                    if(gameLoad(gameLoadSaveDialog(true, "Load", false))) {
                         gameDialog("Game loaded.", 0xFFFFFFFF, true);
                         gameStartDialog();
                         //timerGetDeltaTime(gameTimer);
                         return true;
                    }
               } else if(tempMenuItem == 3) {
                    gameInstructionsDialog(false);
               } else if(tempMenuItem == 4) {
                    gameOptionsDialog();
               } else if(tempMenuItem == 5) {
                    gameCreditsDialog(false);
               } else {
                    if(gameConfirmDialog("Really quit?", 0xFFFFFFFF, false))
                         flTerminate();
               }
          }
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, false);
          }
     }
     //timerGetDeltaTime(gameTimer);
     return false;
}

void shipCrash() {
     shipHealth = 0;
     gameLives--;
     if(gameLives < 0) {
          gameDialog("GAME OVER!", 0xFF0000BF, true);
          gameHiScoreDialog(gameHiScoresUpdate(), true);
          if(!gameMenu())
               gameRestart();
     } else {
          gameDialog("YOU CRASHED!", 0xFF0000BF, true);
          shipHealth = shipMaxHealth;
          levelRestart();
     }
}

bool shipDamage() {
     if((shipY + 7) <= gameClosestGroundHeight) {
          if(shipFuel < 1.0f) {
               shipCrash();
               return true;
          }
          if((shipLastX == shipX) && (shipLastY == shipY))
               return false;
     }
     shipHealth -= pos((int)(((float)shipVelocityY / ((float)((shipThrust * 10) * gameLandingTolerance) / 100.0f)) * 100.0f));
     if(shipHealth < 0) {
          shipCrash();
          return true;
     }
     return false;
}

void shipCollision(bool inForce) {
     if(shipDamage())
          return;
     if((shipVelocityY > (((shipThrust * 10) * gameLandingTolerance) / 100)) || inForce) {
           shipCrash();
           return;
     }
     
     if(shipVelocityY > 0)
          shipVelocityY = min(0, (1 - ((shipVelocityY / 2)) - gameLevel.lvlGravity));
     else
          shipVelocityY = max(0, (1 - ((shipVelocityY / 2)) - gameLevel.lvlGravity));
     if(shipVelocityX > 0)
          shipVelocityX = min(0, (1 - ((shipVelocityX / 2)) - gameLevel.lvlWind));
     else
          shipVelocityX = max(0, (1 - ((shipVelocityX / 2)) - gameLevel.lvlWind));
}

void shipLanding() {
     if(((shipY + 8) >= gameLevel.lvlPoints[(gameLevel.lvlLandingPoint * 2) + 1]) && ((shipY + 8) <= (gameLevel.lvlPoints[(gameLevel.lvlLandingPoint * 2) + 1] + ((8 * gameLandingTolerance) / 100)))) {
          if((shipVelocityX + shipVelocityY) <= (((shipThrust * 20) * gameLandingTolerance) / 100)) {
               float tempLandingScore = ((float)(shipVelocityX + shipVelocityY) / ((float)((shipThrust * 20) * gameLandingTolerance) / 100.0f));
               tempLandingScore = 1.0f - tempLandingScore;
               int tempPadWidth = gameLevel.lvlPoints[(gameLevel.lvlLandingPoint + 1) * 2] - gameLevel.lvlPoints[gameLevel.lvlLandingPoint * 2];
               int tempPadCenter = gameLevel.lvlPoints[gameLevel.lvlLandingPoint * 2] + (tempPadWidth / 2);
               tempPadWidth += 8;
               tempLandingScore *= (1.0f - ((float)(pos(tempPadCenter - shipX)) / ((float)tempPadWidth / 2.0f)));
               float tempScore = tempLandingScore;
               tempScore *= ((float)shipFuel / (float)gameLevel.lvlFuel);
               tempScore *= ((gameLevel.lvlNumber * 100) * ((gameDifficulty * gameDifficulty) + 1));
               tempScore += ((gameTimeLeft / gameLevel.lvlTime) * ((float)gameLevel.lvlNumber * 50.0f));
               gameScore += (int)tempScore;
               if(gameScore > gameHiScore)
                    gameHiScore = gameScore;
               gameLevelNextDialog(tempLandingScore, (int)tempScore);
               levelNext();
          } else {
               shipCollision(true);
          }
     } else {
          shipCollision(true);
     }
}

void gameTick() {
     shipLastX = shipX;
     shipLastY = shipY;
     
     shipVelocityX += gameLevel.lvlWind;
     shipVelocityY += gameLevel.lvlGravity;
     
     if(shipHyperthrust && (shipFuel >= (((float)shipThrust  * SHIP_FUEL_DRAINFACTOR) / 15.0f))) {
          shipVelocityY -= (shipThrust * 4);
          shipFuel -= ((float)shipThrust / (15 / SHIP_FUEL_DRAINFACTOR));
     }
     if(shipUpthrust && (shipFuel >= (((float)shipThrust * SHIP_FUEL_DRAINFACTOR) / 100.0f))) {
          shipVelocityY -= shipThrust;
          shipFuel -= ((float)shipThrust / (100 / SHIP_FUEL_DRAINFACTOR));
     }
     if(shipLeftthrust && !shipRightthrust && (shipFuel >= (((float)shipThrust * SHIP_FUEL_DRAINFACTOR) / 200.0f))) {
          shipVelocityX -= (shipThrust / 2);
          shipFuel -= ((float)shipThrust / (200 / SHIP_FUEL_DRAINFACTOR));
     }
     if(shipRightthrust && !shipLeftthrust && (shipFuel >= (((float)shipThrust * SHIP_FUEL_DRAINFACTOR) / 200.0f))) {
          shipVelocityX += (shipThrust / 2);
          shipFuel -= (((float)shipThrust * SHIP_FUEL_DRAINFACTOR) / 200.0f);
     }
     //shipVelocityX -= pow(((shipVelocityX * gameLevel.lvlFriction) / 100), 3);
     //shipVelocityY -= pow(((shipVelocityY * gameLevel.lvlFriction) / 100), 3);
     shipVelocityX = mathSigni(shipVelocityX) * (int)(mathSqrtf((float)(shipVelocityX * shipVelocityX) * (1.0f - gameLevel.lvlFriction)) + 0.5f);
     shipVelocityY = mathSigni(shipVelocityY) * (int)(mathSqrtf((float)(shipVelocityY * shipVelocityY) * (1.0f - gameLevel.lvlFriction)) + 0.5f);

     shipX += (shipVelocityX / 10);
     shipY += (shipVelocityY / 10);
 
     if((shipX - 4) < 0) {
          shipX = 4;
          shipVelocityX = 0;
     } else if((shipX + 4) >= SCREEN_WIDTH) {
          shipX = SCREEN_WIDTH - 5;
          shipVelocityX = 0;
     }
     if((shipY - 8) < -128) {
          shipY = -120;
          shipVelocityY = 0;
     } else if((shipY + 8) >= SCREEN_HEIGHT) {
          shipCollision(true);
          return;
     }
  
     gameClosestGroundHeight = gameCalcClosestGround((shipX - 4), 8);
     
     if((shipX >= ((SCREEN_WIDTH * gameLevel.lvlLandingPoint) / (gameLevel.lvlPointsCount - 1))) && (shipX <= ((SCREEN_WIDTH * (gameLevel.lvlLandingPoint + 1)) / (gameLevel.lvlPointsCount - 1)))) {
          if((shipY + 8) >= levelPointsGet(gameLevel.lvlLandingPoint, 1)) {
               if((shipY + 8) <= (levelPointsGet(gameLevel.lvlLandingPoint, 1) + 8)) {
                    shipLanding();
                    return;
               } else {
                    shipCollision(true);
               }
          }
     } else if((shipY + 8) >= gameClosestGroundHeight) {
               while((shipY + 8) >= gameClosestGroundHeight)
                    shipY--;
               shipCollision(false);
               if(shipVelocityY > 0)
                    shipVelocityY = 0;
     }
     gameTimeLeft = max(0, (gameTimeLeft - gameTickRate));
}

void gamePauseDialog() {
     int tempMenuItem = 0;
     int tempX, tempY;
     int tempWidth, tempHeight;
     tempWidth = fontStringWidth(gameFont, "Difficulty: Mudkip!") + 8;
     tempHeight = 68;
     tempX = (SCREEN_WIDTH - tempWidth) / 2;
     tempY = (SCREEN_HEIGHT - tempHeight) / 2;
     gameFont->fontColor = 0xFFFFFFFF;
     while(flRunning) {
          gameRenderLoop();

          graphics2dDrawRectFilled(tempX, tempY, tempWidth, tempHeight, 0xFF7F7F7F);
          graphics2dDrawRect(tempX, tempY, tempWidth, tempHeight, 0xFFFFFFFF);
          graphics2dDrawRect((tempX + 2), (tempY + 2), (tempWidth - 4), (tempHeight - 4), 0xFFFFFFFF);
          
          graphics2dDrawRectFilled((tempX + 3), ((tempY + 20) + (tempMenuItem * 12)), (tempWidth - 6), 8, 0xFF3F3F3F);

          fontDraw2dStringCenter((tempY + 4), gameFont, "- Paused -");
          fontDraw2dStringCenter((tempY + 20), gameFont, "Continue");
          fontDraw2dStringCenter((tempY + 32), gameFont, "Instructions");
          fontDraw2dStringCenter((tempY + 44), gameFont, "Save game");
          fontDraw2dStringCenter((tempY + 56), gameFont, "Quit");
          
          graphicsSwapBuffers();
          
          inputPoll();
          if(inputCheck(INPUT_DPAD_DOWN) == INPUT_JUST_DOWN) {
               tempMenuItem++;
               if(tempMenuItem > 3)
                    tempMenuItem = 0;
          } else if(inputCheck(INPUT_DPAD_UP) == INPUT_JUST_DOWN) {
               tempMenuItem--;
               if(tempMenuItem < 0)
                    tempMenuItem = 3;
          }
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN) {
               if(tempMenuItem == 0) {
                    gameStartDialog();
                    break;
               } else if(tempMenuItem == 1) {
                    gameInstructionsDialog(true);
               } else if(tempMenuItem == 2) {
                    if(gameSave(gameLoadSaveDialog(false, "Save", true)))
                         gameDialog("Game saved.", 0xFFFFFFFF, true);
               } else {
                    if(gameConfirmDialog("Really quit?", 0xFFFFFFFF, true)) {
                         gameDialog("GAME OVER!", 0xFF0000BF, true);
                         gameHiScoreDialog(gameHiScoresUpdate(), true);
                         if(!gameMenu())
                              gameRestart();
                         break;
                    }
               }
          }
          if((inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN) || (inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN)) {
                gameStartDialog();
                break;
          }
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, true);
          }
     }
     
     //timerGetDeltaTime(gameTimer);
}

int main() {
     flInitialize();
     
     //ntfyVisible = NOTIFY_VISIBLE_ALWAYS;

     gameInit();     
     if(!gameMenu())
          gameRestart();

     while(flRunning) {
          gameRenderLoop();
          graphicsSwapBuffers();
          
          //if(gamePaused) {
          //     timerGetDeltaTime(gameTimer);
          //} else {
               gameTickTime += timerGetDeltaTime(gameTimer);
               while(gameTickTime >= gameTickRate) {
                    gameTickTime -= gameTickRate;
                    gameTick();
               }
          //}
          
          inputPoll();
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.", 0xFFFFFFFF, true);
               timerGetDeltaTime(gameTimer);
          }

          shipHyperthrust = inputDown(INPUT_BUTTON_CIRCLE);
          shipLeftthrust = inputDown(INPUT_DPAD_LEFT);
          shipRightthrust = inputDown(INPUT_DPAD_RIGHT);
          shipUpthrust = (inputDown(INPUT_BUTTON_CROSS) && !shipHyperthrust);
          if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN) {
               //gamePaused = !gamePaused;
               gamePauseDialog();
          }
          /*if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN) {
             testEffect--;
             if(testEffect < 0)
                  testEffect = 7;
          }
          if(inputCheck(INPUT_BUTTON_RTRIGGER) == INPUT_JUST_DOWN) {
             testEffect++;
             if(testEffect > 7)
                  testEffect = 0;
          }*/
          if(inputCheck(INPUT_BUTTON_RTRIGGER) == INPUT_JUST_DOWN)
               ntfyShowAll(3.0f);
          if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN)
               graphicsFPSShow = !graphicsFPSShow;
     }
     
     flTerminate();
     return 0;
}
