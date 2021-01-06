TARGET = BattleTrax

PSP_EBOOT_ICON = ICON0.PNG
#PSP_EBOOT_PIC1 = PIC1.PNG

# Add these 2 lines to your makefile omitting the modules that you don't need
FLPATH = ././Code/
FLOBJS = $(FLPATH)funcLib.o \
         $(FLPATH)flDebug.o \
         $(FLPATH)flMemory.o \
         $(FLPATH)flInput.o \
         $(FLPATH)flTimer.o \
         $(FLPATH)flGraphicsSimple.o \
         $(FLPATH)flGraphicsSimpleSprite.o \
         $(FLPATH)flGraphicsSimpleFont.o

OBJS = main.o intro.o game.o player.o menu.o $(FLOBJS)

CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

# Make sure that you have at least these libs in your makefile (more is not a problem)
LIBDIR =
LIBS = -lpspgu -lpspgum -lpsppower -lpsprtc -lpng -lm -lz -lpspvfpu -lpspfpu
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = BattleTrax

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
