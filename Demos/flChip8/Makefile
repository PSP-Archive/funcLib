TARGET = flChip8

#PSP_EBOOT_ICON = ICON0.PNG
#PSP_EBOOT_PIC1 = PIC1.PNG

OBJS = main.o emulator.o ././Code/funcLib.o

CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -fno-strict-aliasing
ASFLAGS = $(CFLAGS)

LIBDIR =
LIBS = -lpspgum -lpspgu -lpsppower -lpsprtc -lpspfpu -lpspvfpu -lm
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = flChip8

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
