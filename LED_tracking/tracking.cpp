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

//    stepper_back.setStepN(step);
//    stepper_back.setSeqN(0);

    return stepper_back;
}

// Initialize webcam
VideoCapture init_webcam(){
    VideoCapture webcam(0);

    webcam.set(CAP_PROP_FRAME_HEIGHT, HEIGHT_IMAGE);
    webcam.set(CAP_PROP_FRAME_WIDTH, WIDTH_IMAGE);

    return webcam;
}

// Main part, tracking of the corner led where is the bin

int led_tracking() {
    // Initialization of matrices
    Mat image;
    Mat extracted;

    // Initialization of color threshold
        // Green leds
        int lower_green[] = {170, 240, 170};
        int upper_green[] = {255, 255, 215};

        // White leds
        int lower_white[] = {250, 250, 250};
        int upper_white[] = {255, 255, 255};

        // Black leds
        int lower_black[] = {0, 0, 0};
        int upper_black[] = {40, 40, 40};

    // Initialization of variables
        // Set distance variables
        int led_y_pos = 0;
        int led_x_pos = 0;
        double dist2corner = 0.0;

        // Set step variable
        static int init_step = 0;
        int degree_correction = 0;

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

        extracted = extract_color(image, lower_white, upper_white);
        led_y_pos = extract_position(extracted, led_x_pos);

        dist2corner = get_dist_corner(led_y_pos);
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

        imshow("Image", image);
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
    inRange(image, Scalar(lower[BLUE], lower[GREEN], lower[RED]),
            Scalar(upper[BLUE], upper[GREEN], upper[RED]), mask);
    bitwise_and(image, image, extracted, mask);

    return extracted;
}

// Extract position of beacon led
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
            if((int) image.at<Vec3b>(row, col)[0] >= 250) {
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
    Pixel pix(x_pos, y_pos);
    circle(image, pix, 10, (0, 0, 255), 2);

    // Return y value
    return y_pos;
}

// Give distance to corner
int get_dist_corner(double h_up_led){
    return (int)((DIST_ZERO/H_ZERO)*h_up_led);
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
