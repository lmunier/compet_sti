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
    camera.set(CAP_PROP_CONTRAST, 65);
    camera.set(CAP_PROP_BRIGHTNESS, 40);

    return camera;
}

// LEDs management
void led_enable(bool enable){
    digitalWrite(LED_PIN, enable);
}

// OpenCV function to detect bottles
int bottles_scanning(){
    // Initialize variables
    Mat image;
    Mat region_of_interest;
    Mat filtered;
    Mat deleted;

    // Initialization of variables
        // Set distance variables
        int bottle_x_pos = 0;
        int bottle_y_pos = 0;

        // Set blur kernel
        int kernel_blur = 3;

        // Extract max, min
        Point max_loc;
        double max = 0.0;

    // Initialization of color threshold
        // Bottles
        int lower_hsv_bottles[] = {0, 0, 0};
        int upper_hsv_bottles[] = {255, 255, 255};

        // Beacons
//        int lower_beacon[NB_COLORS][NB_CHANNELS] = {{176, 218, 255}, {196, 252, 255},
//                                                    {220, 235, 255}, {200, 245, 222}};
//        int upper_beacon[NB_COLORS][NB_CHANNELS] = {{206, 244, 255}, {214, 255, 255},
//                                                    {245, 255, 245}, {220, 255, 235}};

        // Beacons HSV
        int lower_beacon[1][NB_CHANNELS] = {{0, 0, 0}};
        int upper_beacon[1][NB_CHANNELS] = {{255, 255, 230}};

    wiringPiSetup();
    init_pins();

    // Initialize camera
    RaspiCam_Cv camera = init_raspicam();

    // Open camera
    if(!camera.open()) {
        cout << "ERROR: can not open camera" << endl;
        return -1;
    }

    // Turn on light
    led_enable(true);

    while(true){
        // Take video input
        camera.grab();
        camera.retrieve(image);

        deleted = del_color(image, lower_beacon, upper_beacon);
        max_light_localization(deleted, max, max_loc, kernel_blur);
        region_of_interest = set_roi(image, max_loc);
        filtered = extract_color(region_of_interest, lower_hsv_bottles, upper_hsv_bottles);

//        imshow("Original", image);
        imshow("Deleted", deleted);
//        imshow("ROI", region_of_interest);
        imshow("Extract", filtered);

        if(waitKey(10) == 'q')
            break;
    }

    // Turn off light
    led_enable(false);

    return 1;
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
Mat set_roi(Mat& original, Point max_loc){
    // If we do not have any bottles on image
    if(max_loc.x < BOTTLE_TOLERANCE && max_loc.y < BOTTLE_TOLERANCE)
        return original;

    // Initialize variable to keep tracking on the same bottle
    int x_start = 0;
    int y_start = 0;
    int coeff = HEIGHT_IMAGE/2 + max_loc.y/2;

    // Initialize rectangle
    if(max_loc.x - coeff/2 < 0)
        x_start = 0;
    else if(max_loc.x + coeff/2 >= original.cols)
        x_start = original.cols - coeff - 1;
    else
        x_start = max_loc.x - coeff/2;

    if(max_loc.y - coeff/2 < 0)
        y_start = 0;
    else if(max_loc.y + coeff/2 >= original.rows)
        y_start = original.rows - coeff - 1;
    else
        y_start = max_loc.y - coeff/2;

    Rect selection(x_start, y_start, coeff, coeff);

    // Return new region of interest
    return original(selection);
}

// Extract searching beacon led color
Mat extract_color(Mat& original, int lower[], int upper[]){
    // Initialize extracted color matrix
    Mat mask;
    Mat hsv;
    Mat extracted;

    // Change BGR to HSV mode for our image
    cvtColor(original, hsv, COLOR_BGR2HSV);

    // Create and apply mask to our image
    inRange(hsv, Scalar(lower[HUE], lower[SAT], lower[VAL]),
                 Scalar(upper[HUE], upper[SAT], upper[VAL]), mask);
    bitwise_and(original, original, extracted, mask);

    return extracted;
}

// Delete color of beacons
Mat del_color(Mat& original, int lower[][NB_CHANNELS], int upper[][NB_CHANNELS]){
    // Initialization variables
    bool first = true;
    int nb_colors = sizeof(lower[0])/sizeof(int);
    Mat mask;
    Mat deleted;

    // Delete each color separately
    for(int i = 0; i < nb_colors; i++){
        inRange(original, Scalar(lower[i][BLUE], lower[i][GREEN], lower[i][RED]),
                          Scalar(upper[i][BLUE], upper[i][GREEN], upper[i][RED]), mask);

//        bitwise_not(mask, mask);

        imshow("Mask", mask);
        if(first) {
            bitwise_and(original, original, deleted, mask);
            first = false;
        }
        else
            bitwise_and(deleted, deleted, deleted, mask);
    }

    return deleted;
}
