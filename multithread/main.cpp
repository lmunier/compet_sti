//Module:		    beacon.cpp
//version:		    1.0
//Update:           15.06.2018
//Author:	        Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <sys/syscall.h>

#include "beacon.h"
#include "uart.h"
#include "bottles.h"

using namespace std;

// Typedef to give a class method function to a pthread
typedef void* (*UARTPTR)(void*);

int main(){
    // Declaration of mutex to avoid collision on TX pin
    pthread_mutex_t mutex_lock_transmit = PTHREAD_MUTEX_INITIALIZER;

    // Initialize variables
    pthread_t bottles_detection;
    pthread_t beacon_detection;
    pthread_t listen_serial;

    Uart *ptr_uart0 = new Uart();
    ptr_uart0->set_mutex(mutex_lock_transmit);

    //------------------Thread creation----------------
    // Thread to listen arduino
    int rx_verif = pthread_create(&listen_serial, NULL, (UARTPTR) &Uart::infinite_receiving, ptr_uart0);

    if (rx_verif) {
        cout << "Error:unable to create thread," << rx_verif << endl;
        exit(-1);
    }

    // Thread to detect bottles
    int bottles_verif = pthread_create(&bottles_detection, NULL, bottles_scanning, (void*) ptr_uart0);

    if (bottles_verif) {
        cout << "Error:unable to create thread," << bottles_verif << endl;
        exit(-1);
    }

    // Thread to track beacon led
    int led_verif = pthread_create(&beacon_detection, NULL, led_tracking, (void*) ptr_uart0);

    if (led_verif) {
        cout << "Error:unable to create thread," << led_verif << endl;
        exit(-1);
    }

    pthread_join(rx_verif, NULL);
    pthread_join(bottles_verif, NULL);
    pthread_join(led_verif, NULL);

    while(true){
        sleep(0.1);
    }

    pthread_exit(NULL);
}
