//Module:		    tracking.h
//version:		    1.0
//Update:           15.05.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

#include "tracking.h"

void LED_tracking() {
    int dist_to_corner = 0;
    int dist_to_center = 0;
    int c = 0;

    while(c < 5){
        LED_detection(dist_to_center, dist_to_corner);

        if(is_bottle_captured())
            std::cout << "Send to arduino " << dist_to_center << ", " << dist_to_corner << std::endl;

        if(is_aligned())
            std::cout << "Fire !!!" << std::endl;

        c++;
    }
}

void LED_detection(int& center, int& corner){
    std::cout << "I try to detect corner LED" << std::endl;
}

bool is_bottle_captured(){
    return true;
}

bool is_aligned(){
    return true;
}
