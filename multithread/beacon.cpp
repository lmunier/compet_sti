//Module:		    beacon.cpp
//version:		    1.0
//Update:           15.06.2018
//Author:	        Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

#include "beacon.h"

// Initialize pins stepper
Stepper init_stepper(){
    Stepper stepper_back(SB_IN1, SB_IN2, SB_IN3, SB_IN4);
    stepper_back.setRpm(V_MAX);

    return stepper_back;
}

// Initialize webcam
VideoCapture init_webcam(){
    VideoCapture webcam(0);

    sleep(WAIT_WAKEUP_WEBCAM);

    webcam.set(CAP_PROP_FRAME_HEIGHT, HEIGHT_IMAGE_WEBCAM);
    webcam.set(CAP_PROP_FRAME_WIDTH, WIDTH_IMAGE_WEBCAM);
    webcam.set(CAP_PROP_BRIGHTNESS, 0);
    webcam.set(CAP_PROP_GAIN, 0.1);

    webcam.set(CAP_PROP_FPS, MAX_FPS);

    return webcam;
}

// Main part, tracking of the corner led where is the bin
void* led_tracking(void* uart0) {
    // Initialization of matrices
    Mat image;
    Mat blur;
    Mat gray;
    Mat image_hsv;
    Mat extracted;

    // Initialize pointer to uart class
    Uart *ptr_uart0 = static_cast<Uart*>(uart0);

    // Initialization of color threshold
    // White leds
    int lower_hsv_yellow[] = {0, 0, 100};
    int upper_hsv_yellow[] = {50, 60, 140};

    // Initialization of variables
    bool calibration = false;           // Calibration is done
    bool obstacle = true;               // If obstacle between robot and beacon
    char direction = ' ';               // Direction character
    int led_x_pos = 0;                  // Set distance variables
    int height_beacon = 0;              // Height of the beacon
    int y_min = 0, y_max = 0;           // Position y at the min and max of the beacon
    int kernel_blur = 3;                // Set blur kernel

    wiringPiSetup();
    Stepper stepper_back = init_stepper();

    VideoCapture webcam = init_webcam();

    while(!webcam.isOpened()){
        webcam.open(0);
    }

    // Loop on led detection/alignment
    while(true){
        bool bSuccess = webcam.read(image);

        if(!bSuccess){
            cout << "Webcam disconnected." << endl;
        }

        cvtColor(image, image_hsv, COLOR_BGR2HSV);

        extracted = extract_color(image, image_hsv, lower_hsv_yellow, upper_hsv_yellow);
        medianBlur(extracted, blur, kernel_blur);
        extract_position(blur, led_x_pos);

        manage_stepper(stepper_back, led_x_pos);

        // Shooting procedure
        if(ptr_uart0->is_bottle() && calibration) {
            if(!is_aligned(stepper_back)) {
                sleep(0.1);
                if(stepper_back.getStep() < 0 && (direction == ' ' || direction == 'L')){
                    ptr_uart0->send_to_arduino('A', 'R');
                    direction = 'R';
                } else if (stepper_back.getStep() >= 0 && (direction == ' ' || direction == 'R')){
                    ptr_uart0->send_to_arduino('A', 'L');
                    direction = 'L';
                }
            } else if (obstacle) {
                height_beacon = extract_height(image, led_x_pos, y_min, y_max);

                if(height_beacon < BEACON_SIZE_MIN){
                    sleep(0.1);
                    ptr_uart0->send_to_arduino('A', 'O');
                }else
                    obstacle = false;
            }

            while (ptr_uart0->is_bottle() && is_aligned(stepper_back) && !obstacle) {
                ptr_uart0->send_to_arduino('A', 'F', get_dist_corner(y_min, y_max, 'y'));
                sleep(0.1);
            }

            // Set send_fire to false to shoot next bottle
            obstacle = true;
        }

        // Setting calibration to true and step to 0
        if(!calibration && digitalRead(OFF_BUTTON) == 0){
            stepper_back.setStepN(INITIAL_STEP);
            ptr_uart0->set_state_webcam(true);
            calibration = true;

            // Start arduino
            ptr_uart0->send_to_arduino('I');
        }

        // Show result
        #ifdef DISPLAY_IMAGE_WEBCAM
            imshow("Blur", blur);
            imshow("Image", image);
            waitKey(10);
        #endif

        sleep(0.1);
    }

    webcam.release();
    pthread_exit(NULL);
}

// Extract searching beacon led color
Mat extract_color(Mat& image, Mat& to_delete, int lower[], int upper[]){
    // Initialize extracted color matrix
    Mat mask;
    Mat extracted;

    // Create and apply mask to our image
    inRange(to_delete, Scalar(lower[HUE], lower[SAT], lower[VAL]),
                       Scalar(upper[HUE], upper[SAT], upper[VAL]), mask);
    bitwise_and(image, image, extracted, mask);

    return extracted;
}

// Region of interest near of the maximum localization
Rect get_roi(int x_max){
    // Initialize rectangle
    int x_start = x_max - BEACON_WIDTH_MIN/2;
    int y_start = 0;
    int height = HEIGHT_IMAGE_WEBCAM;
    int width = BEACON_WIDTH_MIN;

    if(x_start < 0)
        x_start = 0;
    else if(x_start >= WIDTH_IMAGE_WEBCAM - BEACON_WIDTH_MIN)
        x_start = WIDTH_IMAGE_WEBCAM - BEACON_WIDTH_MIN - 1;

    Rect selection(x_start, y_start, width, height);

    // Return new region of interest
    return selection;
}

// Extract position of beacon led
void extract_position(Mat& image, int& x_max){
    // Initialize matrices
    Mat gray;

    // Extract max, min
    Point min_loc, max_loc;
    double min = 0.0, max = 0.0;

    // Set roi to improve computation and avoid noisy detection
    Rect roi(0, 0, WIDTH_IMAGE_WEBCAM, HEIGHT_IMAGE_WEBCAM/2);

    // Extract max
    cvtColor(image(roi), gray, COLOR_BGR2GRAY);
    minMaxLoc(gray, &min, &max, &min_loc, &max_loc);

    // Show result
    #ifdef DISPLAY_IMAGE_WEBCAM
        circle(image, max_loc, 10, (0, 255, 255), 2);
    #endif

    x_max = max_loc.x;
}

// Return height of beacon led
int extract_height(Mat& image, int x, int& y_min, int& y_max){
    // Initialize variables
    int nbr = 0;
    int hole = 0;
    int x_min = 0, x_max = 0, x_tmp = 0;

    // Extract hight of beacon
    for(int row = 0; row < HEIGHT_IMAGE_WEBCAM; row++){
        nbr = 0;
        x_tmp = 0;

        for(int col = x - TOL_BEACON; col < x + TOL_BEACON; col++){
            if(col < 0)
                col = 0;
            else if (col >= WIDTH_IMAGE_WEBCAM)
                break;

            if((int) image.at<Vec3b>(row, col)[RED] >= 100) {
                x_tmp += col;
                nbr++;
            }
        }

        if (x_min == 0 && x_tmp > 0) {
            x_min = x_tmp/nbr;
            y_min = row;
        } else if (x_min > 0 && x_tmp > 0) {
            x_max = x_tmp/nbr;
            y_max = row;
        } else if (x_min > 0 && x_tmp == 0) {
            hole++;
        }

        if(hole == TOL_HOLE)
             break;
    }

    return y_max - y_min;
}

// Give distance to corner
double get_dist_corner(int pixel_min, int pixel_max, char function){
    double x, x_2, x_3;

    switch(function){
        case 'h':
            x = pixel_max - pixel_min;
            break;
        case 'y':
            x = pixel_max;
            break;
        default:
            return ERROR_DIST;
    }

    x_2 = x * x;
    x_3 = x_2 * x;

    switch(function) {
        case 'h':
            return -3.9789e-7 * x_3 + 3.5671e-4 * x_2 - 0.1134 * x + 14.9132;
        case 'y':
            return 1.241e-6 * x_3 - 1.5465e-4 * x_2 + 0.0187 * x + 1.9045;
    }

    return ERROR_DIST;
}

// Manage stepper back
void manage_stepper(Stepper& stepper_back, int led_x_pos){
    stepper_back.PID_orientation(led_x_pos);

    bool clockwise = false;
    int new_step = 0;
    int step_to_do = stepper_back.get_step_to_do();
    int step = stepper_back.getStep();

    if(WIDTH_IMAGE_WEBCAM/2 - led_x_pos < 0){
        clockwise = true;
        new_step = step + step_to_do;
    } else {
        clockwise = false;
        new_step = step - step_to_do;
    }

    stepper_back.moveTo(clockwise, new_step);
}

// Check if the robot is aligned
bool is_aligned(Stepper stepper_back){
    int actual_step = stepper_back.getStep();
    return (actual_step < TOL_ALIGN) || ((STEP_MAX - actual_step) < TOL_ALIGN);
}
