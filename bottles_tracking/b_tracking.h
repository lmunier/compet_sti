//Module:		    b_tracking.h
//version:		    1.0
//Update:           02.06.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the bottles to throw them.
 */

#ifndef BOTTLES_TRACKING_B_TRACKING_H
#define BOTTLES_TRACKING_B_TRACKING_H

#define LED_PIN         0 // PIN to enable LEDs gpio (BCM 17)

#include <opencv2/opencv.hpp>
#include <cfloat>
#include <ctime>
#include <cstdlib>
#include <iostream>

using namespace cv;
using namespace std;

// Pins initialization
void init_pins();

// OpenCV function to detect bottles
int bottles_scanning();

// LEDs management
void led_enable(bool);

#endif //BOTTLES_TRACKING_B_TRACKING_H
