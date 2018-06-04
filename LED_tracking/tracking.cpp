//Module:		    tracking.h
//version:		    1.0
//Update:           15.05.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

#include "tracking.h"

// Initialize pins stepper
CheapStepper init_stepper(int& init_step){
    CheapStepper stepper_back(SB_IN1, SB_IN2, SB_IN3, SB_IN4);
    init_step = stepper_back.getStep();

    return stepper_back;
}

// Initialize webcam
VideoCapture init_webcam(){
    VideoCapture webcam(0);

    webcam.set(CAP_PROP_FRAME_HEIGHT, HEIGHT_IMAGE);
    webcam.set(CAP_PROP_FRAME_WIDTH, WIDTH_IMAGE);
    webcam.set(CAP_PROP_BRIGHTNESS, 0.5);

    return webcam;
}

// Main part, tracking of the corner led where is the bin

int led_tracking() {
    // Initialization of matrices
    Mat image;
    Mat blur;
    Mat gray;
    Mat image_hsv;
    Mat extracted;

    // Initialization of color threshold
        // White leds
        int lower_hsv_yellow[] = {20, 0, 250};
        int upper_hsv_yellow[] = {60, 55, 255};

    // Initialization of variables
        // Set distance variables
        int led_y_pos = 0;
        int led_x_pos = 0;
        double dist2corner = 0.0;

        // Set step variable
        static int init_step = 0;
        int degree_correction = 0;


	// Set blur kernel
	int kernel_blur = 3;

        // Extract max, min
        //Point minLoc, maxLoc;
        //double min=0.0, max=0.0;

    wiringPiSetup();
    CheapStepper stepper_back = init_stepper(init_step);

    VideoCapture webcam = init_webcam();

    if(!webcam.isOpened()){
        cout << "Can not open webcam." << endl;
        return -1;
    }

    while(true){
        bool bSuccess = webcam.read(image);

        if(!bSuccess){
            cout << "Webcam disconnected."<< endl;
            return -1;
        }

        // Blur image to avoid noise
        GaussianBlur(image, blur, Size(kernel_blur, kernel_blur), 0, 0);

        // Extract max in prevision
        //cvtColor(blur, gray, COLOR_BGR2GRAY);
        //minMaxLoc(gray, &min, &max, &minLoc, &maxLoc);

        // Extracted color to detect LEDs
        cvtColor(blur,image_hsv, COLOR_BGR2HSV);
        extracted = extract_color(image_hsv, lower_hsv_yellow, upper_hsv_yellow);
        led_y_pos = extract_position(extracted, led_x_pos);

        dist2corner = get_dist_corner(led_y_pos, 'y');
        manage_stepper(stepper_back, led_x_pos);

        if(is_aligned(led_x_pos)) {
            cout << "Fire !!!" << endl; // TODO: Send to arduino "FIRE" with dist2corner

            degree_correction = (stepper_back.getStep() - init_step)*360/4096;
            cout << "Degree to correct." << degree_correction << endl;

            /*while (true) {// TODO: place checking that bottle is thrown

                if (waitKey(10) == 27) {
                    cout << "Esc key is pressed by user." << endl;
                    break;
                }
            }*/
        }

        if(is_bottle_captured()) {
            // TODO: Send to arduino led_x_pos to align
            cout << "Send to arduino " << led_x_pos << endl;
        }

        // Show result
        //circle(blur, maxLoc, 10, (255, 255, 0), 2);

        imshow("Image", blur);
        imshow("Extracted", extracted);

        if (waitKey(10) == 27)
        {
            cout << "Esc key is pressed by user. Stopping the video." << endl;
            break;
        }
    }

    return 0;
}

// Extract searching beacon led color
Mat extract_color(Mat& image, int lower[], int upper[]){
    // Initialize extracted color matrix
    Mat mask;
    Mat extracted;

    // Create and apply mask to our image
    inRange(image, Scalar(lower[HUE], lower[SAT], lower[VAL]),
            Scalar(upper[HUE], upper[SAT], upper[VAL]), mask);
    bitwise_and(image, image, extracted, mask);

    return extracted;
}

// Extract position of beacon led
int extract_position(Mat& image, int& center){
    // Initialize matrices
    Mat gray;

    // Extract max, min
    Point minLoc, maxLoc;
    double min=0.0, max=0.0;

    // Initialize variables
    bool stop = false;
    int x_min = 0;
    int x_max = 0;
    int x_pos = 0;
    int y_pos = 0;

    // Extract max
    cvtColor(image, gray, COLOR_BGR2GRAY);
    minMaxLoc(gray, &min, &max, &minLoc, &maxLoc);

    // Find non zero to find the highest pixel
    /*for(int row = 0; row < HEIGHT_IMAGE; row++){

        for(int col = 0; col < WIDTH_IMAGE; col++){
            if((int) image.at<Vec3b>(row, col)[0] >= 20) {
                if (x_min == 0) {
                    x_min = col;
                    stop = true;
                } else
                    x_max = col;
            }
        }
        y_pos = row;

        if(stop)
            break;
        else
            x_min = 0;
    }*/

    // Change values for dist2center
    center = x_pos;

    // Show result
    typedef Point_<uint16_t> Pixel;
    circle(image, maxLoc, 10, (0, 0, 255), 2);

    // Return y value
    return y_pos;
}

// Extract position of beacon led
//-------------------------OLD----------------------------
/*
int extract_position(Mat& image, int& center){
    // Initialize variables
    bool stop = false;
    int x_min = 0;
    int x_max = 0;
    int x_pos = 0;
    int y_pos = 0;

    // Find non zero to find the highest pixel
    for(int row = 0; row < HEIGHT_IMAGE; row++){
        for(int col = 0; col < WIDTH_IMAGE; col++){
            if((int) image.at<Vec3b>(row, col)[0] >= 20) {
                if (x_min == 0) {
                    x_min = col;
                    stop = true;
                } else
                    x_max = col;
            }
        }
        y_pos = row;

        if(stop)
            break;
        else
            x_min = 0;
    }

    if(x_max != 0)
        x_pos = x_min + (x_max-x_min)/2;
    else
        x_pos = x_min;

    // Change values for dist2center
    center = x_pos;

    // Show result
    typedef Point_<uint16_t> Pixel;
    Pixel pix_up(x_pos, y_pos);
    Pixel pix_down(x_pos, y_pos); 
    circle(image, pix_up, 10, (0, 0, 255), 2);
    circle(image, pix_down, 10, (0, 255, 255), 2);

    // Return y value
    return y_pos;
}*/
//--------------------------------------------------------

// Give distance to corner
//-----------------------OLD--------------------------
/*double get_dist_corner(int h_up_led){
    return (DIST_ZERO/H_ZERO)*h_up_led;
}*/
//-----------------------OLD--------------------------

// Give distance to corner
double get_dist_corner(int pixels, char function){
    double x = pixels;
    double x_2 = x * x;
    double x_3 = x_2 * x;

    switch(function) {
        case 'h':
            return -3.9789e-7 * x_3 + 3.5671e-4 * x_2 - 0.1134 * x + 14.9132;
        case 'y':
            return 1.241e-6 * x_3 - 1.5465e-4 * x_2 + 0.0187 * x + 1.9045;
        default :
            cout << "Wrong distance argument." << endl;
            break;
    }

    return -1.0;
}

// Manage stepper back
void manage_stepper(CheapStepper& stepper_back, int led_x_pos){
    int rpm = stepper_back.getRpm();
    stepper_back.PID_orientation(led_x_pos);

    for(int s=0; s<rpm/3; s++) {
        if (WIDTH_IMAGE / 2 - led_x_pos < 0)
            stepper_back.stepCW();
        else
            stepper_back.stepCCW();
    }
}

// If  a bottle is captured
bool is_bottle_captured(){
    return true;
}

// Check if the robot is aligned
bool is_aligned(int led_x_pos){
    return abs(led_x_pos - WIDTH_IMAGE/2) < TOLERANCE_ALIGN;
}
