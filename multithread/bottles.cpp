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
    camera.set(CAP_PROP_CONTRAST, 65);
    camera.set(CAP_PROP_BRIGHTNESS, 40);
    camera.set(CAP_PROP_FPS, 7);

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

    // Set rectangle roi to limit image area
    Rect rect_roi;

    // If a bottle is already grab
    bool led_state = false;

    // Initialization of color threshold
    // Bottles
    int lower_hsv_bottles[][NB_CHANNELS] = {{0, 0, 0}};
    int upper_hsv_bottles[][NB_CHANNELS] = {{255, 255, 255}};

    // Beacons HSV
    int lower_beacon[][NB_CHANNELS] = {{0, 0, 0}};
    int upper_beacon[][NB_CHANNELS] = {{255, 255, 230}};

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

        deleted = del_color(roi, lower_beacon, upper_beacon);
        max_light_localization(deleted, max, max_loc, kernel_blur);
        region_of_interest = set_roi(image, max_loc, rect_roi, bottle_detected);
        filtered = del_color(region_of_interest, lower_hsv_bottles, upper_hsv_bottles);

        // Show results if needed
        if(show_results) {
            imshow("Original", image);
            imshow("Extract", filtered);
        }

        // Send position of bottle to arduino
        send_bottle_pos(ptr_uart0, bottle_pos);

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

// Localize the maximum of light in image
void max_light_localization(Mat& image, double& max, Point& max_loc, int kernel_blur){
    // Initialize matrices
    Mat gray;
    Mat blur;

    // Initialize variables
    double min = 0.0;
    Point min_loc;

    // Blur image to avoid noise
    GaussianBlur(image, blur, Size(kernel_blur, kernel_blur), 0, 0);

    // Extract max in prevision
    cvtColor(blur, gray, COLOR_BGR2GRAY);
    minMaxLoc(gray, &min, &max, &min_loc, &max_loc);

    // Show result
    circle(image, max_loc, 10, (0, 255, 255), 2);
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
    // If we do not have any bottles on image
    int test_bottle = 0;

    for(int i = 0; i < NB_CHANNELS; i++) {
	if(original.at<Vec3b>(max_loc)[i] < BOTTLE_THRESHOLD)
            test_bottle++;
    }

    if(test_bottle == NB_CHANNELS)
        return original;
    else
        bottle_detected = true;


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
Mat del_color(Mat& hsv, int lower[][NB_CHANNELS], int upper[][NB_CHANNELS]){
    // Initialization variables
    bool first = true;
    int nb_colors = sizeof(lower[0])/sizeof(int);
    Mat mask;
    Mat deleted;

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

    return deleted;
}

void send_bottle_pos(Uart* uart0, Point pos){
    string pos_to_send = "F_";

    // Add position of bottle
    pos_to_send += to_string(pos.x);
    pos_to_send += "_";
    pos_to_send += to_string(pos.y);
    pos_to_send += ".";

    uart0->transmit(pos_to_send);
}
