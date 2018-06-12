//Module:		    uart.h
//version:		    1.0
//Update:           06.06.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the UART communication between arduino and raspberry pi3
 */

#ifndef UART_H
#define UART_H


#define SENDING_LIMIT		5	// Value to avoid sending too many times infos to arduino


//---------------STANDARD LIBRARIES--------------

#include <string>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


//---------------CUSTOM LIBRARIES----------------

#include "global_include.h"


using namespace std;


//---------------CLASS DECLARATION---------------

class Uart {
public:
    // Initialization of UART communication
    Uart();
    int get_uart(){ return uart0_filestream; }

    // Function to always listen the RX port
    void* infinite_receiving();

    // Send message to arduino
    void send_to_arduino(char, int, int);
    void send_to_arduino(char, char = ' ', int dist = 0);

    // Initialize mutex
    void set_mutex(pthread_mutex_t mutex){ mutex_lock_transmit = mutex; }

    // Set state of each camera
    void set_state_webcam(bool state){ state_webcam = state; }
    void set_state_camera(bool state){ state_raspicam = state; }

    // Return value of bottle_to_throw
    bool is_bottle(){ return bottle_to_throw; }

    // Return state of each camera
    bool is_webcam_ready(){ return state_webcam; }
    bool is_raspicam_ready(){ return state_raspicam; }

    // Management of variable to not send too many times infos
    void increment_avoid_sending(){ avoid_send_many = (avoid_send_many + 1)%SENDING_LIMIT; }
    int get_avoid_sending(){ return avoid_send_many; }

private:
    // Avoid to send too many times informations to arduino
    int avoid_send_many = 0;

    // Bottle in robot
    bool bottle_to_throw = false;

    // State of each camera
    bool state_webcam = false;
    bool state_raspicam = false;

    bool state_port = false;
    int uart0_filestream = -1;              // int return when port is initialized
    pthread_mutex_t mutex_lock_transmit;

    // Function to transmit data with TX pin
    void transmit(string);

    // Function to receive data with RX pin
    void receive();

    // Decode message received
    void decode_message(unsigned char[]);

    // Close UART port
    void close_port();
};

#endif //UART_H
