//Module:		    bottles.cpp
//version:		    1.0
//Update:           06.06.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the bottles to throw them.
 */

#include "bottles.h"

// Pins initialization
void init_pins(){
    pinMode(LED_PIN, OUTPUT);
}

// Raspicam initialization
RaspiCam_Cv init_raspicam(){
    RaspiCam_Cv camera;
    sleep(WAIT_WAKEUP_CAMERA);

    camera.set(CAP_PROP_FRAME_HEIGHT, HEIGHT_IMAGE);
    camera.set(CAP_PROP_FRAME_WIDTH, WIDTH_IMAGE);
    camera.set(CAP_PROP_CONTRAST, CONTRAST);
    camera.set(CAP_PROP_BRIGHTNESS, BRIGHTNESS);
    camera.set(CAP_PROP_FPS, MAX_FPS);

    return camera;
}

// LEDs management
void led_enable(bool enable){
    digitalWrite(LED_PIN, enable);
}

// OpenCV function to detect bottles
void* bottles_scanning(void* uart0){
    //-----------INIT VARIABLES------------
    // Matrices
    Mat kern = (Mat_<char>(3,3) <<   0, -1,  0,
                                    -1,  5, -1,
                                     0, -1,  0);

    /*Mat kern = (Mat_<char>(3,3) <<  -1, -1, -1,
                                    -1,  8, -1,
                                    -1, -1, -1);*/

    Mat kern_dilate = (Mat_<char>(5, 5) <<  1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1);

    Mat sharp;
    Mat and_im;
    Mat thresh;
    Mat gray;
    Mat result;
    Mat roi;
    Mat image;
    Mat region_of_interest;
    Mat filtered;
    Mat deleted;

    // Initialize pointer to uart class
    Uart *ptr_uart0 = static_cast<Uart*>(uart0);

    // Show results if needed
    bool show_results = true;

    // Set to one if we found a bottle
    bool bottle_detected = false;

    // Set distance variables
    Point bottle_pos;

    // Set blur kernel
    int kernel_blur = 3;

    // Extract max, min light in an image
    Point max_loc;
    double max = 0.0;
    double check_max = 0.0;

    // Set rectangle roi to limit image area
    Rect rect_roi;

    // If a bottle is already grab
    bool led_state = false;

    // Initialization of color threshold
    // Beacons
    int lower_beacon[][NB_CHANNELS] = {{80, 0, 0}};
    int upper_beacon[][NB_CHANNELS] = {{255, 255, 255}};

    //-----------INITIALIZE UTILS--------
    wiringPiSetup();
    init_pins();

    // Initialize camera
    RaspiCam_Cv camera = init_raspicam();

    // Open camera
    if(!camera.open()) {
        cout << "ERROR: can not open camera" << endl;
    }


    //-----------MAIN PART---------------
    while(true){
        // Turn on light if they are disabled
        if(!led_state) {
            led_state = true;
            led_enable(led_state);
        }

        // Take video input
        camera.grab();
        camera.retrieve(image);

        // Compute on input frame to find bottles
        if(!bottle_detected) {
            roi = set_roi(image);
        } else {
            roi = image(rect_roi);
        }

//        deleted = del_color(image, lower_beacon, upper_beacon);
        filter2D(roi, sharp, -1, kern);
//        bilateralFilter (and_im, thresh, 5, 15, 6);
        max_light_localization(sharp, max, max_loc, kernel_blur);

        check_max = max;
        if(check_max >= 150.0)
            check_bottle(roi, lower_beacon, upper_beacon);

        region_of_interest = set_roi(image, max_loc, rect_roi, bottle_detected);
//        filtered = del_color(region_of_interest, lower_hsv_bottles, upper_hsv_bottles);

        // Show results if needed
        if(show_results) {
            imshow("Extract", region_of_interest);

            circle(image, max_loc, 10, (0, 255, 255), 2);
            imshow("Original", image);
        }

        bottle_pos = max_loc;

        // Send position of bottle to arduino
        //cout << "F_" << bottle_pos.x << "_" << bottle_pos.y << endl;
        ptr_uart0->send_to_arduino('F', bottle_pos.x, bottle_pos.y);

        // Turn off light during align/shoot
        while(ptr_uart0->is_bottle()) {
            if(led_state) {
                led_state = false;
                led_enable(led_state);
            }
        }
    }

    // Turn off light
    led_enable(false);

    pthread_exit(NULL);
}

// Check if we don't have beacon on image
bool check_bottle(RaspiCam_Cv camera, int lower[][NB_CHANNELS], int upper[][NB_CHANNELS]){
    // Initialize variables
    Mat hsv;
    Mat mask;

    bool beacon = false;
    double min = 0.0, max = 0.0;
    Point min_loc, max_loc,

    // Turn off light
    led_enable(false);
    sleep(0.05);

    // Take a new picture to avoid beacon on image
    camera.grab();
    camera.retrieve(image);

    // Turn on light
    led_enable(true);

    // Work on new image
    cvtColor(image, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(lower[0][HUE], lower[0][SAT], lower[0][VAL]),
                 Scalar(upper[0][HUE], upper[0][SAT], upper[0][VAL]), mask);

    // Find max to localize beacon position
    minMaxLoc(mask, min, max, min_loc, max_loc);

    if(max > 0.0){
//        rectangle(image, (0,(510,128),(0,255,0),3)

cout << beacon << endl;
    return beacon;
}

// Localize the maximum of light in image
void max_light_localization(Mat& image, double& max, Point& max_loc, int kernel_blur){
    // Initialize variables
    Mat gray;
    Mat blur;
    Scalar mean;
    Scalar stddev;

    // Initialize variables
    double min = 0.0;
    Point min_loc;

    // Blur image to avoid noise
    cvtColor(image, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, Size(kernel_blur, kernel_blur), 0, 0);

    // Extract max to find bottle
    minMaxLoc(blur, &min, &max, &min_loc, &max_loc);
}

// Region of interest near of the maximum localization
Mat set_roi(Mat& original){
    // Initialize rectangle
    int x_start = 0;
    int y_start = 0;
    int height = HEIGHT_IMAGE - AVOID_NOISE - 1;
    int width = WIDTH_IMAGE - 1;

    Rect selection(x_start, y_start, width, height);

    // Return new region of interest
    return original(selection);
}

// Region of interest near of the maximum localization
Mat set_roi(Mat& original, Point max_loc, Rect& rect_roi, bool& bottle_detected){
    // Initialize variable to keep tracking on the same bottle
    int coeff = (HEIGHT_IMAGE + max_loc.y)/2;

    // Initialize rectangle
    if(max_loc.x - coeff/2 <= 0)
        rect_roi.x = 0;
    else if(max_loc.x + coeff/2 >= WIDTH_IMAGE)
        rect_roi.x = WIDTH_IMAGE - coeff - 1;
    else
        rect_roi.x = max_loc.x - coeff/2;

    if(max_loc.y - coeff/2 <= 0)
        rect_roi.y = 0;
    else if(max_loc.y + coeff/2 >= HEIGHT_IMAGE)
        rect_roi.y = HEIGHT_IMAGE - coeff - 1;
    else
        rect_roi.y = max_loc.y - coeff/2;

    rect_roi.height = coeff;
    rect_roi.width = coeff;

    // Return new region of interest
    return original(rect_roi);
}

// Delete some color in HSV
Mat del_color(Mat& image, int lower[][NB_CHANNELS], int upper[][NB_CHANNELS]){
    // Initialization variables
    bool first = true;
    int nb_colors = sizeof(lower[0])/sizeof(int);

    Mat hsv;
    Mat mask;
    Mat deleted;

    cvtColor(image, hsv, COLOR_BGR2HSV);

    // Delete each color separately
    for(int i = 0; i < nb_colors; i++){
        inRange(hsv, Scalar(lower[i][HUE], lower[i][SAT], lower[i][VAL]),
                Scalar(upper[i][HUE], upper[i][SAT], upper[i][VAL]), mask);

        if(first) {
            bitwise_and(hsv, hsv, deleted, mask);
            first = false;
        }
        else
            bitwise_and(deleted, deleted, deleted, mask);
    }

    cvtColor(deleted, deleted, COLOR_HSV2BGR);
    return deleted;
}
