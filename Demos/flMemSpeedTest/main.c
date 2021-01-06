#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspkernel.h>
#include <string.h>
#include <stdio.h>

PSP_MODULE_INFO("flMemSpeedTest", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_FUNCLIB);

int main() {
     flInitialize();

     u8* tempRam = (void*)((u32)memAlloc(16384) | 0x40000000);
     u8* tempVRam = (void*)((u32)vmemAlloc(16384) | 0x40000000);
     u8* tempSPad = (void*)(MEMORY_SCRATCHPAD_BASE | 0x40000000);

     Timer* tempTimer = timerCreate();
     float tempRamTime[2];
     float tempVRamTime[2];
     float tempSPadTime[2];
     
     float tempMemsetTime[2];
     float tempMemcpyTime[2];
     float tempMemclrTime[2];
     
     float tempMallocTime[3];
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memSetByte(tempRam, 0x0F, 16384);
     tempRamTime[0] = timerGetDeltaTime(tempTimer);
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memSetByte(tempVRam, 0x0F, 16384);
     tempVRamTime[0] = timerGetDeltaTime(tempTimer);
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memSetByte(tempSPad, 0x0F, 16384);
     tempSPadTime[0] = timerGetDeltaTime(tempTimer);
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memCopy(tempRam, (void*)((u32)tempRam + 8192), 8192);
     tempRamTime[1] = timerGetDeltaTime(tempTimer) * 2.0f;
     tempRamTime[1] -= tempRamTime[0];
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memCopy(tempVRam, (void*)((u32)tempVRam + 8192), 8192);
     tempVRamTime[1] = timerGetDeltaTime(tempTimer) * 2.0f;
     tempVRamTime[1] -= tempVRamTime[0];
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memCopy(tempSPad, (void*)((u32)tempSPad + 8192), 8192);
     tempSPadTime[1] = timerGetDeltaTime(tempTimer) * 2.0f;
     tempSPadTime[1] -= tempSPadTime[0];
     
     
     /* Disguarded for the above which I assume to be more accurate
        Although the results from below are roughly the same as for above
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memCopy(tempRam, (void*)(MEMORY_USER_BASE | 0x40000000), 16384);
     tempRamTime[1] = timerGetDeltaTime(tempTimer);
     tempRamTime[1] -= tempRamTime[0];
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memCopy(tempRam, tempVRam, 16384);
     tempVRamTime[1] = timerGetDeltaTime(tempTimer);
     tempVRamTime[1] -= tempRamTime[0];
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memCopy(tempRam, tempSPad, 16384);
     tempSPadTime[1] = timerGetDeltaTime(tempTimer);
     tempSPadTime[1] -= tempRamTime[0];*/
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memSetByte(tempRam, 0x0F, 16384);
     tempMemsetTime[0] = timerGetDeltaTime(tempTimer);
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memset(tempRam, 0x0F, 16384);
     tempMemsetTime[1] = timerGetDeltaTime(tempTimer);
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memClear(tempRam, 16384);
     tempMemclrTime[0] = timerGetDeltaTime(tempTimer);
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memset(tempRam, 0x00, 16384);
     tempMemclrTime[1] = timerGetDeltaTime(tempTimer);
     
     u8* tempRam2 = (void*)((u32)memAlloc(16384) | 0x40000000);
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memCopy(tempRam, tempRam2, 16384);
     tempMemcpyTime[0] = timerGetDeltaTime(tempTimer);
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memcpy(tempRam, tempRam2, 16384);
     tempMemcpyTime[1] = timerGetDeltaTime(tempTimer);
     
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     memAlloc(16384);
     tempMallocTime[0] = timerGetDeltaTime(tempTimer);
     timerGetDeltaTime(tempTimer);
     memQalloc(16384);
     tempMallocTime[1] = timerGetDeltaTime(tempTimer);
     timerGetDeltaTime(tempTimer);
     vmemAlloc(16384);
     tempMallocTime[2] = timerGetDeltaTime(tempTimer);


     char tempString[256];
     u32 tempY;
     while(flRunning) {
          tempY = 12;
          sprintf(tempString, "Ram read/write time:        %f / %f", tempRamTime[1], tempRamTime[0]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "VRam read/write time:       %f / %f", tempVRamTime[1], tempVRamTime[0]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "Scratchpad read/write time: %f / %f", tempSPadTime[1], tempSPadTime[0]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          tempY += 12;
          sprintf(tempString, "memSet/memset time:    %f / %f", tempMemsetTime[0], tempMemsetTime[1]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "memCopy/memcpy time:   %f / %f", tempMemcpyTime[0], tempMemcpyTime[1]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "memClear/memset time:  %f / %f", tempMemclrTime[0], tempMemclrTime[1]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          tempY += 12;
          sprintf(tempString, "memAlloc time:   %f", tempMallocTime[0]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "memQalloc time:  %f", tempMallocTime[1]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "vmemAlloc time:  %f", tempMallocTime[2]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          tempY += 12;
          char* tempMemString = memString(memFreeSpaceTotal(), true);
          sprintf(tempString, "Free Memory:  %s", tempMemString);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          memFree(tempMemString);
          tempY += 12;
          graphicsSwapBuffers();
          
          inputPoll();
          if(inputCheck(INPUT_BUTTON_START) == INPUT_JUST_DOWN)
               flTerminate();
          if(inputCheck(INPUT_BUTTON_SELECT) == INPUT_JUST_DOWN)
               debugScreenshot();
          if(inputCheck(INPUT_BUTTON_LTRIGGER) == INPUT_JUST_DOWN)
               graphicsFPSShow = !graphicsFPSShow;
     }

     
     flTerminate();
     return 0;
}
