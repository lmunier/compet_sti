#Module:		Makefile
#Version:		1.0
#Update:		29.05.2018
#Responsability:	Munier Louis
#Description :
# This file will manage the compilation of our project.


# Macros definitions
CXX     = g++
CXXFLAGS = -g -std=c++0x -Wall
LDFLAGS = /home/pi/opencv-3.4.1/build/lib -lopencv_world -lboost_filesystem
LIBS = -lstdc++ -lm -Llib -lwiringPi `pkg-config opencv --cxxflags --libs` -lpthread
CXXFILES = main.cpp CheapStepper.cpp tracking.cpp
OFILES = $(CXXFILES:.cpp=.o)
CIBLE = led_tracking.x


# First rule definition
led_tracking.x: $(OFILES)
	$(CXX) $(OFILES) $(LIBS) -o led_tracking.x

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
	@/bin/rm -f CheapStepper.o main.o tracking.o *.x *.cpp~ *.h~

#
# -- Dependance rules automatically generate
#
# DO NOT DELETE THIS LINE
