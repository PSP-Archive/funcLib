#include "././Code/funcLib.h"
#include <pspgu.h>
#include <pspkernel.h>
#include <string.h>
#include <stdio.h>

PSP_MODULE_INFO("flMemAlignment", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_FUNCLIB);

/*typedef struct {
     u32 uData[4];
} u128;

typedef struct {
     u32 uData[8];
} u256;*/

typedef struct {
     u32 uData[16];
} u512;

typedef struct {
     u32 uData[32];
} u1024;

int main() {
     flInitialize();

     u8* tempMemLo = (void*)((u32)memAlign(64, 32768) | 0x40000000);
     u8* tempMemHi = (u8*)((u32)tempMemLo + 16384);

     Timer* tempTimer = timerCreate();
     float tempCopyTime[8];
     
     u8* tempDataSrc8 = (u8*)tempMemLo;
     u8* tempDataDest8 = (u8*)tempMemHi;
     u8* tempDataEnd8 = tempDataDest8;
     u16* tempDataSrc16 = (u16*)tempMemLo;
     u16* tempDataDest16 = (u16*)tempMemHi;
     u16* tempDataEnd16 = tempDataDest16;
     u32* tempDataSrc32 = (u32*)tempMemLo;
     u32* tempDataDest32 = (u32*)tempMemHi;
     u32* tempDataEnd32 = tempDataDest32;
     u64* tempDataSrc64 = (u64*)tempMemLo;
     u64* tempDataDest64 = (u64*)tempMemHi;
     u64* tempDataEnd64 = tempDataDest64;
     u128* tempDataSrc128 = (u128*)tempMemLo;
     u128* tempDataDest128 = (u128*)tempMemHi;
     u128* tempDataEnd128 = tempDataDest128;
     u256* tempDataSrc256 = (u256*)tempMemLo;
     u256* tempDataDest256 = (u256*)tempMemHi;
     u256* tempDataEnd256 = tempDataDest256;
     u512* tempDataSrc512 = (u512*)tempMemLo;
     u512* tempDataDest512 = (u512*)tempMemHi;
     u512* tempDataEnd512 = tempDataDest512;
     u1024* tempDataSrc1024 = (u1024*)tempMemLo;
     u1024* tempDataDest1024 = (u1024*)tempMemHi;
     u1024* tempDataEnd1024 = tempDataDest1024;
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     while(tempDataSrc8 < tempDataEnd8)
          *(tempDataDest8++) = *(tempDataSrc8++);
     tempCopyTime[0] = timerGetDeltaTime(tempTimer);

     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     while(tempDataSrc16 < tempDataEnd16)
          *(tempDataDest16++) = *(tempDataSrc16++);
     tempCopyTime[1] = timerGetDeltaTime(tempTimer);
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     while(tempDataSrc32 < tempDataEnd32)
          *(tempDataDest32++) = *(tempDataSrc32++);
     tempCopyTime[2] = timerGetDeltaTime(tempTimer);

     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     while(tempDataSrc64 < tempDataEnd64)
          *(tempDataDest64++) = *(tempDataSrc64++);
     tempCopyTime[3] = timerGetDeltaTime(tempTimer);
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     while(tempDataSrc128 < tempDataEnd128)
          *(tempDataDest128++) = *(tempDataSrc128++);
     tempCopyTime[4] = timerGetDeltaTime(tempTimer);

     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     while(tempDataSrc256 < tempDataEnd256)
          *(tempDataDest256++) = *(tempDataSrc256++);
     tempCopyTime[5] = timerGetDeltaTime(tempTimer);
     
     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     while(tempDataSrc512 < tempDataEnd512)
          *(tempDataDest512++) = *(tempDataSrc512++);
     tempCopyTime[6] = timerGetDeltaTime(tempTimer);

     sceKernelDcacheWritebackInvalidateAll();
     timerGetDeltaTime(tempTimer);
     while(tempDataSrc1024 < tempDataEnd1024)
          *(tempDataDest1024++) = *(tempDataSrc1024++);
     tempCopyTime[7] = timerGetDeltaTime(tempTimer);


     char tempString[256];
     u32 tempY;
     while(flRunning) {
          tempY = 12;
          sprintf(tempString, "8 bit copy time:        %f", tempCopyTime[0]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "16 bit copy time:        %f", tempCopyTime[1]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "32 bit copy time:        %f", tempCopyTime[2]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "64 bit copy time:        %f", tempCopyTime[3]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "128 bit copy time:        %f", tempCopyTime[4]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "256 bit copy time:        %f", tempCopyTime[5]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "512 bit copy time:        %f", tempCopyTime[6]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
          sprintf(tempString, "1024 bit copy time:        %f", tempCopyTime[7]);
          fontDraw2dString(4, tempY, fontDefault, tempString); tempY += 12;
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
