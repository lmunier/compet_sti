//Module:		    tracking.h
//version:		    1.0
//Update:           15.05.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

#ifndef LED_TRACKING_TRACKING_H
#define LED_TRACKING_TRACKING_H

#include <iostream>

void LED_tracking();
void LED_detection(int&, int&);

bool is_bottle_captured();
bool is_aligned();

#endif //LED_TRACKING_TRACKING_H
