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

    // Initialization of variables
    // Set distance variables
    int bottle_x_pos = 0;
    int bottle_y_pos = 0;

    // Set blur kernel
    int kernel_blur = 3;

    // Extract max, min
    Point minLoc, maxLoc;
    double min=0.0, max=0.0;

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
    }

    return 1;
}

// LEDs management
void led_enable(bool enable){
    digitalWrite(LED_PIN, enable);
}
