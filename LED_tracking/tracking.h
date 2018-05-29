//Module:		    tracking.h
//version:		    1.0
//Update:           15.05.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

#ifndef LED_TRACKING_TRACKING_H
#define LED_TRACKING_TRACKING_H

#include <opencv2/opencv.hpp>
//#include <raspicam/raspicam.h>
#include <ctime>
#include <cstdlib>
#include <iostream>

VideoCapture init_webcam();

int LED_tracking();
void LED_detection(int&, int&);
cv2::Mat convert2binary(cv2::Mat);

bool is_bottle_captured();
bool is_aligned();

#endif //LED_TRACKING_TRACKING_H
