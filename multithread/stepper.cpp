//Module:		    stepper.cpp
//version:		    1.0
//Update:           06.06.2018
//Responsability:	Munier Louis
/*Description :
 * This file is based on an open one (CheapStepp.cpp) find on github with the following informations and is
 * adapted for our uses.
 */

/*  CheapStepper.cpp -
	v0.2
	Library for the 28BYJ-48 stepper motor, using ULN2003 driver board
	https://arduino-info.wikispaces.com/SmallSteppers

	Library written by Tyler Henry, 6/2016

	uses 8-step sequence: A-AB-B-BC-C-CD-D-DA

	motor has gear ratio of either:
		64:1 (per manufacturer specs)
		or
		63.68395:1 measured
			(see: http://forum.arduino.cc/index.php?topic=71964.15)
	* 64 steps per internal motor rev
	=

	4096 total mini-steps / revolution
	or ~4076 (4075.7728) depending on exact gear ratio

	assumes 5v power source for rpm calc
*/

#include "stepper.h"

Stepper::Stepper (int in1, int in2, int in3, int in4) {
    pins[0] = in1;
    pins[1] = in2;
    pins[2] = in3;
    pins[3] = in4;

    for (int pin=0; pin<4; pin++){
        pinMode(pins[pin], OUTPUT);
    }
}

void Stepper::setRpm (int rpm){
    delay = calcDelay(rpm);
}

void Stepper::move (bool clockwise, int numSteps){
    for (int n=0; n<numSteps; n++){
        step(clockwise);
    }
}

void Stepper::moveTo (bool clockwise, int toStep){
    // keep to 0-(totalSteps-1) range
    if (toStep >= totalSteps) toStep %= totalSteps;
    else if (toStep < 0) {
        toStep %= totalSteps; // returns negative if toStep not multiple of totalSteps
        if (toStep < 0) toStep += totalSteps; // shift into 0-(totalSteps-1) range
    }
    while (stepN != toStep){
        step(clockwise);
    }
}

void Stepper::step(bool clockwise){
    if (clockwise) seqCW();
    else seqCCW();
}

// Controller to correct rotation
void Stepper::PID_orientation(int pos_beacon) {
    static int e = 0;
    static int ui = 0, e_p = 0, ud = 0;
    static int speed = 0;

    e = MIDDLE - pos_beacon;

    // To minimize the effects of noise and data oscillation
    if(e < TOLERANCE_ROT && e > - TOLERANCE_ROT){
        e = 0;
    }

    ui += e*SAMPLING;
    ud = (int)((e - e_p)/SAMPLING - 0.5);

    speed = (int)(KP_ROT*e + KI_ROT*ui + KD_ROT*ud);

    e_p = e;

    // Anti-windup control to avoid aberrant commands
    if (abs(speed) > V_MAX) {
        ui = (int)((V_MAX/KP_ROT) - e - ud);

        if(speed > 0)
            speed = V_MAX;
        else
            speed = -V_MAX;
    } else if (abs(speed) < V_MIN) {
        ui = (int)((V_MIN/KP_ROT) - e - ud);

        if(speed > 0)
            speed = V_MIN;
        else
            speed = -V_MIN;
    }

    setRpm(abs(speed));
}


/////////////
// PRIVATE //
/////////////

int Stepper::calcDelay (int rpm){
    if (rpm < 6) return delay; // will overheat, no change
    else if (rpm >= 24) return 600; // highest speed

    unsigned long d = 60000000 / (totalSteps* (unsigned long) rpm);
    // in range: 600-1465 microseconds (24-1 rpm)
    return (int) d;
}

int Stepper::calcRpm (int _delay){
    unsigned long rpm = 60000000 / (unsigned long) _delay / totalSteps;
    return (int) rpm;
}

void Stepper::seqCW (){
    seqN++;
    if (seqN > 7) seqN = 0; // roll over to A seq
    seq(seqN);

    stepN++; // track miniSteps
    if (stepN >= totalSteps){
        stepN -=totalSteps; // keep stepN within 0-(totalSteps-1)
    }
}

void Stepper::seqCCW (){
    seqN--;
    if (seqN < 0) seqN = 7; // roll over to DA seq
    seq(seqN);

    stepN--; // track miniSteps
    if (stepN < 0){
        stepN +=totalSteps; // keep stepN within 0-(totalSteps-1)
    }
}

void Stepper::seq (int seqNum){
    int pattern[4];
    // A,B,C,D HIGH/LOW pattern to write to driver board

    switch(seqNum){
        case 0:
            pattern[0] = 1;
            pattern[1] = 0;
            pattern[2] = 0;
            pattern[3] = 0;
            break;

        case 1:
            pattern[0] = 1;
            pattern[1] = 1;
            pattern[2] = 0;
            pattern[3] = 0;
            break;

        case 2:
            pattern[0] = 0;
            pattern[1] = 1;
            pattern[2] = 0;
            pattern[3] = 0;
            break;

        case 3:
            pattern[0] = 0;
            pattern[1] = 1;
            pattern[2] = 1;
            pattern[3] = 0;
            break;

        case 4:
            pattern[0] = 0;
            pattern[1] = 0;
            pattern[2] = 1;
            pattern[3] = 0;
            break;

        case 5:
            pattern[0] = 0;
            pattern[1] = 0;
            pattern[2] = 1;
            pattern[3] = 1;
            break;

        case 6:
            pattern[0] = 0;
            pattern[1] = 0;
            pattern[2] = 0;
            pattern[3] = 1;
            break;

        case 7:
            pattern[0] = 1;
            pattern[1] = 0;
            pattern[2] = 0;
            pattern[3] = 1;
            break;

        default:
            pattern[0] = 0;
            pattern[1] = 0;
            pattern[2] = 0;
            pattern[3] = 0;
            break;

    }

    // write pattern to pins
    for (int p=0; p<4; p++){
        digitalWrite(pins[p], pattern[p]);
    }
    delayMicroseconds(delay);
}
