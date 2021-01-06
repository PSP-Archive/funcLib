// Funclib Graphics v1.0.0
// 
// This module contains functions for dealing with the graphics system.
// Use it to initialize graphics, change graphics modes, etc.
// 
// Contributor(s): Flatmush



#ifndef FLGRAPHICS_H
#define FLGRAPHICS_H

#include "flGlobal.h"

#if FL_GRAPHICS

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
     void* rtPointer;
     u32   rtBuffWidth;
     u32   rtPixelFormat;
     u32   rtWidth;
     u32   rtHeight;
     void* rtDepthBuff;
     float rtFOV;
     u16   rtReserved;
     u8    rtUsed;
     u8    rtBuffer; // 0 = none, 1 = draw, 2 = disp
} RenderTarget;

extern RenderTarget __attribute__((aligned(32))) graphicsRenderTargetStack[FL_GRAPHICS_RENDERTARGET_STACK_SIZE];
extern u32                                       graphicsRenderTargetStackPointer;

/**
  * This boolean stores whether VSync is currently activated or not, it defaults to GRAPHICS_VSYNC_DEFAULT
  * and can only be overridden if FL_GRAPHICS_VSYNC_AUTO is enabled.
  */
extern bool  graphicsVSync;

#if FL_GRAPHICS_DISPLAYLIST_STATIC
/**
  * Static display list memory, used as the main 2 display lists.
  */
extern u32 __attribute__((aligned(64))) graphicsDisplayList[((FL_GRAPHICS_DISPLAYLIST_SIZE + 63) & ~63) << 1];

/**
  * A small static display list, used for graphics using GU_DIRECT .
  */
extern u32 __attribute__((aligned(64))) graphicsDisplayListCache[4096];
#else

/**
  * Display list memory, used as the main 2 display lists.
  */
extern void* graphicsDisplayList;

/**
  * A small display list, used for graphics using GU_DIRECT .
  */
extern void* graphicsDisplayListCache;
#endif
/**
  * The current display list to be used.
  */
extern u8    graphicsDisplayListCurrent;

/**
  * Address of the draw buffer.
  */
extern void* graphicsDrawBuffer;

/**
  * Address of the display buffer.
  */
extern void* graphicsDispBuffer;

/**
  * Address of the Depth/Z buffer.
  */
extern void* graphicsDepthBuffer;



/**
  * Whether line antialiasing is currently enabled or not.
  */
extern bool graphicsLinesAntiAlias;

/**
  * Whether wireframe mode is enabled or not, this currently only effects the drawing of 3d primatives and models.
  * @warning This is not implemented completely yet (even within the 3d drawing routines) so use it sparingly.
  */
extern bool graphicsWireframe;



#if FL_TIMER
/**
  * The current fps, this is recalculated every frame so modifying it has almost no effect.
  */
extern float graphicsFPS;

/**
  * This variable stores whether the FPS display is enabled or not, it defaults to FL_GRAPHICS_FPS_SHOW_DEFAULT.
  */
extern bool  graphicsFPSShow;
#endif



/**
  * This stores whether the graphics system is currently mid-frame or not.
  * @note At the minute this is unused, but it was used in older versions and is planned to be used in the future.
  */
extern bool  graphicsFrame;

/**
  * The current render target address.
  */
extern void* graphicsRenderTarget;

/**
  * The current render target buffer width.
  */
extern int   graphicsRenderTargetStride;

/**
  * The current render target pixel format.
  */
extern u8    graphicsRenderTargetPixelFormat;



/**
  * Returns the address of the current display list, to be written to.
  * @return The address of the current display list.
  */
extern void* graphicsDispListCur();

/**
  * Returns the address of the last display list, to be drawn onto the screen.
  * @return The address of the last display list.
  */
extern void* graphicsDispListLast();


/**
  * Initializes the graphics system, this is called internally by flInitialize().
  * @see flInitialize(), graphicsTerm()
  */
extern void  graphicsInit();

/**
  * Terminates the graphics system, this is called internally by flTerminate().
  * @see flTerminate(), graphicsInit()
  */
extern void  graphicsTerm();

/**
  * Swaps the buffers and display lists around to show what you drew last frame,
  * this function is used as a launching off point for the notify functions and
  * other functions that need to be called each frame.
  * @see ntfyTick()
  */
extern void  graphicsSwapBuffers();



/**
  * Sets up the projection matrix for 3d.
  * @note This is currently called by graphicsInitialize() and every time the render target changes.
  * @param inFOV The field of view for the new projection matrix.
  * @see flInitialize()
  */
extern void  graphicsSetProjection(float inFOV);

/**
  * Begins orthographic drawing mode.
  * @warning This isn't working at the minute, but all 2d on the PSP uses GU_TRANSFORM_2D anyway.
  */
extern void  graphicsBeginOrtho();

/**
  * Ends orthographic drawing mode.
  * @warning This isn't working at the minute, but all 2d on the PSP uses GU_TRANSFORM_2D anyway.
  */
extern void  graphicsEndOrtho();

/**
  * This is basically a wrapper for sceGuEnable/Disable(GU_LINE_SMOOTH).
  * @param inValue Whether to enable/disable line anti-aliasing.
  */
extern void  graphicsSetLinesAntiAlias(bool inValue);

/**
  * This restores the render target to being the draw buffer again.
  */
//extern void graphicsRenderToFramebuffer();

/**
  * This sets the render target to an area in vmem.
  * @param inPtr Pointer to the start of the render target.
  * @param inStride The total width of the buffer including stride.
  * @param inPixelFormat The format of the render target using gu defines (e.g GU_PSM_8888).
  * @return Whether the target switch was successful or not.
  */
//extern bool graphicsRenderToVmem(void* inPtr, int inStride, u8 inPixelFormat);



extern void graphicsRenderTargetPush(RenderTarget* inTarget);
extern RenderTarget* graphicsRenderTargetPop();
extern void graphicsRenderTargetReset();
extern void graphicsRenderTargetApply();
     
     

/**
  * Gets the bits per pixel (BPP) of a pixel type.
  * @param inPixelFormat The format to get the BPP from, using gu defines (e.g GU_PSM_8888).
  * @return The BPP of the pixel format.
  */
extern u8   graphicsPixelFormatBPP(u8 inPixelFormat);

/**
  * Swizzles data so it is optimized for the PSP GPU.
  * @param inData A pointer to the data that is to be swizzled.
  * @param inWidth The width of the data in pixels.
  * @param inHeight The height of the data in pixels.  
  * @param inPixelFormat The pixel format of the data, using gu defines (e.g GU_PSM_8888).
  * @return The pointer to the swizzled data or NULL on failure.
  * @note This doesn't keep the original data, it simply swizzles the pointer you pass in.
  */
extern u8*  graphicsSwizzleData(u8* inData, u32 inWidth, u32 inHeight, u8 inPixelFormat);

/**
  * Unswizzles data so it is back in it's natural state ready for saving/modifying.
  * @param inData A pointer to the data that is to be unswizzled.
  * @param inWidth The width of the data in pixels.
  * @param inHeight The height of the data in pixels.  
  * @param inPixelFormat The pixel format of the data, using gu defines (e.g GU_PSM_8888).
  * @return The pointer to the unswizzled data or NULL on failure.
  * @note This doesn't keep the original data, it simply swizzles the pointer you pass in.
  */
extern u8*  graphicsUnswizzleData(u8* inData, u32 inWidth, u32 inHeight, u8 inPixelFormat);

#if FL_TEXTURE
/**
  * This sets the render target to a texture.
  * @param inTex The texture that will be rendered to.
  * @return Whether the target switch was successful or not.
  * @note As this is a macro it isn't type safe, ensure that inTex is a Texture*.
  */
#define graphicsRenderToTexture(inTex) graphicsRenderToVmem((inTex)->texData, (inTex)->texDataWidth, (inTex)->texPixelFormat)
#endif


#ifdef __cplusplus
}
#endif

#endif

#endif
