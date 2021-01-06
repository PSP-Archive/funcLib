#include "funclib.h"
#include <pspdebug.h>
#include <pspfpu.h>

#if !FL_INCLUDE_ALL_C
#include "flCallbacks.h"
#include "flDebug.h"
#include "flMemory.h"
#include "flGraphics.h"
#include "flInput.h"
#include "flTexture.h"
#include "flFont.h"
#include "flMath.h"
#include "flModel.h"
#include "flNotify.h"
#endif

bool flRunning = false;

void flInitialize() {
     flRunning = true;
     
     pspfpu_set_enable(0);
     pspDebugScreenInit();
     
     #if FL_DEBUG
     debugInit();
     #if FL_DEBUG_STATUS
     debugStatusLog("FuncLib Initializing.");
     #endif
     #endif
     
     #if FL_CALLBACKS
     callbackInit();
     #if FL_DEBUG_STATUS
     debugStatusLog("Callbacks Initialized.");
     #endif
     #endif
     
     #if FL_MATH
     mathInit();
     #if FL_DEBUG_STATUS
     debugStatusLog("Math Initialized.");
     #endif
     #endif
     
     #if FL_MEMORY
     memInit();
     #if FL_DEBUG_STATUS
     debugStatusLog("Memory Initialized.");
     #endif
     #endif
     
     #if FL_GRAPHICS
     graphicsInit();
     #if FL_DEBUG_STATUS
     debugStatusLog("Graphics Initialized.");
     #endif
     #endif
     
     #if FL_TEXTURE
     texInit();
     #if FL_DEBUG_STATUS
     debugStatusLog("Texture Initialized.");
     #endif
     #endif
     
     #if FL_INPUT
     inputInit();
     #if FL_DEBUG_STATUS
     debugStatusLog("Input Initialized.");
     #endif
     #endif
     
     #if FL_FONT
     fontInit();
     #if FL_DEBUG_STATUS
     debugStatusLog("Font Initialized.");
     #endif
     #endif
     
     #if FL_MODEL
     mdlInit();
     #if FL_DEBUG_STATUS
     debugStatusLog("Model Initialized.");
     #endif
     #endif
     
     #if FL_NOTIFY
     ntfyInit();
     #if FL_DEBUG_STATUS
     debugStatusLog("Notify Initialized.");
     #endif
     #endif
     
     #if FL_DEBUG_STATUS
     debugStatusLog("FuncLib Initialized.");
     #endif
}

void flTerminate() {
     #if FL_DEBUG_STATUS
     debugStatusLog("FuncLib Terminating.");
     #endif
     
     #if FL_MEMORY
     memTerm();
     #endif
     
     #if FL_DEBUG_STATUS
     debugStatusLog("FuncLib Terminated.");
     #endif
     
     flRunning = false;
     sceKernelExitGame();
}

#if FL_INCLUDE_ALL_C
#include "flIncludeC.h"
#endif
