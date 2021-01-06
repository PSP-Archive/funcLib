TARGET = flAstro4MHz

#PSP_EBOOT_ICON = ICON0.PNG
#PSP_EBOOT_PIC1 = PIC1.PNG

OBJS = main.o ././Code/funcLib.o

CFLAGS = -O3 -G0 -Wall# -Wextra -Wdisabled-optimization
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -fno-strict-aliasing
ASFLAGS = $(CFLAGS)

# Make sure that you have at least these libs in your makefile (more is not a problem)
LIBDIR =
LIBS = -lpspgum -lpspgu -lpsppower -lpsprtc -lpspfpu -lpspvfpu -lm
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = flAstro4MHz

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
