//Module:		    uart.h
//version:		    1.0
//Update:           06.06.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the UART communication between arduino and raspberry pi3
 */

#ifndef UART_H
#define UART_H

#include <string>
#include <iostream>
#include <cstdio>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART

using namespace std;

class Uart {
public:
    // Initialization of UART communication
    Uart();
    int get_uart(){ return uart0_filestream; }

    // Function to always listen the RX port
    void* infinite_receiving();

    // Function to transmit data with TX pin
    void transmit(string);

    // Initialize mutex
    void set_mutex(pthread_mutex_t mutex){ mutex_lock_transmit = mutex; }

    // Return value of bottle_to_throw
    bool is_bottle(){ return bottle_to_throw; }

private:
    // Bottle in robot
    bool bottle_to_throw = false;

    bool state_port = false;
    int uart0_filestream = -1; // int return when port is initialized
    pthread_mutex_t mutex_lock_transmit;

    // Function to receive data with RX pin
    void receive();

    // Close UART port
    void close_port();
};

#endif //UART_H
