TARGET = flHelloWorld

#PSP_EBOOT_ICON = ICON0.PNG
#PSP_EBOOT_PIC1 = PIC1.PNG

# Add these 2 lines to your makefile omitting the modules that you don't need
FLPATH = ././Code/
FLOBJS = $(FLPATH)funcLib.o \
         $(FLPATH)flCallbacks.o \
         $(FLPATH)flDebug.o \
         $(FLPATH)flMemory.o \
         $(FLPATH)flGraphics.o \
         $(FLPATH)flGraphics2d.o \
         $(FLPATH)flInput.o \
         $(FLPATH)flTexture.o \
         $(FLPATH)flTexturePCX.o \
         $(FLPATH)flTextureRAW.o \
         $(FLPATH)flTextureTGA.o \
         $(FLPATH)flTimer.o \
         $(FLPATH)flFont.o \
         $(FLPATH)flMath.o

OBJS = main.o $(FLOBJS)

CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -fno-strict-aliasing
ASFLAGS = $(CFLAGS)

# Make sure that you have at least these libs in your makefile (more is not a problem)
LIBDIR =
LIBS = -lpspgum -lpspgu -lpsppower -lpsprtc -lm -lpspvfpu -lpspfpu
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = flHelloWorld

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
