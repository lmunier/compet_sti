//Module:		    b_tracking.cpp
//version:		    1.0
//Update:           02.06.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the bottles to throw them.
 */

#include "b_tracking.h"

// Pins initialization
void init_pins(){
    pinMode(LED_PIN, OUTPUT);
}

// Raspicam initialization
RaspiCam_Cv init_raspicam(){
    RaspiCam_Cv camera;

    camera.set(CAP_PROP_FRAME_HEIGHT, HEIGHT_IMAGE);
    camera.set(CAP_PROP_FRAME_WIDTH, WIDTH_IMAGE);

    return camera;
}

// OpenCV function to detect bottles
int bottles_scanning(){
    // Initialize variables
    Mat image;

    wiringPiSetup();
    init_pins();

    // Initialize camera
    RaspiCam_Cv camera = init_raspicam();

    // Open camera
    if(!camera.open()) {
        cout << "ERROR: can not open camera" << endl;
        return -1;
    }

    while(true){
        camera.grab();
        camera.retrieve(image);

        imshow("Image", image);

        if(waitKey(10) == 27)
            break;
    }

    return 1;
}

// LEDs management
void led_enable(bool enable){
    digitalWrite(LED_PIN, enable);
}
