//Module:		    b_tracking.h
//version:		    1.0
//Update:           02.06.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the bottles to throw them.
 */

#ifndef BOTTLES_TRACKING_B_TRACKING_H
#define BOTTLES_TRACKING_B_TRACKING_H

#include <wiringPi.h>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>
#include <cfloat>
#include <ctime>
#include <cstdlib>
#include <iostream>

using namespace cv;
using namespace std;
using namespace raspicam;

#define LED_PIN             0       // PIN to enable LEDs gpio (BCM 17)

#define HUE                 0       // Channel hue on image vector
#define SAT                 1       // Channel saturation on image vector
#define VAL                 2       // Channel value on image vector

#define NB_CHANNELS         3       // Number of channel per pixel

#define HEIGHT_IMAGE        480     // Height of our image/frame
#define WIDTH_IMAGE         640     // Width of our image/frame

#define BOTTLE_TOLERANCE    10      // Tolerance to not detect bottle if the max luminosity point is on the top left
#define AVOID_NOISE         HEIGHT_IMAGE/3

// Pins initialization
void init_pins();

// Raspicam initialization
RaspiCam_Cv init_raspicam();

// LEDs management
void led_enable(bool);

// OpenCV function to detect bottles
int bottles_scanning();

// Maximum light localization
void max_light_localization(Mat&, double&, Point&, int);

// Region Of Interest to avoid near localization
Mat set_roi(Mat&);

// Surchatge of region Of Interest near of the maximum localization
Mat set_roi(Mat&, Point);

// Delete some color in HSV
Mat del_color(Mat&, int[][NB_CHANNELS], int[][NB_CHANNELS]);

#endif //BOTTLES_TRACKING_B_TRACKING_H
