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

    webcam.set(CAP_PROP_FRAME_HEIGHT, HEIGHT_IMAGE);
    webcam.set(CAP_PROP_FRAME_WIDTH, WIDTH_IMAGE);

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
    Mat extracted;

    // Initialization of color threshold
        // Green leds
        int lower_green[] = {170, 240, 170};
        int upper_green[] = {255, 255, 215};

        // Green leds
        int lower_white[] = {250, 250, 250};
        int upper_white[] = {255, 255, 255};

    // Initialization of variables
        // Set distance variables
        double y = 0.0;
        double dist2corner = 0.0;
        int dist2center = 0;

    while(true){
        bool bSuccess = webcam.read(image);

        if(!bSuccess){
            cout << "Webcam disconnected."<< endl;
            return -1;
        }

        extracted = extract_color(image, lower_white, upper_white);
        y = extract_position(extracted, dist2center);

        dist2corner = get_dist_corner(y);

        if(is_bottle_captured())
            cout << "Send to arduino " << dist_to_center << ", " << dist_to_corner << endl;

        if(is_aligned(dist2center))
            cout << "Fire !!!" << endl;

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
Mat extract_color(Mat &image, int lower[], int upper[]){
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
int extract_position(Mat& image, int& center){
    // Initialize variables
    bool stop = false;
    int x_min = 0;
    int x_max = 0;
    int x = 0;
    int y = 0;

    // Find non zero to find the highest pixel
    for(int row = 0; row < HEIGHT_IMAGE; row++){
        for(int col = 0; col < WIDTH_IMAGE; col++){
            if((int) image.at<Vec3b>(row, col)[0] >= 250) {
                cout << "row " << row << "col " << col << "x_min " << x_min << "x_max " << x_max << " " << (int) image.at<Vec3b>(row, col)[0] << endl;

                if (x_min == 0) {
                    x_min = col;
                    stop = true;
                } else
                    x_max = col;
            }
        }
        y = row;

        if(stop)
            break;
        else
            x_min = 0;
    }

    if(x_max != 0)
        x = x_min + (x_max-x_min)/2;
    else
        x = x_min;

    // Change values for dist2center
    center = x;

    // Show result
    typedef Point_<uint16_t> Pixel;
    Pixel pix(x, y);
    circle(image, pix, 10, (0, 0, 255), 2);

    // Return y value
    return y;
}

// Give distance to corner
double get_dist_corner(double h_up_led){
    return (DIST_ZERO/H_ZERO)*h_up_led;
}

// If  a bottle is captured
bool is_bottle_captured(){
    return true;
}

// Check if the robot is aligned
bool is_aligned(int dist2center){
    return dist2center < TOLERANCE;
}