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
    Point max_loc;
    double max = 0.0;

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
        // Take video input
        camera.grab();
        camera.retrieve(image);

        max_light_localization(image, max, max_loc);
    }
}

// Localize the maximum of light in image
void max_light_localization(Mat& image, double& max, Point& max_loc){
    // Initialize matrices
    Mat gray;
    Mat blur;

    // Initialize variables
    int kernel_blur = 3;
    double min = 0;
    Point min_loc = 0;

    // Blur image to avoid noise
    GaussianBlur(image, blur, Size(kernel_blur, kernel_blur), 0, 0);

    // Extract max in prevision
    cvtColor(blur, gray, COLOR_BGR2GRAY);
    minMaxLoc(gray, &min, &max, &min_loc, &max_loc);
}

// Region of interest near of the maximum localization
Mat set_roi(Mat& original, Point max_loc){
    // Initialize variable to keep tracking on the same bottle
    int coeff = max_loc.y/2 + HEIGHT_IMAGE/2;

    // Initialize rectangle
    Rect selection(max_loc.x - coeff/2, max_loc.x + coeff/2, max_loc.y - coeff/2, max_loc.y + coeff/2);

    // Return new region of interest
    return LoadedImage(Rec);
}

// LEDs management
void led_enable(bool enable){
    digitalWrite(LED_PIN, enable);
}
