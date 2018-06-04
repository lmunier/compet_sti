
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

#define LED_PIN         0       // PIN to enable LEDs gpio (BCM 17)

#define HEIGHT_IMAGE    480     // Height of our image/frame
#define WIDTH_IMAGE     640     // Width of our image/frame

// Pins initialization
void init_pins();

// Raspicam initialization
RaspiCam_Cv init_raspicam();

// OpenCV function to detect bottles
int bottles_scanning();

// LEDs management
void led_enable(bool);

#endif //BOTTLES_TRACKING_B_TRACKING_H
