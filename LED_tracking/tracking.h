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
#define SB_IN1          1       // Correpsond to GPIO 18 (BCM)
#define SB_IN2          4       // Correpsond to GPIO 23 (BCM)
#define SB_IN3          5       // Correpsond to GPIO 24 (BCM)
#define SB_IN4          6       // Correpsond to GPIO 25 (BCM)

#define BLUE            0       // Channel blue on image vector
#define GREEN           1       // Channel green on image vector
#define RED             2       // Channel red on image vector

#define HEIGHT_IMAGE    320     // Height of our image/frame
#define WIDTH_IMAGE     640     // Width of our image/frame

#define H_ZERO          1       // Measure between up led and ceil of our image at distance "zero"
#define DIST_ZERO       10      // Distance "zero"

#define TOLERANCE_ALIGN 10      // Tolerance for alignment

// Initialize pins stepper
CheapStepper init_stepper(int&, int&);

// Initialize webcam
VideoCapture init_webcam();

// Main part, tracking of the corner led where is the bin
int led_tracking();

// Extract searching beacon led color
Mat extract_color(Mat&, int[], int[]);

// Extract position of beacon led
int extract_position(Mat&, int&);

// Give distance to corner
int get_dist_corner(double);

// Manage stepper back
void manage_stepper(CheapStepper&, int, int&);

// If  a bottle is captured
bool is_bottle_captured();

// Check if the robot is aligned
bool is_aligned(int);

#endif //LED_TRACKING_TRACKING_H
