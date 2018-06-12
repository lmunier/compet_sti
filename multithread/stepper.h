//Module:		    CheapStepper
//version:		    1.0
//Update:           15.05.2018
//Responsability:	Munier Louis
/*Description :
 * This file is based on an open one find on github with the following information and will be adapted for our uses.
 */

/*  CheapStepper.h -
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

#ifndef STEPPER_H
#define STEPPER_H

//--------STANDARD LIBRARIES-------------

#include <cstdlib>
#include <iostream>


//--------NON STANDARD LIBRARIES---------

#include <wiringPi.h>


//--------DEFINE SOME CONSTANTS----------

#define V_MIN           7       // Minimum rpm
#define V_MAX           19      // Maximum rpm
#define STEP_MIN_TO_DO	1	// Minimum step to do
#define STEP_MAX_TO_DO	128	// Maximum step to do

#define NBR_STEP        3       // Number of step to do
#define STEP_MAX        4096    // Step maximum for this stepper
#define TOLERANCE_ROT   10      // Tolerance PID rotation
#define MIDDLE          160     // Desired target position

#define KP_ROT          0.2    // Proportionnal coefficient rotation
#define KI_ROT          0       // Integrtive coefficient rotation
#define KD_ROT          0.001       // Derivative coefficient rotation

#define SAMPLING        0.1     // (Sampling time for PID)
#define INITIAL_STEP	0	// Offset at the beginning of competition


//--------FUNCTIONS----------------------

class Stepper
{

public:
    Stepper(int in1, int in2, int in3, int in4);

    void setRpm (int rpm); // sets speed (10-24 rpm, hi-low torque)
    // <6 rpm blocked in code, may overheat
    // 23-24rpm may skip
    void setSeqN (int sequence){seqN = sequence;}
    void setStepN (int step){stepN = step;}

    void set4076StepMode() { totalSteps = 4076; }
    void setTotalSteps (int numSteps) { totalSteps = numSteps; }
    // allows custom # of steps (usually 4076)

    // blocking! (pauses arduino until move is done)
    void move (bool clockwise, int numSteps); // 4096 steps = 1 revolution
    void moveTo (bool clockwise, int toStep); // move to specific step position

    void moveCW (int numSteps) { move (true, numSteps); }
    void moveCCW (int numSteps) { move (false, numSteps); }
    void moveToCW (int toStep) { moveTo (true, toStep); }
    void moveToCCW (int toStep) { moveTo (false, toStep); }

    void step (bool clockwise);
    // move 1 step clockwise or counter-clockwise

    void stepCW () { step (true); } // move 1 step clockwise
    void stepCCW () { step (false); } // move 1 step counter-clockwise

    int getStep() { return stepN; } // returns current miniStep position
    int getDelay() { return delay; } // returns current delay (microseconds)
    int getRpm() { return calcRpm(); } // returns current rpm

    int getPin(int p) {
        if (p<4) return pins[p]; // returns pin #
        return 0; // default 0
    }

    int getStepsLeft() { return stepsLeft; } // returns steps left in current move

    void PID_orientation(int);
    int get_step_to_do(){ return step_to_do; }

private:
    int step_to_do = 0;
    int calcDelay(int rpm); // calcs microsecond step delay for given rpm
    int calcRpm(int _delay); // calcs rpm for given delay in microseconds
    int calcRpm(){
        return calcRpm(delay); // calcs rpm from current delay
    }

    void seqCW();
    void seqCCW();
    void seq(int seqNum); // send specific sequence num to driver

    int pins[4] = {0}; // in1, in2, in3, in4

    int stepN = 0; // keeps track of step position
    // 0-4095 (4096 mini-steps / revolution) or maybe 4076...
    int totalSteps = 4096;

    int delay = 900; // microsecond delay between steps
    // 900 ~= 16.25 rpm
    // low speed (high torque) = 1465 ~= 1 rpm
    // high speed (low torque) = 600 ~=  24 rpm

    int seqN = -1; // keeps track of sequence number

    // variables for non-blocking moves:
    unsigned long lastStepTime; // time in microseconds that last step happened
    int stepsLeft = 0; // steps left to move, neg for counter-clockwise

};

#endif
