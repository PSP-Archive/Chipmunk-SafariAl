TARGET_LIB = libchipmunk.a
OBJS = src/chipmunk.o src/cpArbiter.o src/cpArray.o src/cpBB.o src/cpBody.o src/cpCollision.o src/cpHashSet.o src/cpPolyShape.o src/cpShape.o src/cpSpace.o src/cpSpaceHash.o src/cpVect.o \
	    src/constraints/cpBreakableJoint.o src/constraints/cpConstraint.o src/constraints/cpDampedRotarySpring.o src/constraints/cpDampedSpring.o src/constraints/cpGearJoint.o src/constraints/cpGrooveJoint.o src/constraints/cpPinJoint.o src/constraints/cpPivotJoint.o src/constraints/cpRotaryLimitJoint.o src/constraints/cpSimpleMotor.o src/constraints/cpSlideJoint.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall -ffast-math
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= 

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

install:
	mkdir -p $(PSPSDK)/include/chipmunk
	cp -f src/*.h $(PSPSDK)/include/chipmunk
	cp -f $(TARGET_LIB) $(PSPSDK)/lib/