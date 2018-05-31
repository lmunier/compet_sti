//Module:		    tracking.h
//version:		    1.0
//Update:           15.05.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

#include "tracking.h"

// Initialize webcam
VideoCapture init_webcam(){
    VideoCapture webcam(0);

    webcam.set(CAP_PROP_FRAME_HEIGHT, 320);
    webcam.set(CAP_PROP_FRAME_WIDTH, 640);

    return webcam;
}

// Main part, tracking of the corner led where is the bin

int led_tracking() {
    VideoCapture webcam = init_webcam();

    if(!webcam.isOpened()){
        cout << "Can not open webcam." << endl;
        return -1;
    }

    // Initialization of matrices
    Mat image;
    Mat binary;
    Mat extracted;

    // Initialization of color threshold
        // Green leds
        int lower[] = {170, 240, 170};
        int upper[] = {255, 255, 215};

    // Initialization of variables
        // Set distance variables
        int dist_to_corner = 0;
        int dist_to_center = 0;

        // Set threshold and maxValue
        double thresh = 10;
        double maxValue = 255;

    while(true){
        bool bSuccess = webcam.read(image);

        if(!bSuccess){
            cout << "Webcam disconnected."<< endl;
            return -1;
        }

        extracted = extract_color(image, lower, upper);
        binary = convert2binary(extracted, thresh, maxValue);
        //extract_position(binary, dist_to_center, dist_to_corner);

        if(is_bottle_captured())
            cout << "Send to arduino " << dist_to_center << ", " << dist_to_corner << endl;

        if(is_aligned())
            cout << "Fire !!!" << endl;

        imshow("Image", image);

        if (waitKey(10) == 27)
        {
            cout << "Esc key is pressed by user. Stopping the video." << endl;
            break;
        }
    }

    return 0;
}

// Convert input image in binary image
Mat convert2binary(Mat image, double thresh, double maxValue){
    // Initialize binary matrix
    Mat binary;

    // Binary threshold on our image
    threshold(image, binary, thresh, maxValue, THRESH_BINARY);

    return binary;
}

// Extract searching beacon led color
Mat extract_color(Mat image, int lower[], int upper[]){
    // Initialize extracted color matrix
    Mat mask;
    Mat extracted;

    // Create and apply mask to our image
    cv::inRange(image, Scalar(lower[BLUE], lower[GREEN], lower[RED]),
                Scalar(upper[BLUE], upper[GREEN], upper[RED]), mask);
    bitwise_and(image, image, extracted, mask);

    return extracted;
}

// Extract position of beacon led
void extract_position(Mat binary, int& center, int& corner){
    cout << "I try to detect corner LED" << endl;
}

// If  a bottle is captured
bool is_bottle_captured(){
    return true;
}

// Check if the robot is aligned
bool is_aligned(){
    return true;
}
