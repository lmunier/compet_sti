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
//    camera.set(CAP_PROP_EXPOSURE, RASPICAM_EXPOSURE_NIGHT);
    camera.set(CAP_PROP_FPS, MAX_FPS);

    camera.setAWB(RASPICAM_AWB_OFF);

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
    Mat kern = (Mat_<char>(3,3) <<  0, -1,  0,
                                   -1,  5, -1,
                                    0, -1,  0);

    Mat erode_kern = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

    Mat sharp;
    Mat roi;
    Mat image;
    Mat region_of_interest;
    Mat deleted;
    Mat test;

    // Different variables
    Uart *ptr_uart0 = static_cast<Uart*>(uart0);        // Initialize pointer to uart class
    bool show_results = true;                           // Show results if needed
    bool bottle_detected = false;                       // Set to one if we found a bottle
    bool led_state = false;                             // State of the led to toggle them
    bool beacon_detected = false;                       // Beacon is on image or not
    int kernel_blur = 7;                                // Set blur kernel
    Point bottle_pos;                                   // Set distance variables

    // Set rectangle roi to limit image area
    Rect rect_roi = Rect(0, HEIGHT_IMAGE/2-1, WIDTH_IMAGE, HEIGHT_IMAGE/2);

    // Extract max, min light in an image
    Point max_loc;
    double max = 0.0;

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
    } else {
        ptr_uart0->set_state_camera(true);
    }

    //-----------MAIN PART---------------
    while(false){
        // Turn on light if they are disabled
        if(!led_state){
            led_state = true;
            led_enable(led_state);
        }
        // Take video input
        camera.grab();
        camera.retrieve(image);

        #ifndef DISPLAY_IMAGE_RASPICAM
            imshow("Original", image);
        #endif

        // Compute on input frame to find bottles
        /*if(!bottle_detected) {
            roi = set_roi(image);
        } else {
            roi = image(rect_roi);
        }*/

        filter2D(image, sharp, -1, kern);
//        bilateralFilter (and_im, thresh, 5, 15, 6);
//        max_light_localization(sharp, max, max_loc, kernel_blur);

//        if(max >= NO_BOTTLE)
        erode(sharp, sharp, erode_kern);
        erode(sharp(rect_roi), test, erode_kern);
        test.copyTo(sharp(rect_roi));

//        deleted = check_bottle(camera, sharp, lower_beacon, upper_beacon, beacon_detected, led_state);
cout << "After delete" << endl;
        max_light_localization(deleted, max, max_loc, kernel_blur);

        cout << beacon_detected << endl;

        //region_of_interest = set_roi(image, max_loc, rect_roi, bottle_detected);
//        filtered = del_color(region_of_interest, lower_hsv_bottles, upper_hsv_bottles);

        // Show results if needed
        #ifdef DISPLAY_IMAGE_RASPICAM
            //imshow("Extract", region_of_interest);
            imshow("Sharp", sharp);

            circle(image, max_loc, 10, (0, 255, 255), 2);
            imshow("Final", image);
        #endif

        bottle_pos = max_loc;

        // Send position of bottle to arduino
        ptr_uart0->send_to_arduino('F', bottle_pos.x, bottle_pos.y);

        // Turn off light during align/shoot
        while(ptr_uart0->is_bottle()){
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
Mat check_bottle(RaspiCam_Cv& camera, Mat& original, int lower[][NB_CHANNELS], int upper[][NB_CHANNELS], bool& beacon, bool& led){
    // Initialize variables
    Mat hsv;
    Mat mask;
    Mat mask_not;
    Mat image;
    Mat new_image;
    Mat contours;

    double min = 0.0, max = 0.0;
    Point min_loc, max_loc;

    // Turn off light
    led = false;
    led_enable(led);
    sleep(0.5);

    // Take a new picture to avoid beacon on image
    camera.grab();
    camera.retrieve(image);

    // Turn on light
    led = true;
    led_enable(led);

    // Work on new image
    cvtColor(image, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(lower[0][HUE], lower[0][SAT], lower[0][VAL]),
                 Scalar(upper[0][HUE], upper[0][SAT], upper[0][VAL]), mask);

    bitwise_not(mask, mask_not);

    if(countNonZero(mask) > 0){
        // Find contours on our mask
        bitwise_and(original, original, new_image, mask_not);

        #ifndef DISPLAY_IMAGE_RASPICAM
            imshow("Image sub", new_image);
        #endif

        beacon = true;
    } else {
        beacon = false;
    }


    if(beacon)
        return new_image;
    else
        return original;
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
cout << max << endl;
    // Threshold to avoid false positiv
    if(max <= NO_BOTTLE)
        max_loc = Point(0,0);
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
