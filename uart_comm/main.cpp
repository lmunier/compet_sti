//Module:		    main.cpp
//version:		    1.0
//Update:           06.06.2018
//Responsability:	Munier Louis
/*Description :
 * Manage UART communication
 */

#include <iostream>
#include <cstdio>
#include <string.h>

#include <wiringSerial.h>
#include <wiringPi.h>

using namespace std;

int serial;

// Initialization of UART communication
void init_uart();

// Function to run on interrupt
void receive();

int main() {
    // Initialize variables
    if((serial = serialOpen("/dev/ttyAMA0", 9600)) < 0){
        cout << "Can not open serial device" << endl;
        return -1;
    }

    // Initialize GPiO
    wiringPiSetup();

//    wiringPiISR(16, INT_EDGE_FALLING, &receive);

    serialPutchar(serial, '1');

    while(true){
        cout << serialGetchar(serial) << endl;

        putchar(serialGetchar(serial));
        fflush(stdout);
    }

    return 0;
}

void receive(){
    cout << serialGetchar(serial) << endl;

    putchar(serialGetchar(serial));
    fflush(stdout);
}
