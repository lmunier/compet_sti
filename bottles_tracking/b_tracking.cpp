//Module:		    b_tracking.cpp
//version:		    1.0
//Update:           02.06.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the bottles to throw them.
 */

#include "b_tracking.h"

// Pins initialization
void init_pins(){
    pinMode(LED_PIN, OUTPUT);
}

// OpenCV function to detect bottles
int bottles_scanning(){
    wiringPiSetup();
    init_pins();

    int c = 5;

    while(c > 0){
        led_enable(true);
        delay(5000);
        //led_enable(false);
        //delay(5000);

        //c--;
    }

    return 1;
}

// LEDs management
void led_enable(bool enable){
    digitalWrite(LED_PIN, enable);
}
