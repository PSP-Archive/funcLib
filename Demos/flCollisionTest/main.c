#include "././Code/funcLib.h"

PSP_MODULE_INFO("flCollisionTest", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

Font*    gameFont     = NULL;
Timer*   gameTimer    = NULL;
float    gameTickRate = 0.0125f;
float    gameTick     = 0.0f;

Line2d   playerLine;
Line2d   collisionLine;
Circle2d collisionCircle;

int main() {
     flInitialize();
     
     gameFont = fontCreateDebug(0, 0, 0xFF00FF00);
     gameTimer = timerCreate();
     
     playerLine.linePoints[0].x = (float)((SCREEN_WIDTH >> 1) - 64);
     playerLine.linePoints[0].y = (float)(SCREEN_HEIGHT >> 1);
     playerLine.linePoints[1].x = playerLine.linePoints[0].x + (float)(mathRandi(65) - 32);
     playerLine.linePoints[1].y = playerLine.linePoints[0].y + (float)(mathRandi(65) - 32);
     
     collisionLine.linePoints[0].x = (float)((SCREEN_WIDTH >> 1) + 64);
     collisionLine.linePoints[0].y = (float)(SCREEN_HEIGHT >> 1);
     collisionLine.linePoints[1].x = collisionLine.linePoints[0].x + (float)(mathRandi(65) - 32);
     collisionLine.linePoints[1].y = collisionLine.linePoints[0].y + (float)(mathRandi(65) - 32);
     
     collisionCircle.circCenter.x = (float)((SCREEN_WIDTH >> 1) + 128);
     collisionCircle.circCenter.y = (float)(SCREEN_HEIGHT >> 1);
     collisionCircle.circRadius = (float)mathRandi(33);
     
     while(flRunning) {
   
          graphics2dDrawRectFilled(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFF7F0000);
          if(collision2dLineLine(playerLine, collisionLine)) {
               graphics2dDrawLine(collisionLine.linePoints[0].x, collisionLine.linePoints[0].y, collisionLine.linePoints[1].x, collisionLine.linePoints[1].y, 0xFF0000FF);
               graphics2dDrawEllipse((collisionLast.x - 4), (collisionLast.y - 4), 8, 8, 0xFF00FFFF);
          } else
               graphics2dDrawLine(collisionLine.linePoints[0].x, collisionLine.linePoints[0].y, collisionLine.linePoints[1].x, collisionLine.linePoints[1].y, 0xFF00FF00);
          if(collision2dLineCircle(playerLine, collisionCircle)) {
               graphics2dDrawEllipse((collisionCircle.circCenter.x - collisionCircle.circRadius), (collisionCircle.circCenter.y - collisionCircle.circRadius), (collisionCircle.circRadius * 2.0f), (collisionCircle.circRadius * 2.0f), 0xFF0000FF);
               graphics2dDrawEllipse((collisionLast.x - 4), (collisionLast.y - 4), 8, 8, 0xFF00FFFF);
          } else
               graphics2dDrawEllipse((collisionCircle.circCenter.x - collisionCircle.circRadius), (collisionCircle.circCenter.y - collisionCircle.circRadius), (collisionCircle.circRadius * 2.0f), (collisionCircle.circRadius * 2.0f), 0xFF00FF00);
          graphics2dDrawLine(playerLine.linePoints[0].x, playerLine.linePoints[0].y, playerLine.linePoints[1].x, playerLine.linePoints[1].y, 0xFFFFFFFF);
          
          graphicsSwapBuffers();

          inputPoll();
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
               debugScreenshot();          
          if(inputCheck(INPUT_BUTTON_CROSS) == INPUT_JUST_DOWN) {
               playerLine.linePoints[1].x = playerLine.linePoints[0].x + (float)(mathRandi(65) - 32);
               playerLine.linePoints[1].y = playerLine.linePoints[0].y + (float)(mathRandi(65) - 32);
     
               collisionLine.linePoints[1].x = collisionLine.linePoints[0].x + (float)(mathRandi(65) - 32);
               collisionLine.linePoints[1].y = collisionLine.linePoints[0].y + (float)(mathRandi(65) - 32);
               
               collisionCircle.circRadius = (float)mathRandi(33);
          }


          gameTick += timerGetDeltaTime(gameTimer);
          while(gameTick > gameTickRate) {
               gameTick -= gameTickRate;
               if(inputDown(INPUT_DPAD_LEFT)) {
                    playerLine.linePoints[0].x--;
                    playerLine.linePoints[1].x--;
               } else if(inputDown(INPUT_DPAD_RIGHT)) {
                    playerLine.linePoints[0].x++;
                    playerLine.linePoints[1].x++;
               }
               if(inputDown(INPUT_DPAD_UP)) {
                    playerLine.linePoints[0].y--;
                    playerLine.linePoints[1].y--;
               } else if(inputDown(INPUT_DPAD_DOWN)) {
                    playerLine.linePoints[0].y++;
                    playerLine.linePoints[1].y++;
               }
          }
     }

     
     flTerminate();
     return 0;
}
