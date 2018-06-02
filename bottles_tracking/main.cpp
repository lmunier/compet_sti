//Module:		    main.cpp
//version:		    1.0
//Update:           02.06.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the bottles to throw them.
 */

#include <iostream>
#include "b_tracking.h"

int main() {
    if(bottles_scanning() == -1)
        cout << "There was a problem during the run." << endl;

    return 0;
}