#include "././Code/funcLib.h"
#include "anyTheme.c"

PSP_MODULE_INFO("OMGFlashzorzzz", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

int main() {
     flInitialize();
     
     Timer* gameTimer = timerCreate();
     
     char tempPath[256];
     sprintf(tempPath, "mem:/%ui:%i.tga", (unsigned int)anyTheme, size_anyTheme);
     //sprintf(tempPath, "./AnyTheme.bmp");
     Texture* gameFlashTex = texLoad(tempPath);
    
     float gameFlashTime = 0.0f;
     bool gameShowFlash = 0;
    
     while(flRunning) {
     
          if(gameShowFlash)
               graphics2dDrawTexture(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, gameFlashTex);
          
          graphicsSwapBuffers();
          
          gameFlashTime += timerGetDeltaTime(gameTimer);
          while(gameFlashTime > 0.2f) {
               gameFlashTime -= 0.2f;
               gameShowFlash = !gameShowFlash;
          }
     }

     
     flTerminate();
     return 0;
}
