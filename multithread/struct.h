//Module:		struct.h
//Version:		1.0
//Update:		07.06.2018
//Responsability:	Munier Louis
//Description:
/* Structure declaration to pass multiple arguments to pthread functions
 *
 */

#ifndef STRUCT_H
#define STRUCT_H

#include <pthread.h>

#include "uart.h"

struct arg_struct {
    bool bottle_to_throw;
    Uart uart0;
};

#endif
