//Module:		    beacon.h
//version:		    1.0
//Update:           06.06.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */


#ifndef LED_TRACKING_TRACKING_H
#define LED_TRACKING_TRACKING_H


//--------STANDARD LIBRARIES----------

#include <cfloat>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <unistd.h>


//--------NON STANDARD LIBRARIES------

#include <opencv2/opencv.hpp>


//--------CUSTOM LIBRARIES------------

#include "stepper.h"
#include "uart.h"
#include "global_include.h"


using namespace cv;
using namespace std;


//--------DEFINE SOME CONSTANTS-------

#define SB_IN1          	1       // Correpsond to GPIO 18 (BCM)
#define SB_IN2          	4       // Correpsond to GPIO 23 (BCM)
#define SB_IN3          	5       // Correpsond to GPIO 24 (BCM)
#define SB_IN4          	6       // Correpsond to GPIO 25 (BCM)

#define TOL_BEACON      	10      // Tolerance on beacon width/inclination
#define ERROR_DIST          -1.0	// If we have an error when we calculate the distance
#define TOL_HOLE            20      // Tolerance on "hole" in beacon detection
#define BEACON_SIZE_MIN		60.0	// Minimum size to avoid having an obstacle on the way
#define BEACON_WIDTH_MIN	40.0	// Width to determine new ROI in order to minimize computation

#define HUE             	0       // Channel hue on image vector
#define SAT             	1       // Channel saturation on image vector
#define VAL             	2       // Channel value on image vector

#define RED	                2	    // Red channel

#define HEIGHT_IMAGE    	240     // Height of our image/frame
#define WIDTH_IMAGE     	320     // Width of our image/frame

#define TOLERANCE_ALIGN 	10      // Tolerance for alignment
#define WAIT_WAKEUP_WEBCAM	2	// Waiting to the webcam waking up
#define MAX_FPS			5	// FPS max for the webcam

#define OFF_BUTTON		29	// Wiring pin to confirm calibration (same as shutdown)

//--------FUNCTIONS------------------

// Initialize pins stepper
Stepper init_stepper();

// Initialize webcam
VideoCapture init_webcam();

// Main part, tracking of the corner led where is the bin
void* led_tracking(void*);

// Extract searching beacon led color
Mat extract_color(Mat&, Mat&, int[], int[]);

// Reduce region of interest to reduce computation costs
Rect get_roi(int);

// Extract position of beacon led
void extract_position(Mat&, int&);

// Extract position of beacon led
int extract_height(Mat&, int, int&, int&);

// Give distance to corner
double get_dist_corner(int, int, char);

// Manage stepper back
void manage_stepper(Stepper&, int);

// Check if the robot is aligned
bool is_aligned(Stepper);

#endif //LED_TRACKING_TRACKING_H
