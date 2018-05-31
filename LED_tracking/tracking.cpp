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

int LED_tracking() {
    cv::VideoCapture webcam = init_webcam();

    if(!webcam.isOpened()){
        std::cout << "Can not open webcam." << std::endl;
        return -1;
    }

    // Initialization of matrices
    cv2::Mat image;
    cv2::Mat binary;
    cv2::Mat extracted;

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
            std::cout << "Webcam disconnected."<< std::endl;
            return -1;
        }

        extracted = extract_color(binary, lower, upper);
        binary = convert2binary(extracted);
        //extract_position(binary, dist_to_center, dist_to_corner);

        if(is_bottle_captured())
            std::cout << "Send to arduino " << dist_to_center << ", " << dist_to_corner << std::endl;

        if(is_aligned())
            std::cout << "Fire !!!" << std::endl;

        cv::imshow("Image", image);

        if (cv::waitKey(10) == 27)
        {
            std::cout << "Esc key is pressed by user. Stopping the video." << std::endl;
            break;
        }
    }

    return 0;
}

// Convert input image in binary image
cv2::Mat convert2binary(cv2::Mat image, double thresh, double maxValue){
    // Initialize binary matrix
    cv2::Mat binary;

    // Binary threshold on our image
    threshold(image, binary, thresh, maxValue, cv2.THRESH_BINARY);

    return binary;
}

// Extract searching beacon led color
cv2::Mat extract_color(cv2::Mat image, int lower[], int upper[]){
    // Initialize extracted color matrix
    cv2::Mat mask;
    cv2::Mat extracted;

    // Create and apply mask to our image
    cv2::inRange(image, lower, upper, mask);
    cv2::bitwise_and(image, image, extracted, mask);

    return extracted;
}

// Extract position of beacon led
void extract_position(cv2::Mat binary, int& center, int& corner){
    std::cout << "I try to detect corner LED" << std::endl;
}

// If  a bottle is captured
bool is_bottle_captured(){
    return true;
}

// Check if the robot is aligned
bool is_aligned(){
    return true;
}
