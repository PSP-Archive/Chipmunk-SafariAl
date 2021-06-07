TARGET = CPhysicsDemo/chipmunkdemo
OBJS = main.o Demo1.o Demo2.o Demo3.o Demo4.o Demo5.o Demo6.o Demo7.o 
 
INCDIR = 
CFLAGS = -G0 -Wall -O0 -g -ffast-math
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
 
LIBDIR =
LDFLAGS =
LIBS=-lpspctrl -lpspgum -lpspgu -lstdc++ -lchipmunk -lm
 
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Chipmunk Physics Demo
BUILD_PRX = 1
 
 
 
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak