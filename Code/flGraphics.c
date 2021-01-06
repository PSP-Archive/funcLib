#include "flGlobal.h"
#if FL_GRAPHICS != 0
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdisplay.h>

#if FL_FONT != 0
#include <stdio.h>
#endif

#if FL_INCLUDE_ALL_C == 0
#include "flGraphics.h"
#include "flMemory.h"
#include "flDebug.h"
#include "flTimer.h"
#include "flFont.h"
#include "flNotify.h"
#include "flTexture.h"
#endif

#define PSP_DISPLAY_PIXEL_FORMAT(inPixFormat) ((inPixFormat) == GU_PSM_4444 ? PSP_DISPLAY_PIXEL_FORMAT_4444 : ((inPixFormat) == GU_PSM_5650 ? PSP_DISPLAY_PIXEL_FORMAT_565 : ((inPixFormat) == GU_PSM_5551 ? PSP_DISPLAY_PIXEL_FORMAT_5551 : PSP_DISPLAY_PIXEL_FORMAT_8888)))

RenderTarget __attribute__((aligned(32))) graphicsRenderTargetStack[FL_GRAPHICS_RENDERTARGET_STACK_SIZE];
u32                                       graphicsRenderTargetStackPointer = 0;

bool  graphicsVSync = FL_GRAPHICS_VSYNC_DEFAULT;
#if FL_GRAPHICS_DISPLAYLIST_STATIC
u32 __attribute__((aligned(64))) graphicsDisplayList[((FL_GRAPHICS_DISPLAYLIST_SIZE + 63) & ~63) << 1];
u32 __attribute__((aligned(64))) graphicsDisplayListCache[4096];
#else
void* graphicsDisplayList = NULL;
void* graphicsDisplayListCache = NULL;
#endif
u8    graphicsDisplayListCurrent = 0;

void* graphicsDrawBuffer = NULL;
void* graphicsDispBuffer = NULL;
void* graphicsDepthBuffer = NULL;

bool graphicsLinesAntiAlias = FL_GRAPHICS_LINES_ANTIALIAS_DEFAULT;
bool graphicsWireframe = false;

#if FL_TIMER
Timer* graphicsFPSTimer = NULL;
float  graphicsFPS = 0;
bool   graphicsFPSShow = FL_GRAPHICS_FPS_SHOW_DEFAULT;
#endif

bool  graphicsFrame = false;
bool  graphicsOrtho = false;

void* graphicsDispListCur() {
     if(!(graphicsDisplayListCurrent & 1))
          return graphicsDisplayList;
     u32 tempOut = ((FL_GRAPHICS_DISPLAYLIST_SIZE + 63) & ~63);
     tempOut += (u32)graphicsDisplayList;
     return (void*)tempOut;
}

void* graphicsDispListLast() {
     if(graphicsDisplayListCurrent & 1)
          return graphicsDisplayList;
     u32 tempOut = ((FL_GRAPHICS_DISPLAYLIST_SIZE + 63) & ~63);
     tempOut += (u32)graphicsDisplayList;
     return (void*)tempOut;
}

void graphicsInit() {
     u32 tempDListSize = (((FL_GRAPHICS_DISPLAYLIST_SIZE + 63) & ~63) << 1);
     #if FL_GRAPHICS_DISPLAYLIST_STATIC == 0
     graphicsDisplayList = memAlign(64, tempDListSize);
     graphicsDisplayListCache = memAlign(64, 4096);
     if(!graphicsDisplayList) {
          #if FL_DEBUG_ERROR != 0
          debugError("Couldn't allocate display list, graphics initialization error.\n");
          #endif
          return;
     }
     #endif

     #if FL_VSHMODE == 0
     #if FL_MEMORY_VMEM != 0
     
     graphicsDrawBuffer = vmemAllocBuffer(SCREEN_PITCH, SCREEN_HEIGHT, graphicsPixelFormatBPP(FL_GRAPHICS_DRAW_BUFFER_FORMAT), (SCREEN_STRIDE - SCREEN_WIDTH));
     #if FL_GRAPHICS_DISP_BUFFER_VRAM
     graphicsDispBuffer = vmemAllocBuffer(SCREEN_PITCH, SCREEN_HEIGHT, graphicsPixelFormatBPP(FL_GRAPHICS_DISP_BUFFER_FORMAT), (SCREEN_STRIDE - SCREEN_WIDTH));
     #else
     graphicsDispBuffer = memAlign(64, ((SCREEN_PITCH * SCREEN_HEIGHT * graphicsPixelFormatBPP(FL_GRAPHICS_DRAW_BUFFER_FORMAT)) >> 3));
     #endif
     graphicsDepthBuffer = vmemAllocBuffer(SCREEN_PITCH, SCREEN_HEIGHT, 16, (SCREEN_STRIDE - SCREEN_WIDTH));
     vmemInit();
     #else
     graphicsDrawBuffer  = (void*)0x0;
     graphicsDispBuffer  = (void*)graphicsPixelFormatBPP(FL_GRAPHICS_DRAW_BUFFER_FORMAT);
     graphicsDepthBuffer = (void*)(graphicsPixelFormatBPP(FL_GRAPHICS_DRAW_BUFFER_FORMAT) + graphicsPixelFormatBPP(FL_GRAPHICS_DISP_BUFFER_FORMAT));
     #endif
     graphicsRenderTargetStack[0].rtPointer = graphicsDrawBuffer;
     graphicsRenderTargetStack[0].rtBuffWidth = SCREEN_STRIDE;
     graphicsRenderTargetStack[0].rtPixelFormat = FL_GRAPHICS_DRAW_BUFFER_FORMAT;
     graphicsRenderTargetStack[0].rtWidth = SCREEN_WIDTH;
     graphicsRenderTargetStack[0].rtHeight = SCREEN_HEIGHT;
     graphicsRenderTargetStack[0].rtDepthBuff = graphicsDepthBuffer;
     graphicsRenderTargetStack[0].rtFOV = 45.0f;
     graphicsRenderTargetStack[0].rtUsed = true;
     graphicsRenderTargetStack[0].rtBuffer = 1;
     #else
     int tempBuffWidth, tempFormat, tempSync = 0;
     sceDisplayGetFrameBuf(&graphicsDrawBuffer, &tempBuffWidth, &tempFormat, tempSync);
     // TODO - Fix these values for VSHMODE.
     graphicsDispBuffer  = (void*)((u32)graphicsDrawBuffer + 0x88000);
     graphicsDepthBuffer = (void*)((u32)graphicsDrawBuffer + 0x110000);
     // TODO - Correctly calculate render target info.
     #endif
     
     #if FL_MEMORY != 0
     memClear(graphicsDisplayList, tempDListSize);
     memClear(graphicsDisplayListCache, 4096);
     #else
     u32* i = (u32*)graphicsDisplayList;
     while((u32)i < ((u32)graphicsDisplayList + tempDListSize))
          *(i++) = 0x00;
     i = (u32*)graphicsDisplayListCache;
     while((u32)i < ((u32)graphicsDisplayListCache + 4096))
          *(i++) = 0x00;
     #endif
     
     #if FL_VSHMODE == 0
     sceGuInit();
     sceGuStart(GU_DIRECT, graphicsDisplayList);
     
     sceGuDrawBuffer(FL_GRAPHICS_DRAW_BUFFER_FORMAT, graphicsDrawBuffer, SCREEN_STRIDE);
     sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, graphicsDispBuffer, SCREEN_STRIDE);
     sceGuDepthBuffer(graphicsDepthBuffer, SCREEN_STRIDE);

     sceGuOffset(2048 - (SCREEN_WIDTH >> 1), 2048 - (SCREEN_HEIGHT >> 1));
     sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
     sceGuDepthRange(65535, 0);

     sceGuAmbientColor(0xFFFFFFFF);
     
     sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
     sceGuEnable(GU_SCISSOR_TEST);
     
     sceGuDepthFunc(GU_GEQUAL);
     sceGuEnable(GU_DEPTH_TEST);
     
     sceGuFrontFace(GU_CW);
     sceGuEnable(GU_CULL_FACE);
     
     sceGuShadeModel(GU_SMOOTH);
     
     sceGuEnable(GU_CLIP_PLANES);
     
     sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
     sceGuEnable(GU_BLEND);
     
     sceGuTexEnvColor(0xFFFFFFFF);
     
     if(graphicsLinesAntiAlias)
          sceGuEnable(GU_LINE_SMOOTH);
     
     sceGuAlphaFunc(GU_EQUAL, 0x00000000, 0xFF000000);
     sceGuEnable(GU_ALPHA_TEST);
     
     sceGuEnable(GU_TEXTURE_2D);
     
     sceGuEnable(GU_DITHER);
     
     sceGuColor(0xFFFFFFFF);
     
     sceGuFinish();
     sceGuSync(GU_SYNC_WAIT, GU_SYNC_FINISH);
 
     sceDisplayWaitVblankStart();
     sceGuDisplay(GU_TRUE);
     #endif
     
     #if FL_TIMER != 0
     graphicsFPSTimer = timerCreate();
     #endif
     
     // Begin the call list for next frame.
     sceGuStart(GU_CALL, graphicsDispListCur());
     sceGuFinish();
     sceGuStart(GU_DIRECT, graphicsDisplayListCache);
     sceGuCallList(graphicsDispListCur());
     sceGuFinish();
     graphicsDisplayListCurrent ^= 1;
     
     sceGuStart(GU_CALL, graphicsDispListCur());
     sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_FAST_CLEAR_BIT);
     sceGumMatrixMode(GU_MODEL);
     sceGumLoadIdentity();
     #if FL_GRAPHICS_DRAW_BUFFER_FORMAT != FL_GRAPHICS_DISP_BUFFER_FORMAT
     sceGuDrawBufferList(FL_GRAPHICS_DRAW_BUFFER_FORMAT, (void*)graphicsDrawBuffer, SCREEN_STRIDE);
     #endif
     
     graphicsFrame = true;
     
     graphicsSetProjection(graphicsRenderTargetStack[0].rtFOV);
     
     sceDisplaySetFrameBuf((FL_GRAPHICS_DISP_BUFFER_VRAM ? (void*)((u32)graphicsDispBuffer | 0x04000000) : graphicsDispBuffer), SCREEN_STRIDE, PSP_DISPLAY_PIXEL_FORMAT(FL_GRAPHICS_DISP_BUFFER_FORMAT), PSP_DISPLAY_SETBUF_IMMEDIATE);
}

void graphicsSetProjection(float inFOV) {
     sceGumMatrixMode(GU_PROJECTION);
     sceGumLoadIdentity();
     sceGumPerspective(inFOV, ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT), 0.5f, 1000.0f);

     sceGumMatrixMode(GU_VIEW);
     sceGumLoadIdentity();
     
     sceGumMatrixMode(GU_MODEL);
     sceGumLoadIdentity();

     sceGuClearColor(0xFF000000); 
     sceGuClearDepth(0x00);
}

void graphicsTerm() {
     sceGuFinish();
     sceGuSync(GU_SYNC_WAIT, GU_SYNC_FINISH);
     
     #if !FL_GRAPHICS_DISPLAYLIST_STATIC
     if(graphicsDisplayList) {
          memFree(graphicsDisplayList);
          graphicsDisplayList = NULL;
     }
     if(graphicsDisplayListCache) {
          memFree(graphicsDisplayListCache);
          graphicsDisplayListCache = NULL;
     }
     #endif
        
     #if FL_MEMORY_VMEM
     vmemTerm();
     #endif
     
     graphicsDrawBuffer = NULL;
     graphicsDispBuffer = NULL;
     graphicsDepthBuffer = NULL;

     graphicsLinesAntiAlias = FL_GRAPHICS_LINES_ANTIALIAS_DEFAULT;
     graphicsWireframe = false;

     #if FL_TIMER
     graphicsFPSTimer = NULL;
     graphicsFPS = 0;
     graphicsFPSShow = FL_GRAPHICS_FPS_SHOW_DEFAULT;
     #endif

     graphicsFrame = false;
     graphicsOrtho = false;
     
     graphicsRenderTargetStackPointer = 0;
     
     sceDisplayWaitVblankStart();
     sceGuDisplay(GU_FALSE);
     sceGuTerm();
}

void graphicsSwapBuffers() {
     u32 tempRTSPtr = graphicsRenderTargetStackPointer;
     if(graphicsRenderTargetStackPointer) {
          graphicsRenderTargetStackPointer = 0;
          graphicsRenderTargetApply();
     }
     
     pspDebugScreenSetXY(0, 0);
     
     #if FL_TIMER
     #if FL_GRAPHICS_FPS_DAMP
     graphicsFPS = (FL_GRAPHICS_FPS_DAMPRATIO * (1 / timerGetDeltaTime(graphicsFPSTimer))) + ((1 - FL_GRAPHICS_FPS_DAMPRATIO) * graphicsFPS);
     #else
     graphicsFPS = (1 / timerGetDeltaTime(graphicsFPSTimer));
     #endif
     
     #if FL_NOTIFY
     ntfyTick();
     #endif
     
     if(graphicsFPSShow) {
          #if FL_FONT != 0
          char tempFPSString[16];
          sprintf(tempFPSString, "FPS: %4.2f", graphicsFPS);
          fontDraw2dString(0, 0, fontDefault, tempFPSString);
          #else
          pspDebugScreenSetTextColor(FL_GRAPHICS_FPS_SHOW_COLOR);
          pspDebugScreenSetBackColor(~FL_GRAPHICS_FPS_SHOW_COLOR);
          pspDebugScreenPrintf("FPS: %4.2f", graphicsFPS);
          #if FL_GRAPHICS_FPS_SHOW_COLOR != 0xFFFFFFFF
          pspDebugScreenSetTextColor(0xFFFFFFFF);
          pspDebugScreenSetBackColor(0x00000000);
          #endif
          #endif
     }
     #endif
     
     #if FL_GRAPHICS_VSYNC_AUTO != 0
     bool tempVSync = (graphicsVSync && !(graphicsFPS < FL_GRAPHICS_VSYNC_AUTO_CUTOFF));
     #else
     bool tempVSync = (graphicsVSync != 0);
     #endif
     

     #if FL_VSHMODE == 0
     sceGuSync(GU_SYNC_WAIT, GU_SYNC_FINISH);
     sceGuFinish();
     #if FL_GRAPHICS_DRAW_BUFFER_FORMAT == FL_GRAPHICS_DISP_BUFFER_FORMAT
     graphicsDispBuffer = graphicsDrawBuffer;
     if(tempVSync)
          sceDisplayWaitVblankStart();
     #if !FL_GRAPHICS_COPY_BUFFER
     graphicsDrawBuffer = sceGuSwapBuffers();
     pspDebugScreenSetOffset((int)graphicsDrawBuffer);
     #endif
     sceGuStart(GU_DIRECT, graphicsDisplayListCache);
     #if FL_GRAPHICS_COPY_BUFFER
     sceGuCopyImage(FL_GRAPHICS_DRAW_BUFFER_FORMAT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_STRIDE, (void*)((u32)graphicsDrawBuffer | 0x04000000), 0, 0, SCREEN_STRIDE, (FL_GRAPHICS_DISP_BUFFER_VRAM ? (void*)((u32)graphicsDispBuffer | 0x04000000) : graphicsDispBuffer));
     #endif
     sceGuCallList(graphicsDispListCur());
     sceGuFinish();
     #else
     vertTsVs* tempVerts;
     
     sceGuStart(GU_DIRECT, graphicsDisplayListCache);
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDepthMask(GU_TRUE);
     
     sceGuTexMode(FL_GRAPHICS_DRAW_BUFFER_FORMAT, 0, 0, GU_FALSE);
     sceGuTexImage(0, SCREEN_STRIDE, SCREEN_HEIGHT, SCREEN_STRIDE, (void*)graphicsDrawBuffer);
     sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
     sceGuTexFilter(GU_NEAREST, GU_NEAREST);
     
     tempVerts = sceGuGetMemory(sizeof(vertTsVs) << 1);
     tempVerts[0].vertX = 0;
     tempVerts[0].vertY = 0;
     tempVerts[0].vertZ = 0;
     tempVerts[0].vertU = 0;
     tempVerts[0].vertV = 0;
     tempVerts[1].vertX = SCREEN_WIDTH;
     tempVerts[1].vertY = SCREEN_HEIGHT;
     tempVerts[1].vertZ = 0;
     tempVerts[1].vertU = SCREEN_WIDTH;
     tempVerts[1].vertV = SCREEN_HEIGHT;
          
     sceGuDrawBufferList(FL_GRAPHICS_DISP_BUFFER_FORMAT, (void*)graphicsDispBuffer, SCREEN_STRIDE);
     
     if(tempVSync)
          sceDisplayWaitVblankStart();

     //sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, tempVertCount, 0, tempVerts);
     sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);
     
     sceGuDrawBufferList(FL_GRAPHICS_DRAW_BUFFER_FORMAT, (void*)graphicsDrawBuffer, SCREEN_STRIDE);
     sceGuEnable(GU_DEPTH_TEST);
     sceGuDepthMask(GU_FALSE);
     
     sceGuCallList(graphicsDispListCur());
     sceGuFinish();
     #if FL_TEXTURE
     texBound = NULL;
     #endif
     #endif
     
     graphicsDisplayListCurrent ^= 1;
     sceGuStart(GU_CALL, graphicsDispListCur());
     
     if(tempRTSPtr) {
          graphicsRenderTargetStackPointer = tempRTSPtr;
          graphicsRenderTargetApply();
     }
     
     sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_FAST_CLEAR_BIT);
     sceGumMatrixMode(GU_MODEL);
     sceGumLoadIdentity();
     #endif
}

void graphicsBeginOrtho() {
     if(graphicsOrtho)
          return;
     sceGumMatrixMode(GU_PROJECTION);
     sceGumPushMatrix();
     sceGumLoadIdentity();
     sceGumOrtho(0, 480, 272, 0, -1, 1);
     sceGumMatrixMode(GU_VIEW);
     sceGumPushMatrix();
     sceGumLoadIdentity();
     sceGumMatrixMode(GU_MODEL);
     sceGumLoadIdentity();
     graphicsOrtho = true;
}

void graphicsEndOrtho() {
     if(!graphicsOrtho)
          return;
     sceGumMatrixMode(GU_VIEW);
     sceGumPopMatrix();
     sceGumMatrixMode(GU_PROJECTION);
     sceGumPopMatrix();
     sceGumMatrixMode(GU_MODEL);
     sceGumLoadIdentity();
     graphicsOrtho = false;
}

void graphicsSetLinesAntiAlias(bool inValue) {
     if(graphicsLinesAntiAlias == inValue)
          return;
     if(inValue)
          sceGuEnable(GU_LINE_SMOOTH);
     else
          sceGuDisable(GU_LINE_SMOOTH);
     graphicsLinesAntiAlias = inValue;
}

/*bool graphicsRenderToVmem(void* inPtr, int inStride, u8 inPixelFormat) {
     if(!((unsigned int)inPtr & 0x04000000)) {
          #if DEBUG_ERROR != 0
          debugError("Render target invalid, must be in VRam.");
          #endif
          return false;
     }
     graphicsRenderTarget = (void*)((unsigned int)inPtr & 0xBBFFFFFF);
     sceGuDrawBufferList(inPixelFormat, graphicsRenderTarget, inStride);
     graphicsRenderTargetStride = inStride;
     graphicsRenderTargetPixelFormat = inPixelFormat;
     return true;
}*/

void graphicsRenderTargetPush(RenderTarget* inTarget) {
     if(graphicsRenderTargetStackPointer >= 7) {
          return;
     }
     graphicsRenderTargetStack[++graphicsRenderTargetStackPointer] = *inTarget;
     graphicsRenderTargetStack[graphicsRenderTargetStackPointer].rtUsed = true;
     graphicsRenderTargetApply();
}

RenderTarget* graphicsRenderTargetPop() {
     if(!graphicsRenderTargetStackPointer) {
          return NULL;
     }
     graphicsRenderTargetStack[graphicsRenderTargetStackPointer].rtUsed = false;
     graphicsRenderTargetStackPointer--;
     graphicsRenderTargetApply();
     return &graphicsRenderTargetStack[graphicsRenderTargetStackPointer];
}

void graphicsRenderTargetReset() {
     graphicsRenderTargetStackPointer = 0;
     u32 i;
     for(i = 1; i < FL_GRAPHICS_RENDERTARGET_STACK_SIZE; i++)
          graphicsRenderTargetStack[i].rtUsed = false;
}

void graphicsRenderTargetApply() {
     if(!graphicsRenderTargetStack[graphicsRenderTargetStackPointer].rtUsed)
          return;
     
     RenderTarget* tempRT = &graphicsRenderTargetStack[graphicsRenderTargetStackPointer];
     
     if(!tempRT->rtBuffer) {
          sceGuDrawBufferList(tempRT->rtPixelFormat, tempRT->rtPointer, tempRT->rtBuffWidth);
          sceGuOffset(2048 - (tempRT->rtWidth >> 1), 2048 - (tempRT->rtHeight >> 1));
          sceGuViewport(2048, 2048, tempRT->rtWidth, tempRT->rtHeight);
          sceGuScissor(0, 0, tempRT->rtWidth, tempRT->rtHeight);
     
          if(tempRT->rtDepthBuff) {
               sceGuDepthMask(GU_FALSE);
               sceGuDepthBuffer(tempRT->rtDepthBuff, tempRT->rtBuffWidth);
               sceGuEnable(GU_DEPTH_TEST);
          } else {
               sceGuDisable(GU_DEPTH_TEST);
               sceGuDepthMask(GU_TRUE);
          }
     }
     
     graphicsSetProjection(tempRT->rtFOV);
}



u8 graphicsPixelFormatBPP(u8 inPixelFormat) {
     switch(inPixelFormat) {
          case GU_PSM_8888:
          case GU_PSM_T32:
               return 32;
          case GU_PSM_4444:
          case GU_PSM_5650:
          case GU_PSM_5551:
          case GU_PSM_T16:
               return 16;
          case GU_PSM_DXT3:
          case GU_PSM_DXT5:
          case GU_PSM_T8:
               return 8;
          case GU_PSM_DXT1:
          case GU_PSM_T4:
               return 4;
     }
     return 0;
}

u8* graphicsSwizzleData(u8* inData, u32 inWidth, u32 inHeight, u8 inPixelFormat) {
     if(!inData) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Can't swizzle data from a NULL pointer.");
          #endif
          return NULL;
     }
     if(!inWidth || !inHeight) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Can't swizzle data with a dimension of 0.");
          #endif
          return NULL;
     }
     u32 tempWidth = ((inWidth * graphicsPixelFormatBPP(inPixelFormat)) >> 3);
     if((tempWidth & 15) || (inHeight & 7)) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Data must have a width in which is a multiple of 16 bytes (%u), and a height which is a multiple of 8  (%u).", (unsigned int)tempWidth, (unsigned int)inHeight);
          #endif
          return inData;
     }
     u32 tempSize = tempWidth * inHeight;
     
     u8* tempDest = (u8*)memQalloc(tempSize);
     if(!tempDest) {
          #if FL_DEBUG_ERROR != 0
          debugError("Not enough memory to perform swizzle.");
          #endif
          return NULL;
     }
     
     u32  tempBlockX, tempBlockY;
     u32  j;
 
     u32  tempWidthBlocks  = (tempWidth >> 4);
     u32  tempHeightBlocks = (inHeight >> 3);
 
     u32  tempSrcPitch     = ((tempWidth - 16) >> 2);
     u32  tempSrcRow       = (tempWidth << 3);

     u8*   tempYSrc        = inData;
     u32*  tempDestPtr     = (u32*)tempDest;
 
     u8*  tempXSrc;
     u32* tempSrc;
 
     for(tempBlockY = 0; tempBlockY < tempHeightBlocks; ++tempBlockY) {
          tempXSrc = tempYSrc;
          for(tempBlockX = 0; tempBlockX < tempWidthBlocks; ++tempBlockX) {
               tempSrc = (u32*)tempXSrc;
               for(j = 0; j < 8; ++j) {
                    *(tempDestPtr++) = *(tempSrc++);
                    *(tempDestPtr++) = *(tempSrc++);
                    *(tempDestPtr++) = *(tempSrc++);
                    *(tempDestPtr++) = *(tempSrc++);
                    tempSrc += tempSrcPitch;
               }
               tempXSrc += 16;
          }
          tempYSrc += tempSrcRow;
     }

     memCopy(inData, tempDest, tempSize);
     memFree(tempDest);

     if(memIsCachedPointer(inData)) {
          if(tempSize >= 16384)
               sceKernelDcacheWritebackAll();
          else
               sceKernelDcacheWritebackRange(inData, tempSize);
     }
     return inData;
}

u8* graphicsUnswizzleData(u8* inData, u32 inWidth, u32 inHeight, u8 inPixelFormat) {
     if(!inData) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Can't unswizzle data from a NULL pointer.");
          #endif
          return NULL;
     }
     if(!inWidth || !inHeight) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Can't unswizzle data with a dimension of 0.");
          #endif
          return NULL;
     }
     u32 tempWidth = ((inWidth * graphicsPixelFormatBPP(inPixelFormat)) >> 3);  
     if((tempWidth & 15) || (inHeight & 7)) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Data must have a width in which is a multiple of 16 bytes (%u), and a height which is a multiple of 8  (%u).", (unsigned int)tempWidth, (unsigned int)inHeight);
          #endif
          return inData;
     }
     u32 tempSize = (tempWidth * inHeight);
     
     u8* tempDest = (u8*)memQalloc(tempSize);
     if(!tempDest) {
          #if FL_DEBUG_ERROR != 0
          debugError("Not enough memory to perform unswizzle.");
          #endif
          return NULL;
     }
     
     u32  tempBlockX, tempBlockY;
     u32  j;
 
     u32  tempWidthBlocks  = (tempWidth >> 4);
     u32  tempHeightBlocks = (inHeight >> 3);
 
     u32  tempDestPitch    = ((tempWidth - 16) >> 2);
     u32  tempDestRow      = (tempWidth << 3);

     u32* tempSrc          = (u32*)inData;
     u8*  tempYDest        = (u8*)tempDest;
 
     u8*  tempXDest;
     u32* tempDestPtr;
 
     for(tempBlockY = 0; tempBlockY < tempHeightBlocks; ++tempBlockY) {
          tempXDest = tempYDest;
          for(tempBlockX = 0; tempBlockX < tempWidthBlocks; ++tempBlockX) {
               tempDestPtr = (u32*)tempXDest;
               for(j = 0; j < 8; ++j) {
                    *(tempDestPtr++) = *(tempSrc++);
                    *(tempDestPtr++) = *(tempSrc++);
                    *(tempDestPtr++) = *(tempSrc++);
                    *(tempDestPtr++) = *(tempSrc++);
                    tempDestPtr += tempDestPitch;
               }
               tempXDest += 16;
          }
          tempYDest += tempDestRow;
     }
   
     memCopy(inData, tempDest, tempSize);
     memFree(tempDest);

     if(memIsCachedPointer(inData)) {
          if(tempSize >= 16384)
               sceKernelDcacheWritebackAll();
          else
               sceKernelDcacheWritebackRange(inData, tempSize);
     }
     
     return inData;
}

#endif
