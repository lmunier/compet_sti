//Module:		    uart.h
//version:		    1.0
//Update:           06.06.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the UART communication between arduino and raspberry pi3
 */

#ifndef UART_H
#define UART_H

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

private:
    bool state_port = false;
    int uart0_filestream = -1; // int return when port is initialized

    // Function to transmit data with TX pin
    void transmit(int);

    // Function to receive data with RX pin
    void receive(int);

    // Close UART port
    void close_port(int);
};

#endif //UART_H
