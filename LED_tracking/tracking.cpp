//Module:		    tracking.h
//version:		    1.0
//Update:           15.05.2018
//Responsability:	Munier Louis
/*Description :
 * Management of the camera who tracks the corner LED to throw the bottle in right place.
 */

#include "tracking.h"

int LED_tracking() {
    VideoCapture webcam = init_webcam();

    if(!webcam.isOpened()){
        std::cout << "Can not open webcam." << std::endl;
        return -1;
    }

    // Initialization of matrices
    cv2::Mat image;

    int dist_to_corner = 0;
    int dist_to_center = 0;
    int c = 0;

    while(true){
        bool bSuccess = webcam.read(image);

        if(!bSuccess){
            std::cout << "Webcam disconnected."<< std::endl;
            return -1;
        }
        //LED_detection(dist_to_center, dist_to_corner);

        if(is_bottle_captured())
            std::cout << "Send to arduino " << dist_to_center << ", " << dist_to_corner << std::endl;

        if(is_aligned())
            std::cout << "Fire !!!" << std::endl;

        imshow("Image", image);

        if (waitKey(10) == 27)
        {
            cout << "Esc key is pressed by user. Stopping the video." << endl;
            break;
        }
    }

    return 0;
}

VideoCapture init_webcam(){
    VideoCapture webcam(0);

    return webcam;
}

cv2::Mat convert2binary(cv2::Mat image){
    // Initialize binary matrix
    cv2::Mat binary;

    // Set threshold and maxValue
    double thresh = 0;
    double maxValue = 255;

    return binary;
}

void LED_detection(int& center, int& corner){
    std::cout << "I try to detect corner LED" << std::endl;
}

bool is_bottle_captured(){
    return true;
}

bool is_aligned(){
    return true;
}
