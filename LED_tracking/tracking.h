//Module:		    tracking.h
//version:		    1.0
//Update:           15.05.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

#ifndef LED_TRACKING_TRACKING_H
#define LED_TRACKING_TRACKING_H

#include "CheapStepper.h"

#include <opencv2/opencv.hpp>
#include <cfloat>
#include <ctime>
#include <cstdlib>
#include <iostream>

using namespace cv;
using namespace std;

// Define some constants
#define BLUE            0
#define GREEN           1
#define RED             2

#define HEIGHT_IMAGE    320
#define WIDTH_IMAGE     640

#define H_ZERO          1
#define DIST_ZERO       10

#define TOLERANCE       10

// Initialize webcam
VideoCapture init_webcam();

// Main part, tracking of the corner led where is the bin
int led_tracking();

// Extract searching beacon led color
Mat extract_color(Mat&, int[], int[]);

// Extract position of beacon led
int extract_position(Mat&, int&);

// Give distance to corner
double get_dist_corner(double);

// If  a bottle is captured
bool is_bottle_captured();

// Check if the robot is aligned
bool is_aligned(int);

#endif //LED_TRACKING_TRACKING_H
