TARGET = flSecurityGlobe

#PSP_EBOOT_ICON = ICON0.PNG
#PSP_EBOOT_PIC1 = PIC1.PNG

OBJS = main.o ././Code/funcLib.o

CFLAGS = -g -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -fno-strict-aliasing
ASFLAGS = $(CFLAGS)

# Make sure that you have at least these libs in your makefile (more is not a problem)
LIBDIR =
LIBS = -lpspgum -lpspgu -lpsppower -lpsprtc -lpspfpu -lpspvfpu -lm
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = flSecurityGlobe
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
