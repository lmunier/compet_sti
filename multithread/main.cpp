//Module:		    beacon.cpp
//version:		    1.0
//Update:           06.06.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

#include <iostream>
#include <cstdlib>
#include <pthread.h>

#include "beacon.h"
#include "uart.h"
#include "bottles.h"

using namespace std;

typedef void* (*UARTPTR)(void*);

int main(){
    // Initialize variables
    pthread_t bottles_detection;
    pthread_t beacon_detection;
    pthread_t listen_serial;

    Uart *ptr_uart0 = new Uart();

    //------------------Thread creation----------------
    // Thread to listen arduino
    int rx_verif = pthread_create(&listen_serial, NULL, (UARTPTR) &Uart::infinite_receiving, ptr_uart0);

    if (rx_verif) {
        cout << "Error:unable to create thread," << rx_verif << endl;
        exit(-1);
    }

    // Thread to listen arduino
    int bottles_verif = pthread_create(&beacon_detection, NULL, led_tracking, NULL);

    if (bottles_verif) {
        cout << "Error:unable to create thread," << bottles_verif << endl;
        exit(-1);
    }

    // Thread to listen arduino
    int led_verif = pthread_create(&bottles_detection, NULL, bottles_scanning, NULL);

    if (led_verif) {
        cout << "Error:unable to create thread," << led_verif << endl;
        exit(-1);
    }

    pthread_exit(NULL);
}
