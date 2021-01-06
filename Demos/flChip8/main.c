#include "././Code/funcLib.h"
#include "emulator.h"

PSP_MODULE_INFO("flChip8", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

Timer*        emuTimer     = NULL;
Font*         emuFont      = NULL;

float         emuTick      = 0.0f;
float         emuTickTimer = 0.0f;

unsigned char emuKeyMap[16];

bool          emuShowDebug = false;

int main() {
    flInitialize();
    
	emuReset();
	
	//emuProgramLoad("./Games/PONG");
	//emuProgramLoad("./Games/PONG2");
	//emuProgramLoad("./Games/MISSILE");
	emuProgramLoad("./Games/INVADERS");
	//emuProgramLoad("./Games/MAZE");
	//emuProgramLoad("./Games/MERLIN");
	//emuProgramLoad("./Games/BREAKOUT");
	//emuProgramLoad("./Games/BRIX");
	//emuProgramLoad("./Games/TETRIS");
	//emuProgramLoad("./Games/SYZYGY");
	//emuProgramLoad("./Games/BLITZ");
	//emuProgramLoad("./Games/BLINKY");
	//emuProgramLoad("./Games/TANK");
	//emuProgramLoad("./Games/UFO");
	//emuProgramLoad("./Games/SQUASH");
	//emuProgramLoad("./Games/TICTAC");
	//emuProgramLoad("./Games/VERS");
	//emuProgramLoad("./Games/WIPEOFF");
	//emuProgramLoad("./Games/CONNECT4");
	//emuProgramLoad("./Games/HIDDEN");
	//emuProgramLoad("./Games/GUESS");
	//emuProgramLoad("./Games/WALL");

    emuKeyMap[4] = INPUT_DPAD_LEFT;
    emuKeyMap[6] = INPUT_DPAD_RIGHT;
    emuKeyMap[5] = INPUT_BUTTON_CROSS;

    emuFont  = fontCreateDebug(0, 0, 0xFFFFFFFF);
	emuTimer = timerCreate();

    while(flRunning) {
        emuDisplayRender();
        if(emuShowDebug)
             emuDebugInfo();
        graphicsSwapBuffers();

        inputPoll();
        int i;
        for(i = 0; i < 16; i++) {
            if(inputCheck(emuKeyMap[i]) == INPUT_JUST_DOWN)
                emuInputButtonDown(i);
            if(inputCheck(emuKeyMap[i]) == INPUT_JUST_UP)
                emuInputButtonUp(i);
        }
        if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN) {
             debugScreenshot();
             emuShowDebug = !emuShowDebug;
        }

        float tempTick = timerGetDeltaTime(emuTimer);
        emuTick += tempTick;
        emuTickTimer += tempTick;
    
        while(emuTickTimer >= emuTimerFrequency) {
             emuTickTimer -= emuTimerFrequency;
             emuTimerTick();
        }
        while(emuTick >= emuFrequency) {
             emuTick -= emuFrequency;
             if(emuKeyboardWaiting > 15)
                  emuExecSingle();
        }
    }


    flTerminate();
    return 0;
}
