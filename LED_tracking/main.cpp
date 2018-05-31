//Module:		    tracking.cpp
//version:		    1.0
//Update:           15.05.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

#include "tracking.h"

int main() {
    if(led_tracking() == -1)
        std::cout << "There was a problem during the run." << std::endl;

    return 0;
}
