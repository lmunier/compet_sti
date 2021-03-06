//Module:		    uart.cpp
//version:		    1.0
//Update:           15.06.2018
//Author:	        Munier Louis
/*Description :
 * Management of the UART communication between arduino and raspberry pi3. This part is based a lot on the code given in
 * the link http://www.raspberry-projects.com/pi/programming-in-c/uart-serial-port/using-the-uart
 */

#include "uart.h"

Uart::Uart(){
    //-------------------------
    //----- SETUP USART 0 -----
    //-------------------------
    //At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively

    //OPEN THE UART
    //The flags (defined in fcntl.h):
    //	Access modes (use 1 of these):
    //		O_RDONLY - Open for reading only.
    //		O_RDWR - Open for reading and writing.
    //		O_WRONLY - Open for writing only.
    //
    //	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
    //						if there is no input immediately available (instead of blocking). Likewise, write requests can also return
    //						immediately with a failure status if the output can't be written immediately.
    //
    //	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
    uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY);		//Open in blocking read/write mode

    if (uart0_filestream == -1){
        //ERROR - CAN'T OPEN SERIAL PORT
        printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
    } else {
        state_port = true;
    }

    //CONFIGURE THE UART
    //The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
    //	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
    //	CSIZE:- CS5, CS6, CS7, CS8
    //	CLOCAL - Ignore modem status lines
    //	CREAD - Enable receiver
    //	IGNPAR = Ignore characters with parity errors
    //	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
    //	PARENB - Parity enable
    //	PARODD - Odd parity (else even)
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;		//<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);
}

// Allways listen pin RX
void* Uart::infinite_receiving() {
    while(state_port){
        receive();
        sleep(0.1);
    }

    return NULL;
}

// Send message to arduino
void Uart::send_to_arduino(char type, int x, int y){
    string to_send = "";

    switch(type) {
        case 'F': to_send += "F_"; break;
        case 'B': to_send += "B_"; break;
    }

    to_send += to_string(x);
    to_send += "_";
    to_send += to_string(y);
    to_send += ".";

    if(state_raspicam && state_webcam && enable_sending){
        if(get_avoid_sending() == SENDING_LIMIT-1){
            transmit(to_send);

            #ifdef DISPLAY_MESSAGE
                cout << to_send << endl;
            #endif
        }
    }

    increment_avoid_sending();
}

void Uart::send_to_arduino(char type, char param, int dist) {
    string to_send = "";

    switch(type) {
        case 'S':
            to_send += "S_";

            switch(param) {
                case 'D': to_send += "D.";
                          break;
                case 'G': to_send += "G.";
                          break;
                case 'B': to_send += "B.";
                          break;
                case 'P': to_send += "P.";
                          break;
                case 'A': to_send += "A.";
                          break;
            }
            break;
        case 'A':
            to_send += "A_";

            switch(param) {
                case 'L': to_send += "L.";
                          send_align--;
                          break;
                case 'R': to_send += "R.";
                          send_align--;
                          break;
                case 'F': to_send += "F_";
                          to_send += to_string(dist);
                          to_send += ".";
                          send_fire--;
                          break;
            }
            break;
        case 'I': to_send += "I.";
                  break;
    }

    if(state_raspicam && state_webcam && enable_sending){
        if((send_fire > 0 && param == 'F') || (send_align > 0 && (param == 'L' || param == 'R'))){
            transmit(to_send);

            #ifdef DISPLAY_MESSAGE
                cout << to_send << endl;
            #endif
        }else if(type == 'I' || type == 'S'){
            transmit(to_send);

            #ifdef DISPLAY_MESSAGE
                cout << to_send << endl;
            #endif
        }
    }

    increment_avoid_sending();
}

// To transmit informations by TX pin to the arduino
void Uart::transmit(string to_send) {
    //----- TX BYTES -----
    unsigned char tx_buffer[20];
    unsigned char *p_tx_buffer;

    p_tx_buffer = &tx_buffer[0];

    for(unsigned int c = 0; c < to_send.length(); c++)
        *p_tx_buffer++ = to_send.at(c);

    pthread_mutex_lock(&mutex_lock_transmit);

    if (uart0_filestream != -1){
        //Filestream, bytes to write, number of bytes to write
        int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));

        if (count < 0){
            printf("UART TX error\n");
        }
    }

    pthread_mutex_unlock(&mutex_lock_transmit);
}

// Receive informations from arduino
void Uart::receive() {
    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1) {
        // Read up to 255 characters from the port if they are there
        unsigned char rx_buffer[256];

        //Filestream, buffer to store in, number of bytes to read (max)
        int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);

        if (rx_length < 0) {
            //An error occured (will occur if there are no bytes)
        } else if (rx_length == 0) {
            //No data waiting
        } else {
            //Bytes received
            rx_buffer[rx_length] = '\0';
            decode_message(rx_buffer);
        }
    }
}

// Decode the message received by the RX port
void Uart::decode_message(unsigned char message[]){
    switch(message[0]) {
        case 'B':
            if (bottle_to_throw == false){
                bottle_to_throw = true;
            } else {
                bottle_to_throw = false;
                send_fire = SENDING_LIMIT;
                send_align = SENDING_LIMIT;
            }
            break;
        case 'N':
            if (enable_sending == false){
                enable_sending = true;
            } else {
                enable_sending = false;
                send_fire = SENDING_LIMIT;
                send_align = SENDING_LIMIT;
            }
            break;
   }

    #ifdef DISPLAY_MESSAGE
        cout << "Decode " << message << endl;
    #endif
}

// Close uart port
void Uart::close_port() {
    state_port = false;
    close(uart0_filestream);
}

