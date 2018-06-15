#Module:        Makefile
#Version:       1.0
#Update:        15.06.2018
#Author:        Munier Louis
#Description :
# This file will manage the compilation of our project.


# Macros definitions
CXX     = g++
CXXFLAGS = -g -std=c++11 -Wall
LDFLAGS = /home/pi/opencv-3.4.1/build/lib -lopencv_world -lboost_filesystem \
          -lmmal -lmmal_core -lmmal_util
LIBS = -lstdc++ -lm -Llib -lwiringPi `pkg-config opencv --cflags --libs` \
       -lpthread -lraspicam -lraspicam_cv -lopencv_core -lopencv_highgui
CXXFILES = main.cpp bottles.cpp beacon.cpp stepper.cpp uart.cpp
OFILES = $(CXXFILES:.cpp=.o)
CIBLE = multithread.x


# First rule definition
bottles_tracking.x: $(OFILES)
	$(CXX) $(OFILES) $(LIBS) -o multithread.x

# Particular target definition
depend:
	@echo " *** DEPENDANCIES UPDATE ***"
	@(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	  $(CXX) -MM $(CXXFLAGS) $(CXXFILES) | \
	  egrep -v "/usr/include" \
	 ) >Makefile.new
	@mv Makefile.new Makefile

clean:
	@echo " *** CLEAN UP OBJECT AND EXECUTABLE MODULE ***"
	@/bin/rm -f main.o multithread.o beacon.o uart.o stepper.o bottles.o *.x *.c~ *.h~

#
# -- Dependance rules automatically generate
#
# DO NOT DELETE THIS LINE
