#Module:		Makefile
#Version:		1.0
#Update:		02.06.2018
#Responsability:	Munier Louis
#Description :
# This file will manage the compilation of our project.


# Macros definitions
CC     = g++
CFLAGS = -g -Wall
LDFLAGS = /home/pi/opencv-3.4.1/build/lib -lopencv_world -lboost_filesystem
LIBS = -lstdc++ -lm -Llib -lwiringPi `pkg-config opencv --cflags --libs`
CFILES = main.cpp b_tracking.cpp
OFILES = $(CFILES:.cpp=.o)
CIBLE = bottles_tracking.x


# First rule definition
bottles_tracking.x: $(OFILES)
	$(CC) $(OFILES) $(LIBS) -o bottles_tracking.x

# Particular target definition
depend:
	@echo " *** DEPENDANCIES UPDATE ***"
	@(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	  $(CC) -MM $(CFLAGS) $(CFILES) | \
	  egrep -v "/usr/include" \
	 ) >Makefile.new
	@mv Makefile.new Makefile

clean:
	@echo " *** CLEAN UP OBJECT AND EXECUTABLE MODULE ***"
	@/bin/rm -f main.o b_tracking.o *.x *.c~ *.h~

#
# -- Dependance rules automatically generate
#
# DO NOT DELETE THIS LINE
