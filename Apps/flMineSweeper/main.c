#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspkernel.h>
#include <string.h>
#include <stdio.h>

PSP_MODULE_INFO("flMineSweeper", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_FUNCLIB);

u32 gridWidth  = 0;
u32 gridHeight = 0;
u8* gridData   = NULL;
int gridMines  = 0;

void gameDialog(char* inMessage);

void gridCreate(u32 inWidth, u32 inHeight, u32 inMines);
void gridPlaceMines(u32 inCount);
void gridPlaceMine();
void gridDraw();
bool gridMined(int inX, int inY);
int  gridMarked(int inX, int inY);
bool gridCovered(int inX, int inY);
int  gridNumber(int inX, int inY);
int  gridSelect(int inX, int inY);
void gridMark(int inX, int inY);
int  gridCondition();

void levelCreate();
void levelWin();
void levelLose();

void hudDraw();

u32 cursorX = 0;
u32 cursorY = 0;

Timer* gameTimer    = NULL;
float  gameTime     = 0.0f;
float  gameTickRate = (1.0f / 50.0f);
float  gameTick     = 0.0f;

u32    gameLevel      = 0;





void gameDialog(char* inMessage) {
     int tempX, tempY;
     int tempWidth, tempHeight;
     tempWidth = (fontStringWidth(fontDefault, inMessage) + 16);
     tempWidth = max(tempWidth, (fontStringWidth(fontDefault, "Press [X/O]") + 16));
     tempHeight = ((fontHeight(fontDefault) * 3) + 16);
     tempX = (SCREEN_WIDTH - tempWidth) >> 1;
     tempY = (SCREEN_HEIGHT - tempHeight) >> 1;
     
     inputLock(0.5f);
     
     while(flRunning) {
          
          gridDraw();
          hudDraw();
          
          graphics2dDrawRectFilled(tempX, tempY, tempWidth, tempHeight, 0xFF7F7F7F);
          graphics2dDrawRect(tempX, tempY, tempWidth, tempHeight, 0xFFFFFFFF);
          
          fontDraw2dStringCenter((tempY + 8), fontDefault, inMessage);
          fontDraw2dStringCenter((tempY + (2 * fontHeight(fontDefault)) + 8), fontDefault, "Press [X/O]");
         
          graphicsSwapBuffers();

          inputPoll();
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN)
               break;
          if(inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN)
               break;
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
               debugScreenshot();
               gameDialog("Screenshot taken.");
          }
     }
}

void gridCreate(u32 inWidth, u32 inHeight, u32 inMines) {
     if(!inWidth || !inHeight)
          return;
     if((inWidth << 4) > SCREEN_WIDTH)
          inWidth = (SCREEN_WIDTH >> 4);
     if((inHeight << 4) > (SCREEN_HEIGHT - 16))
          inHeight = (SCREEN_HEIGHT >> 4) - 1;
     gridWidth = inWidth;
     gridHeight = inHeight;
     if(gridData)
          memFree(gridData);
     gridData = (u8*)memAlloc(inWidth * inHeight);
     if(!gridData)
          return;
     memSetByte(gridData, 1, (inWidth * inHeight));
     gridMines = 0;
     gridPlaceMines(inMines);
}

void gridPlaceMines(u32 inCount) {
     if(inCount > (gridWidth * gridHeight))
          inCount = (gridWidth * gridHeight);
     u32 i;
     for(i = 0; i < inCount; i++)
          gridPlaceMine();
}

void gridPlaceMine() {
     if(!gridData || !gridWidth || !gridHeight)
          return;
     if(gridMines == (gridWidth * gridHeight))
          return;
     u32 tempX = mathRandi(gridWidth);
     u32 tempY = mathRandi(gridHeight);
     while(gridMined(tempX, tempY)) {
          tempX = mathRandi(gridWidth);
          tempY = mathRandi(gridHeight);
     }
     gridData[(tempY * gridWidth) + tempX] |= 2;
     gridMines++;
}

void gridDraw() {
     if(!gridData || !gridWidth || !gridHeight)
          return;
          
     u32 tempX = ((SCREEN_WIDTH - (gridWidth << 4)) >> 1);
     u32 tempY = 16 + (((SCREEN_HEIGHT - 16) - (gridHeight << 4)) >> 1);
     
     u32 i; u32 j; int tempNumber;
     for(j = 0; j < gridHeight; j++) {
          for(i = 0; i < gridWidth; i++) {
               if(gridCovered(i, j)) {
                    graphics2dDrawRectFilled((tempX + (i << 4)), (tempY + (j << 4)), 16, 16, 0xFF3F3F3F);
                    int tempGridMark = gridMarked(i, j);
                    if(tempGridMark == 1) {
                         fontDraw2dChar((tempX + (i << 4) + 5), (tempY + (j << 4) + 5), fontDefault, 'X');
                    } else if (tempGridMark == 2) {
                         fontDraw2dChar((tempX + (i << 4) + 5), (tempY + (j << 4) + 5), fontDefault, '?');
                    }
               } else {
                    if(gridMined(i, j)) {
                         graphics2dDrawRectFilled((tempX + (i << 4)), (tempY + (j << 4)), 16, 16, 0xFF0000FF);
                         graphics2dDrawEllipseFilled(((tempX + (i << 4)) + 4), ((tempY + (j << 4)) + 4), 12, 12, 0xFF000000);
                    } else {
                         graphics2dDrawRectFilled((tempX + (i << 4)), (tempY + (j << 4)), 16, 16, 0xFFBFBFBF);
                         tempNumber = gridNumber(i, j);
                         if(tempNumber > 0)
                              fontDraw2dChar((tempX + (i << 4) + 5), (tempY + (j << 4) + 5), fontDefault, (tempNumber + '0'));
                    }
               }
          }     
     }
     
     for(i = 0; i <= gridWidth; i++)
          graphics2dDrawLine((tempX + (i << 4)), tempY, (tempX + (i << 4)), (tempY + (gridHeight << 4)), 0xFF7F7F7F);
     for(i = 0; i <= gridHeight; i++)
          graphics2dDrawLine(tempX, (tempY + (i << 4)), (tempX + (gridWidth << 4)), (tempY + (i << 4)), 0xFF7F7F7F);
     
     graphics2dDrawRectFilled((tempX + (cursorX << 4)), (tempY + (cursorY << 4)), 16, 16, 0x7F00FF00);
     graphics2dDrawRect((tempX + (cursorX << 4)), (tempY + (cursorY << 4)), 16, 16, 0xFF00FF00);
}

bool gridMined(int inX, int inY) {
     if(!gridData || !gridWidth || !gridHeight)
          return false;
     if((inX >= gridWidth) || (inY >= gridHeight))
          return false;
     if((inX < 0) || (inY < 0))
          return false;
          
     return ((gridData[(inY * gridWidth) + inX] & 2) == 2);
}

int gridMarked(int inX, int inY) {
     if(!gridData || !gridWidth || !gridHeight)
          return -1;
     if((inX >= gridWidth) || (inY >= gridHeight))
          return -1;
     if((inX < 0) || (inY < 0))
          return -1;
     
     if(!(gridData[(inY * gridWidth) + inX] & 4))
          return 0;
     if((gridData[(inY * gridWidth) + inX] & 8))
          return 2;
     return 1;
}

bool gridCovered(int inX, int inY) {
     if(!gridData || !gridWidth || !gridHeight)
          return -1;
     if((inX >= gridWidth) || (inY >= gridHeight))
          return -1;
     if((inX < 0) || (inY < 0))
          return -1;
     
     return ((gridData[(inY * gridWidth) + inX] & 1) == 1);
}

int gridNumber(int inX, int inY) {
     if(!gridData || !gridWidth || !gridHeight)
          return -1;
     if((inX >= gridWidth) || (inY >= gridHeight))
          return -1;
     if((inX < 0) || (inY < 0))
          return -1;
     
     int tempOut = 0;
     
     tempOut += (gridMined((inX - 1), (inY - 1)) ? 1 : 0);
     tempOut += (gridMined((inX - 1), inY) ? 1 : 0);
     tempOut += (gridMined((inX - 1), (inY + 1)) ? 1 : 0);
     
     tempOut += (gridMined(inX, (inY - 1)) ? 1 : 0);
     tempOut += (gridMined(inX, (inY + 1)) ? 1 : 0);
     
     tempOut += (gridMined((inX + 1), (inY - 1)) ? 1 : 0);
     tempOut += (gridMined((inX + 1), inY) ? 1 : 0);
     tempOut += (gridMined((inX + 1), (inY + 1)) ? 1 : 0);

     return tempOut;
}

int gridSelect(int inX, int inY) {
     if(!gridData || !gridWidth || !gridHeight)
          return -1;
     if((inX >= gridWidth) || (inY >= gridHeight))
          return -1;
     if((inX < 0) || (inY < 0))
          return -1;

     if(!(gridCovered(inX, inY)))
          return 0;
          
     if(gridMarked(inX, inY) != 0)
          return 0;
          
     gridData[(inY * gridWidth) + inX] ^= 1;
     
     if(gridMined(inX, inY))
          return 1;
     
     if(gridNumber(inX, inY) == 0) {
          gridSelect((inX - 1), inY);
          gridSelect((inX - 1), (inY - 1));
          gridSelect((inX - 1), (inY + 1));

          gridSelect(inX, (inY - 1));
          gridSelect(inX, (inY + 1));
          
          gridSelect((inX + 1), inY);
          gridSelect((inX + 1), (inY - 1));
          gridSelect((inX + 1), (inY + 1));
     }

     return 0;
}

void gridMark(int inX, int inY) {
     if(!gridData || !gridWidth || !gridHeight)
          return;
     if((inX >= gridWidth) || (inY >= gridHeight))
          return;
     if((inX < 0) || (inY < 0))
          return;
     
     if(!(gridCovered(inX, inY)))
          return;
     
     if(gridData[(inY * gridWidth) + inX] & 8) {
          gridData[(inY * gridWidth) + inX] &= ~12;
          return;
     }
     if(gridData[(inY * gridWidth) + inX] & 4) {
          gridData[(inY * gridWidth) + inX] |= 8;
          gridMines++;
          return;
     }
     
     gridData[(inY * gridWidth) + inX] |= 4;
     gridMines--;
}

int gridCondition() {
     if(!gridData || !gridWidth || !gridHeight)
          return -2;

     u32 tempTilesLeft = 0;
     u32 tempTilesWrong = 0;
     u32 i; u32 j;
     for(j = 0; j < gridHeight; j++) {
          for(i = 0; i < gridWidth; i++) {
               if(gridCovered(i, j)) {
                    if(!(gridMined(i, j))) {
                         tempTilesLeft++;
                         if(gridMarked(i, j) == 1)
                              tempTilesWrong++;
                    }
               } else if(gridMined(i, j)) {
                    return -1;
               }
          }
     }
     if(!gridMines && !tempTilesWrong)
          return 1;
     if(!tempTilesLeft)
          return 1;
     return 0;
}



void levelCreate() {
     u32 tempDimensions = (3 + gameLevel);
     u32 tempMines = ((tempDimensions * tempDimensions) >> 4) + gameLevel;
     if(tempMines == 0)
          tempMines++;
     while(tempMines >= ((tempDimensions * tempDimensions) >> 2))
          tempMines--;
     gridCreate(tempDimensions, tempDimensions, tempMines);
}

void levelWin() {
     gameDialog("You Win!");
     gameLevel++;
     gameTime = 0.0f;
     levelCreate();
}

void levelLose() {
     gameDialog("You Lose!");
     gameTime = 0.0f;
     levelCreate();
}




void hudDraw() {
     graphics2dDrawRectFilled(0, 0, SCREEN_WIDTH, 16, 0x7FFF0000);
     
     char tempString[256];
     
     sprintf(tempString, "Mines: %i", gridMines);
     fontDraw2dString(4, 4, fontDefault, tempString);

     sprintf(tempString, "Time: %.1f", gameTime);
     fontDraw2dStringCenter(4, fontDefault, tempString);
     
     sprintf(tempString, "Level: %u", (unsigned int)gameLevel);
     fontDraw2dStringFromRight((SCREEN_WIDTH - 4), 4, fontDefault, tempString);
}


int main() {
     flInitialize();

     gameTimer = timerCreate();
     gameLevel = 1;
     levelCreate();

     float tempTime;
     while(flRunning) {
          gridDraw();
          hudDraw();
          graphicsSwapBuffers();
          
          
          tempTime = timerGetDeltaTime(gameTimer);
          gameTime += tempTime;
          gameTick += tempTime;
          while(gameTick >= gameTickRate) {
               gameTick -= gameTickRate;
          
               inputPoll();
               if(inputCheck(INPUT_DPAD_UP) == INPUT_JUST_DOWN)
                    cursorY -= (cursorY > 0 ? 1 : 0);
               if(inputCheck(INPUT_DPAD_DOWN) == INPUT_JUST_DOWN)
                    cursorY += (cursorY < (gridHeight - 1) ? 1 : 0);
               if(inputCheck(INPUT_DPAD_LEFT) == INPUT_JUST_DOWN)
                    cursorX -= (cursorX > 0 ? 1 : 0);
               if(inputCheck(INPUT_DPAD_RIGHT) == INPUT_JUST_DOWN)
                    cursorX += (cursorX < (gridWidth - 1) ? 1 : 0);
                    
               if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN)
                    gridSelect(cursorX, cursorY);
               if(inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN)
                    gridMark(cursorX, cursorY);
                    
               if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN)
                    flTerminate();
               if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
                    debugScreenshot();
                    gameDialog("Screenshot taken.");
               }
               if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN)
                    graphicsFPSShow = !graphicsFPSShow;
                    
               int tempCondition = gridCondition();
               if(tempCondition < 0) {
                    if(tempCondition == -1)
                         levelLose();
                    else
                         levelCreate();
               } else if(tempCondition > 0) {
                    levelWin();
               }
          }
     }

     
     flTerminate();
     return 0;
}
