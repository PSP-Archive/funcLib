#include "flGlobal.h"
#if FL_NOTIFY != 0
#include <stdio.h>
#include <psprtc.h>
#include <pspkernel.h>
#include <psppower.h>
#include <pspimpose_driver.h>

#if FL_INCLUDE_ALL_C == 0
#include "flNotify.h"
#include "flMemory.h"
#include "flTimer.h"
#include "flDebug.h"
#include "flGraphics.h"
#include "flGraphics2d.h"
#include "flFont.h"
#endif

#if FL_KERNELMODE
int     ntfyVolLast = -1;
int     ntfyVolMuteLast = -1;
float   ntfyVolTime = 0.0f;
Timer*  ntfyVolTimer = NULL;
#endif

int     ntfyBattChrgLast = -1;
int     ntfyBattPercentLast = -1;
int     ntfyBattLowLast = -1;
float   ntfyBattTime = 0.0f;
Timer*  ntfyBattTimer = NULL;

pspTime ntfyTimeLast;
float   ntfyTimeTime = 0.0f;
Timer*  ntfyTimeTimer = NULL;

#if FL_NOTIFY_TRAYSIZE
NotifyTrayItem ntfyTray[FL_NOTIFY_TRAYSIZE];
Timer*         ntfyTrayTimer = NULL;
#endif

u32    ntfyTickCount = 0;
bool   ntfyInitialized = false;
u8     ntfyVisible = NOTIFY_VISIBLE_AUTO;


void ntfyInit() {
     sceRtcGetCurrentClockLocalTime(&ntfyTimeLast);

     #if FL_NOTIFY_TRAYSIZE
     u32 i;
     for(i = 0; i < FL_NOTIFY_TRAYSIZE; i++)
          ntfyTray[i].ntiTime = 0.0f;
     ntfyTrayTimer = timerCreate();
     #endif

     #if FL_KERNELMODE
     ntfyVolTimer = timerCreate();
     #endif
     
     ntfyBattTimer = timerCreate();
     
     ntfyTimeTimer = timerCreate();
     
     ntfyInitialized = true;
}

void ntfyTick() {
     if(!ntfyInitialized)
          return;
     
     ntfyTickCount++;
     if(ntfyTickCount > FL_NOTIFY_FREQUENCY) {
          float tempTick;                 
     
          #if FL_KERNELMODE
          tempTick = timerGetDeltaTime(ntfyVolTimer);
          if(ntfyVolTime > 0.0f) {
               ntfyVolTime -= tempTick;
               if(ntfyVolTime < 0.0f)
                    ntfyVolTime = 0.0f;
          }
          #if FL_NOTIFY_VOLUME
          ntfyCheckVolume();
          #endif
          #endif

          tempTick = timerGetDeltaTime(ntfyBattTimer);
          if(ntfyBattTime > 0.0f) {
               ntfyBattTime -= tempTick;
               if(ntfyBattTime < 0.0f)
                    ntfyBattTime = 0.0f;
          }
          #if FL_NOTIFY_BATTERY
          ntfyCheckBattery();
          #endif
          
          tempTick = timerGetDeltaTime(ntfyTimeTimer);
          if(ntfyTimeTime > 0.0f) {
               ntfyTimeTime -= tempTick;
               if(ntfyTimeTime < 0.0f)
                    ntfyTimeTime = 0.0f;
          }
          #if FL_NOTIFY_TIME
          ntfyCheckTime();
          #endif
          
          #if FL_NOTIFY_TRAYSIZE
          ntfyTrayTick();
          #endif
          
          ntfyTickCount = 0;
     }
     
     if(ntfyVisible == NOTIFY_VISIBLE_ALWAYS) {
          #if FL_NOTIFY_VOLUME
          ntfyShowVolume();
          #endif
          #if FL_NOTIFY_BATTERY
          ntfyShowBattery();
          #endif
          #if FL_NOTIFY_TIME
          ntfyShowTime();
          #endif
     } else {
          #if FL_NOTIFY_VOLUME
          if(ntfyVolTime > 0.0f)
               ntfyShowVolume();
          #endif
          #if FL_NOTIFY_BATTERY
          if(ntfyBattTime > 0.0f)
               ntfyShowBattery();
          #endif
          #if FL_NOTIFY_TIME
          if(ntfyTimeTime > 0.0f)
               ntfyShowTime();
          #endif
     }
     
     #if FL_NOTIFY_TRAYSIZE
     ntfyTrayShow();
     #endif
}

void ntfyShowAll(float inTime) {
     #if FL_NOTIFY_VOLUME
     ntfyShowVolume();
     ntfyVolTime = inTime;
     #endif
     #if FL_NOTIFY_BATTERY
     ntfyShowBattery();
     ntfyBattTime = inTime;
     #endif
     #if FL_NOTIFY_TIME
     ntfyShowTime();
     ntfyTimeTime = inTime;
     #endif
     #if FL_NOTIFY_TIME
     ntfyShowDate();
     #endif
}

#if FL_KERNELMODE
void ntfyCheckVolume() {
     if(!ntfyInitialized)
          return;
          
     int tempVol = sceImposeGetParam(PSP_IMPOSE_MAIN_VOLUME);
     int tempVolMute = sceImposeGetParam(PSP_IMPOSE_MUTE);

     if((tempVol >= 0) && (ntfyVolLast != tempVol)) {
          ntfyVolLast = tempVol;
          if((ntfyVolLast >= 0) && (ntfyVolTime < FL_NOTIFY_DURATION))
               ntfyVolTime = FL_NOTIFY_DURATION;
     }
     if((tempVolMute >= 0) && (ntfyVolMuteLast != tempVolMute)) {
          if((ntfyVolMuteLast >= 0) && (ntfyVolTime < FL_NOTIFY_DURATION))
               ntfyVolTime = FL_NOTIFY_DURATION;
          ntfyVolMuteLast = tempVolMute;
     }
}
#endif

void ntfyCheckBattery() {
     if(!ntfyInitialized)
          return;
          
     int tempBattLow = scePowerIsLowBattery();
     int tempCharge = scePowerIsPowerOnline();
     int tempPercent = scePowerGetBatteryLifePercent();
     
     if(tempBattLow > 0) {
          if(ntfyBattTime < FL_NOTIFY_DURATION)
               ntfyBattTime = FL_NOTIFY_DURATION;
          #if FL_DEBUG_STATUS
          if(ntfyBattLowLast != 0)
               debugStatusLog("Battery low.");
          #endif
          ntfyBattLowLast = tempBattLow;
     }
     
     if((tempCharge >= 0) && (tempCharge != ntfyBattChrgLast)) {
          if(ntfyBattChrgLast >= 0) {
               if(ntfyBattTime < FL_NOTIFY_DURATION)
                    ntfyBattTime = FL_NOTIFY_DURATION;
               #if FL_DEBUG_STATUS
               if(tempCharge > 0)
                   debugStatusLog("AC plugged in.");
               else
                   debugStatusLog("AC removed.");          
               #endif
          }
          ntfyBattChrgLast = tempCharge;
     }
     
     if((ntfyBattPercentLast >= 0) && (ntfyBattTime < FL_NOTIFY_DURATION)) {
          if((tempPercent < 75) && (ntfyBattPercentLast > 75))
               ntfyBattTime = FL_NOTIFY_DURATION;
          else if((tempPercent > 75) && (ntfyBattPercentLast < 75))
               ntfyBattTime = FL_NOTIFY_DURATION;
          else if((tempPercent < 50) && (ntfyBattPercentLast > 50))
               ntfyBattTime = FL_NOTIFY_DURATION;
          else if((tempPercent > 50) && (ntfyBattPercentLast < 50))
               ntfyBattTime = FL_NOTIFY_DURATION;
          else if((tempPercent < 25) && (ntfyBattPercentLast > 25))
               ntfyBattTime = FL_NOTIFY_DURATION;
          else if((tempPercent > 25) && (ntfyBattPercentLast < 25))
               ntfyBattTime = FL_NOTIFY_DURATION;
     }
     ntfyBattPercentLast = tempPercent;
}

void ntfyCheckTime() {
     if(!ntfyInitialized)
          return;
          
     pspTime tempTime;
     sceRtcGetCurrentClockLocalTime(&tempTime);
    
     if((tempTime.hour != ntfyTimeLast.hour) && (ntfyTimeTime < FL_NOTIFY_DURATION))
          ntfyTimeTime = FL_NOTIFY_DURATION;
     else if((ntfyTimeLast.minutes != tempTime.minutes) && (ntfyTimeTime < FL_NOTIFY_DURATION)) {
          if((tempTime.minutes == 15) || (tempTime.minutes == 30) || (tempTime.minutes == 45))
               ntfyTimeTime = FL_NOTIFY_DURATION;
     }

     ntfyTimeLast = tempTime;
}

#if FL_KERNELMODE
void ntfyShowVolume() {
     if(!ntfyVisible)
          return;
          
     int tempVol = sceImposeGetParam(PSP_IMPOSE_MAIN_VOLUME);
     int tempVolMute = sceImposeGetParam(PSP_IMPOSE_MUTE);
     if(tempVol < 0) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Error retrieving volume percentage.");
          #endif
          return;
     }
     graphics2dDrawRectFilled(24, (SCREEN_HEIGHT - 12), (SCREEN_WIDTH - 28), 8, 0x7F7F7F7F);
     if(tempVol > 0) {
          tempVol *= (SCREEN_WIDTH - 28);
          tempVol /= 30;
          graphics2dDrawRectFilled(24, (SCREEN_HEIGHT - 12), tempVol, 8, 0xFFFFFFFF);
     }
     
     if(tempVolMute > 0) {
          graphics2dDrawLine(8, (SCREEN_HEIGHT - 12), 16, (SCREEN_HEIGHT - 4), 0xFF0000FF);
          graphics2dDrawLine(16, (SCREEN_HEIGHT - 12), 8, (SCREEN_HEIGHT - 4), 0xFF0000FF);
     }
}
#endif

void ntfyShowBattery() {
     if(!ntfyVisible)
          return;

     int tempBatt = scePowerGetBatteryLifePercent();
     int tempLowBatt = scePowerIsLowBattery();
     int tempAC = scePowerIsPowerOnline();
     if(tempBatt < 0) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Error retrieving battery percentage.");
          #endif
          return;
     }
     if(tempAC < 0)
          tempAC = 0;
     
     u32 tempWidth = 32;
     u32 tempHeight = 12;
     u32 tempX = (SCREEN_WIDTH - (tempWidth + 4));
     u32 tempY = 4;
     
     graphics2dDrawRectFilled(tempX, tempY, tempWidth, tempHeight, 0x7F7F7F7F);
     graphics2dDrawRectFilled((tempX + 1), tempY, ((tempBatt * (tempWidth - 2)) / 100), tempHeight, (tempAC ? 0xFF00DFFF : (tempLowBatt > 0 ? 0x7F0000FF : 0x7F00FF00)));
     graphics2dDrawRect(tempX, tempY, tempWidth, tempHeight, 0xFFFFFFFF);
     graphics2dDrawLine((tempX + tempWidth), (tempY + 3), (tempX + tempWidth), ((tempY + tempHeight) - 3), 0xFFFFFFFF);
     
     #if FL_FONT
     char tempString[16];
     if(tempAC)
          sprintf(tempString, "AC");
     else
          sprintf(tempString, "%i", tempBatt);
     
     u32 tempFontWidth = fontStringWidth(fontDefault, tempString);
     u32 tempFontHeight = fontHeight(fontDefault);
     fontDraw2dString((tempX + ((tempWidth - tempFontWidth) >> 1)), (tempY + ((tempHeight - tempFontHeight) >> 1)), fontDefault, tempString);
     #endif
}

void ntfyShowTime() {
     if(!ntfyVisible)
          return;
          
     pspTime tempTime;
     sceRtcGetCurrentClockLocalTime(&tempTime);
     
     u32 tempWidth = 56;
     u32 tempHeight = 12;
     u32 tempX = (SCREEN_WIDTH - (tempWidth + 40));
     u32 tempY = 4;
     
     graphics2dDrawRectFilled((tempX + (tempHeight >> 1)), tempY, (tempWidth - (tempHeight >> 1)), tempHeight, 0xFF7F7F7F);
     graphics2dDrawEllipseFilled(tempX, tempY, tempHeight, tempHeight, 0xFF7F7F7F);
     graphics2dDrawEllipse(tempX, tempY, tempHeight, tempHeight, 0xFFFFFFFF);
     graphics2dDrawLine((tempX + (tempHeight >> 1)), (tempY + (tempHeight >> 1)), (tempX + (tempHeight >> 1)), tempY, 0xFFFFFFFF);
     graphics2dDrawLine((tempX + (tempHeight >> 1)), (tempY + (tempHeight >> 1)), (tempX + tempHeight), (tempY + (tempHeight >> 1)), 0xFFFFFFFF);
          
     #if FL_FONT
     char tempString[16];
     sprintf(tempString, "%02u:%02u", (unsigned int)tempTime.hour, (unsigned int)tempTime.minutes);
    
     u32 tempFontHeight = fontHeight(fontDefault);
     fontDraw2dString((tempX + tempHeight + 4), (tempY + ((tempHeight - tempFontHeight) >> 1)), fontDefault, tempString);
     #endif
}

void ntfyShowDate() {

}

#if FL_NOTIFY_TRAYSIZE
void ntfyTrayAdd(char* inMessage, u32 inColor) {
     if(!ntfyInitialized || !inMessage || !inMessage[0])
          return;
     
     int i;
     #if FL_NOTIFY_TRAYSIZE > 1
     for(i = (FL_NOTIFY_TRAYSIZE - 2); i >= 0 ; i--)
          ntfyTray[i + 1] = ntfyTray[i];
     #endif
     for(i = 0; i < 255; i++)
          ntfyTray[0].ntiMessage[i] = inMessage[i];
     ntfyTray[0].ntiMessage[255] = 0;
     ntfyTray[0].ntiColor = inColor;
     ntfyTray[0].ntiTime = (FL_NOTIFY_DURATION * 2.0f);
}

void ntfyTrayTick() {
     if(!ntfyInitialized)
         return;
     
     float tempTick = timerGetDeltaTime(ntfyTrayTimer);
     
     u32 i;
     for(i = 0; i < FL_NOTIFY_TRAYSIZE; i++) {
          if(ntfyTray[i].ntiTime > 0.0f)
               ntfyTray[i].ntiTime -= tempTick;
          if(ntfyTray[i].ntiTime < 0.0f)
               ntfyTray[i].ntiTime = 0.0f;
     }
}

void ntfyTrayShow() {
     if(!ntfyVisible)
          return;
     
     int tempY = SCREEN_HEIGHT;
     
     u32 i;
     for(i = 0; i < FL_NOTIFY_TRAYSIZE; i++) {
          tempY -= fontHeight(fontDefault);
          
          if((ntfyVisible != NOTIFY_VISIBLE_ALWAYS) && (ntfyTray[i].ntiTime <= 0.0f))
               continue;

          graphics2dDrawRectFilled(0, tempY, SCREEN_WIDTH, fontHeight(fontDefault), ((ntfyTray[i].ntiColor & 0x00FFFFFF) | 0xBF000000));
          fontDraw2dString(0, tempY, fontDefault, ntfyTray[i].ntiMessage);
     }
}
#endif

#endif
