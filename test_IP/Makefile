#Module: Makefile Version: 1.0 Update: 02.06.2018 Responsability: 
#Munier Louis Description :
# This file will manage the compilation of our project.


# Macros definitions
CXX     = g++
CXXFLAGS = -g -std=c++11 -Wall
LDFLAGS = /home/pi/opencv-3.4.1/build/lib -lopencv_world -lboost_filesystem \
          -lmmal -lmmal_core -lmmal_util
LIBS = -lstdc++ -lm -Llib -lwiringPi `pkg-config opencv --cflags --libs` \
       -lpthread -lraspicam -lraspicam_cv -lopencv_core -lopencv_highgui
CXXFILES = test_dilate_erode.cpp
OFILES = $(CXXFILES:.cpp=.o)
CIBLE = test_dilate_erode.x


# First rule definition
bottles_tracking.x: $(OFILES)
	$(CXX) $(OFILES) $(LIBS) -o test_dilate_erode.x

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
	@/bin/rm -f tes_dilate_erode.o *.x *.c~ *.h~

#
# -- Dependance rules automatically generate
#
# DO NOT DELETE THIS LINE
