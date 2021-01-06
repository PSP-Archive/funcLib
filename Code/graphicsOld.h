#define g_vram_base (u32*)(MEMORY_VMEM_BASE | MEMORY_UNCACHED_OFFSET);

#define	PSP_LINE_SIZE SCREEN_STRIDE

#define A(color) ((u8)(color >> 24 & 0xFF))
#define B(color) ((u8)(color >> 16 & 0xFF))
#define G(color) ((u8)(color >> 8 & 0xFF))
#define R(color) ((u8)(color & 0xFF))

#define IS_ALPHA(color) (((color) & 0xff000000) == 0xff000000 ? 0 : 1)
#define FRAMEBUFFER_SIZE ((SCREEN_STRIDE * SCREEN_HEIGHT) << 2)
#define MAX(X, Y) max(x, y)

#define Color u32

#define Image Texture

//extern void makeColorAlpha(Image* source, Color color);


#define loadImage(filename) texLoad(filename)
//extern void blitImageToImage(int sx, int sy, int width, int height, Image* source, int dx, int dy, Image* destination);
#define blitImageToScreen(sx, sy, width, height, source, dx, dy) graphics2dDrawTextureSection(dx, dy, width, height, source, sx, sy, width, height)
//extern void blitAlphaImageToImage(int sx, int sy, int width, int height, Image* source, int dx, int dy, Image* destination);
#define blitAlphaImageToScreen(sx, sy, width, height, source, dx, dy) graphics2dDrawTextureSection(dx, dy, width, height, source, sx, sy, width, height)
#define createImage(width, height) texCreate(width, height, GU_PSM_8888)
#define freeImage(image) texFree(image)
//extern void clearImage(Color color, Image* image);
//extern void clearScreen(Color color);
//extern void fillImageRect(Color color, int x0, int y0, int width, int height, Image* image);
#define fillScreenRect(color, x0, y0, width, height) graphics2dDrawRectFilled(x0, y0, width, height, color)
#define putPixelScreen(color, x, y) graphics2dDrawPoint(x, y, color)
//extern void putPixelImage(Color color, int x, int y, Image* image);
//extern Color getPixelScreen(int x, int y);
//extern Color getPixelImage(int x, int y, Image* image);
//extern void printTextScreen(int x, int y, const char* text, u32 color);
//extern void printTextImage(int x, int y, const char* text, u32 color, Image* image);
//extern void saveImage(const char* filename, Color* data, int width, int height, int lineSize, int saveAlpha);
#define flipScreen() graphicsSwapBuffers()
#define initGraphics() graphicsInit()
#define disableGraphics() graphicsTerm()
#define drawLineScreen(x0, y0, x1, y1, color) graphics2dDrawLine(x0, y0, x1, y1, color)
//extern void drawLineImage(int x0, int y0, int x1, int y1, Color color, Image* image);
#define getVramDrawBuffer() graphicsDrawBuffer
#define getVramDisplayBuffer() graphicsDisplayBuffer
//extern void guStart();
