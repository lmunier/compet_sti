//Module:		    tracking.cpp
//version:		    1.0
//Update:           15.05.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

/* TODO:
 * - implementation communication arduino
 * - implementation test bottle
 * - implementation test align AND no angle to correct
 */

#include "tracking.h"

#include <future>
#include <thread>

int main() {
    auto state = async(launch::async, led_tracking());

    cout << "caca" << endl;

    int i = 0;
    cin >> i;
    cout << i << endl;
    if(state == -1)
        cout << "There was a problem during the run." << endl;

    return 0;
}
