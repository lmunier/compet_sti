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
        int lower_hsv_bottles[] = {20, 0, 250};
        int upper_hsv_bottles[] = {60, 55, 255};

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
    i = 0;
    while(true){
        // Take video input
        camera.grab();
        camera.retrieve(image);

        max_light_localization(image, max, max_loc, kernel_blur);
        region_of_interest = set_roi(image, max_loc);
        filtered = extract_color(region_of_interest, lower_hsv_bottles, upper_hsv_bottles);
//        filtered = extract_color(image, lower_hsv_bottles, upper_hsv_bottles);

        imshow("Original", image);
        imshow("ROI", region_of_interest);
        imshow("Extract", filtered);

        if(waitKey(10) == 's'){
            imwrite("/home/pi/bottle" + std::to_string(i+1) + ".png", region_of_interest);
            i++
        }

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

    cout << "Coeff " << coeff << endl;
    cout << "Max_loc " << max_loc << endl;
    cout << "Rows " << original.rows << " Cols " << original.cols << endl;
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
    cout << "x_start" << x_start << "y_start" << y_start << endl;
    cout << "Rect" << selection << endl;
    // Return new region of interest
    return original(selection);
//    return original;
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
