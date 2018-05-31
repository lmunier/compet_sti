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
#include <ctime>
#include <cstdlib>
#include <iostream>

// Initialize webcam
cv::VideoCapture init_webcam();

// Main part, tracking of the corner led where is the bin
int led_tracking();

// Convert input image in binary image
cv::Mat convert2binary(cv::Mat, double, double);

// Extract searching beacon led color
cv::Mat extract_color(cv::Mat, int[], int[]);

// Extract position of beacon led
void extract_position(cv::Mat, int&, int&);

// If  a bottle is captured
bool is_bottle_captured();

// Check if the robot is aligned
bool is_aligned();

#endif //LED_TRACKING_TRACKING_H
