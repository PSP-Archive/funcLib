// The following flags define the behaviour of funclib.
// These flags must coincide with the FLOBJS line of your makefile
// Don't worry, anything you leave out will be set to the default value,
// also the config file is validated at the end, so bad values will not stick.


// Global behaviour modifiers.

#define FL_INCLUDE_ALL_H                  1 ///< Include all enabled headers in funcLib.h so you only have to include one file in your source.
#define FL_INCLUDE_ALL_C                  1 ///< Includes all the c source files into one file, to be compiled as one object file (Adds a lot of optimization, the makefile needs to be changed for this).
#define FL_PREFIX                         0 ///< Enable prefixing of function/variable/struct names (e.g so colorRGB8888 would be flColorRGB8888).
#define FL_PREFIX_BOTH                    0 ///< Enable use of both prefixed and non-prefixed function/variable/struct names.
#define FL_VSHMODE                        0 ///< Enable/Disable VSH mode, used when writing vsh plugins.
#define FL_KERNELODE                      0 ///< Enable/Disable kernel mode features in modules.






// Core Modules - These modules are all optional, but we advise that you treat them as necessary,
//                this is because funclib is most effective when all the core modules are enabled.

#define FL_CALLBACKS                      1 ///< Include the callbacks module.
#define FL_CALLBACKS_EXIT                 1 ///< Include the exit callback (Enables the default home button behaviour).
#define FL_CALLBACKS_POWER                0 ///< [ERRORS] Include the power callback.

#define FL_DEBUG                          1 ///< Include (and enable) debug module (also enables internal debugging. It's very useful so should only be removed for final releases if at all.).
#define FL_DEBUG_TIMESTAMP                1 ///< Enable/Disable timestamping in the logfile.
#define FL_DEBUG_DATEFORMAT_AMERICAN      0 ///< Enable/Disable american date format.
#define FL_DEBUG_CALLEDFROM               0 ///< Enables called from info in warnings/errors aswell as in the log.
#define FL_DEBUG_LOGGING                  0 ///< Enable/Disable error logging.
#define FL_DEBUG_LOGCLEAR                 0 ///< Clear old debug logs at start of program.
#define FL_DEBUG_LOGPATH                  "./flDebugLog.txt" ///< Path to error log file.
#define FL_DEBUG_STATUS                   0 ///< Enable/Disable logging of additional events such as program start, etc.
#define FL_DEBUG_STATUS_NOTIFY            0 ///< Enable/Disable showing of status messages in the notification tray.
#define FL_DEBUG_STATUS_COLOR             0xFFFF0000 ///< Choose the color in which status messages will be displayed.
#define FL_DEBUG_SCREENSHOTPATH           "./flDebugPic%i.tga" ///< Path to save error screenshot (Must contain a %i).
#define FL_DEBUG_ERROR                    1 ///< Enable/Disable error handling.
#define FL_DEBUG_ERROR_SHOW               1 ///< Enable/Disable visible errors (Still get logged if logging is enabled).
#define FL_DEBUG_ERROR_NOTIFY             1 ///< Enable/Disable showing of errors in the notification tray.
#define FL_DEBUG_ERROR_COLOR              0xFF0000FF ///< Choose the color in which errors will be displayed.
#define FL_DEBUG_ERROR_SCREENSHOT         0 ///< When enabled a screenshot is taken upon an error occuring (Requires GRAPHICS and TEXTURE).
#define FL_DEBUG_WARNING                  0 ///< Enable/Disable warning handling.
#define FL_DEBUG_WARNING_SHOW             0 ///< Enable/Disable visible warnings (Still get logged if logging is enabled).
#define FL_DEBUG_WARNING_NOTIFY           0 ///< Enable/Disable showing of warnings in the notification tray.
#define FL_DEBUG_WARNING_COLOR            0xFF007FFF ///< Choose the color in which warnings will be displayed.
#define FL_DEBUG_WARNING_SCREENSHOT       0 ///< When enabled a screenshot is taken upon a warning occuring (Requires GRAPHICS and TEXTURE).
#define FL_DEBUG_DEVWARNING               0 ///< Enable/Disable dev warning handling.
#define FL_DEBUG_DEVWARNING_SHOW          0 ///< Enable/Disable visible dev warnings (Still get logged if logging is enabled).
#define FL_DEBUG_DEVWARNING_NOTIFY        0 ///< Enable/Disable showing of developer warnings in the notification tray.
#define FL_DEBUG_DEVWARNING_COLOR         0xFFFF3F3F ///< Choose the color in which dev warnings will be displayed.
#define FL_DEBUG_DEVWARNING_SCREENSHOT    0 ///< When enabled a screenshot is taken upon a dev warning occuring (Requires GRAPHICS and TEXTURE).
#define FL_DEBUG_CRITICALERROR_SHOW       1 ///< Enable/Disable visible critical error (Programs still quits, critical errors cannot be disabled).
#define FL_DEBUG_CRITICALERROR_COLOR      0xFF00007F ///< Choose the color in which critical errors will be displayed.
#define FL_DEBUG_CRITICALERROR_SCREENSHOT 1 ///< When enabled a screenshot is taken upon a critical error occuring (Requires GRAPHICS and TEXTURE).



#define FL_MEMORY                         1 ///< Include memory module (wrapper for memory allocation functions).
#define FL_MEMORY_ALIGNMENT_DEFAULT       64 ///< Default alignment (used for memAlloc), should be >= 16 and a binary number (e.g 16, 32, 64, etc.).
#define FL_MEMORY_SCRATCHPAD              1 ///< Enable the use of the 16kb scratchpad.
#define FL_MEMORY_VMEM                    1 ///< Include video memory functions.
#define FL_MEMORY_VMEM_BLOCKSIZE          512 ///< Should be a binary number (128, 256, 512, etc.), lower number means less wastage but slower (Must be above 16).
#define FL_MEMORY_VMEM_OVERFLOW           1 ///< If main memory is full, overflow into vram.
#define FL_MEMORY_VMEM_STIDE_FILL         0 ///< If enabled, all memory allocation functions will use stride memory to make best use of it.
#define FL_MEMORY_VMEM_CACHE              16384 ///< Size of vmem cache, used to speed up various modules and to provide stability.
#define FL_MEMORY_POOL                    0 ///< Include memory pooling module.
#define FL_MEMORY_PAGE                    0 ///< Include memory paging functions.
#define FL_MEMORY_PAGE_PATH               "./flPageFile.dat" ///< The file where paged memory chunks are saved.
#define FL_MEMORY_LOGGING                 0 ///< [INCOMPLETE] Enable memory logging.
#define FL_MEMORY_LOGPATH                 "./flMemoryLog.txt" ///< Path to memory log file (Set to DEBUG_LOGPATH to use the same file for both).
#define FL_MEMORY_ERRORBYTES              0 ///< Adds error checking bytes to the beginning and end of memory allocations, can't deal with uncached memory though.

#define FL_FILE                           1 ///< Include file module.
#define FL_FILE_PBPPLUS                   0 ///< Include pbp plus archive support for file module.

#define FL_TIMER                          1 ///< Include timer module.

#define FL_INPUT                          1 ///< Include input module.

#define FL_MATH                           1 ///< Include maths module.
#define FL_MATH_VFPU                      1 ///< Use VFPU code where possible (You need to add -lpspvfpu to the LIBS line of your makefile, and PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU); to the top of your main.c).
#define FL_MATH_RANDTYPE                  FL_MATH_RAND_VFPU ///< Which method of random number generation to use FL_MATH_RAND_(STDIO/VFPU/MERSENNETWISTER).
#define FL_MATH_RANDOMSEED                0 ///< Randomly reseed random number generator, only works for FL_MATH_STDIO.
#define FL_MATH_RANDOMSEED_FREQUENCY      4096 ///< How often to randomly reseed (A low number can cause repetition if the rand function is called again in less than a microsecond.
#define FL_MATH_TRIGCACHE                 0 ///< [SLOW] How many trig calls to cache, so that calls to trig functions should carry a lower average overhead but it turns out slower so don't use.

#define FL_STRING                         1 ///< Include string module.






// Extra modules - We advise you use these modules as you wish but treat them as optional.

#define FL_COLOR                             1 ///< Include color module.

#define FL_GRAPHICS                          1 ///< Include (and enable) graphics module.
#define FL_GRAPHICS_DISP_BUFFER_VRAM         1 ///< Whether to put the display buffer in vram.
#define FL_GRAPHICS_DISP_BUFFER_FORMAT       GU_PSM_4444 ///< [ERROR when disp and draw are different] What format to use for the display buffer.
#define FL_GRAPHICS_DRAW_BUFFER_FORMAT       GU_PSM_4444 ///< [ERROR when disp and draw are different] What format to use for the draw buffer.
#define FL_GRAPHICS_COPY_BUFFER              0 ///< Copy the buffers across rather than swapping.
#define FL_GRAPHICS_VSYNC_DEFAULT            0 ///< Default (starting) vsync setting.
#define FL_GRAPHICS_VSYNC_AUTO               0 ///< Will automatically enable/disable vsync depending on the framerate.
#define FL_GRAPHICS_VSYNC_AUTO_CUTOFF        55.0f ///< If the FPS goes below this value and GRAPHICS_VSYNC_AUTO is enabled then vsync will turn off.
#define FL_GRAPHICS_DISPLAYLIST_SIZE         262144 ///< Initial size of display list, and only size of display list if it is static.
#define FL_GRAPHICS_DISPLAYLIST_STATIC       0 ///< Declare the display list statically rather than dynamically (No real effect, except that the display list cannot be resized).
#define FL_GRAPHICS_RENDERTARGET_STACK_SIZE  8 ///< The size of the render target stack, used when switching render targets.
#define FL_GRAPHICS_LINES_ANTIALIAS_DEFAULT  0 ///< Default line anti-aliasing value (RECOMENDATION: Set to 0, and turn Line AA on/off when needed).
#define FL_GRAPHICS_FPS_SHOW_DEFAULT         0 ///< Default (starting) show fps setting.
#define FL_GRAPHICS_FPS_SHOW_COLOR           0xFFFFFFFF ///< Color of fps display.
#define FL_GRAPHICS_FPS_DAMP                 0 ///< Whether to damp the fps display (make the numbers stablize).
#define FL_GRAPHICS_FPS_DAMPRATIO            0.25 ///< Amount of effect current fps has on fps display.
#define FL_GRAPHICS_USEDEGENERATETRIS        0 ///< Enable/Disable use of degenerate triangles to lower the amount of calls to draw array required.
#define FL_GRAPHICS_2D                       1 ///< Include 2d graphics module.
#define FL_GRAPHICS_2D_USEDMA                1 ///< Use DMA to copy textures when possible.
#define FL_GRAPHICS_2D_ELLIPSE_MINLINES      16 ///< The minimum number of lines to use when constructing ellipses
#define FL_GRAPHICS_2D_ELLIPSE_EDGESPERRAD   0.25 ///< Number of edges to use per pixel radius.
#define FL_GRAPHICS_3D                       0 ///< Include 3d graphics module.
#define FL_GRAPHICS_EFFECTS_FULLSCREEN       0 ///< Enable/Disable the fullscreen effects module.

#define FL_CAMERA                            1 ///< Enable/Disable camera module.

#define FL_TEXTURE                           1 ///< Include texture module.
#define FL_TEXTURE_UNCACHED                  0 ///< [SEEMS TO CLASH WITH MEMORY ERROR CHECKING] Enable/Disable the use of uncached pointers (Causes wierd artifacts in palettized textures).
#define FL_TEXTURE_AUTOMIPMAP                7 ///< Enable/Disable automatic mipmapping of loaded textures (0 - 7, where 0 is disabled and 7 is maximum).
#define FL_TEXTURE_AUTOMIPMAP_PALETTED       1 ///< Allow auto-mipmapping of paletted textures, not advised as it causes artifacts. (There seems to be an error with level 8 at the minute, so it is internally disabled for this option, but will still occur if you call the function at level 8).
#define FL_TEXTURE_AUTOSWIZZLE               1 ///< Enable/Disable automatic swizzling of loaded textures.
#define FL_TEXTURE_ERRORTEX                  0 ///< Enable/Disable the use of an error texture.
#define FL_TEXTURE_ERRORTEX_COMPILE          0 ///< Compile a default error tex into the program.
#define FL_TEXTURE_ERRORTEX_PATH             "./flTexError.tga" ///< The path for the image loaded to represent a texture error (if one isn't compiled in).
#define FL_TEXTURE_PRESERVENONALPHA          1 ///< If enabled images that are loaded with no alpha channel will be treated differently.
#define FL_TEXTURE_PCX                       0 ///< Include support for PCX textures.
#define FL_TEXTURE_TGA                       1 ///< Include support for TGA textures.
#define FL_TEXTURE_BMP                       0 ///< Include support for BMP textures.
#define FL_TEXTURE_WAL                       0 ///< [INCOMPLETE] Include support for WAL textures.
#define FL_TEXTURE_PNG                       0 ///< [INCOMPLETE] Include support for PNG textures.
#define FL_TEXTURE_FTF                       0 ///< [INCOMPLETE] Include support for FTF textures.
#define FL_TEXTURE_RAW                       0 ///< Include support for RAW textures.
#define FL_TEXTURE_MANIPULATION              0 ///< Include texture manipulation module.

#define FL_MODEL                             0 ///< Include model module.
#define FL_MODEL_OBJ                         0 ///< Include support for OBJ models.
#define FL_MODEL_MD2                         0 ///< Include support for MD2 animated models.

#define FL_SPRITE                            0 ///< Include sprite module.

#define FL_FONT                              1 ///< Include font module.
#define FL_FONT_HSPACE_DEFAULT               0 ///< Default horizontal spacing (in pixels) for fonts.
#define FL_FONT_VSPACE_DEFAULT               0 ///< Default vertical spacing (in pixels) for fonts.
#define FL_FONT_COLOR_DEFAULT                0xFFFFFFFF ///< Default color for fonts.
#define FL_FONT_TABWIDTH_DEFAULT             5 ///< How many spaces long a tab should be.

#define FL_COLLISION                         0 ///< Include collision detection module.

#define FL_NOTIFY                            0 ///< Include notification of status changes.
#define FL_NOTIFY_FREQUENCY                  60 ///< Check for notifications once every FL_NOTIFY_FREQUENCY frames.
#define FL_NOTIFY_DURATION                   10.0f ///< How long to show notifications for.
#define FL_NOTIFY_VOLUME                     0 ///< [REQUIRES KERNEL] Notify the users of volume changes.
#define FL_NOTIFY_BATTERY                    1 ///< Notify if they have a low battery, every quarter segment, or everytime the AC status changes.
#define FL_NOTIFY_TIME                       1 ///< Notify the user of the time at regular intervals.
#define FL_NOTIFY_TRAYSIZE                   4 ///< Size of the notification tray.

#define FL_UMD                               0 ///< [INCOMPLETE] Include umd info module.

#define FL_FILEBROWSER                       0 ///< [INCOMPLETE] Include filebrowser module.


