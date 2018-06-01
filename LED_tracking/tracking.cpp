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
        int dist2center = 0;
        int dist2corner = 0;

    while(true){
        bool bSuccess = webcam.read(image);

        if(!bSuccess){
            cout << "Webcam disconnected."<< endl;
            return -1;
        }

        extracted = extract_color(image, lower_white, upper_white);
        extract_position(extracted, dist2center, dist2corner);

        if(is_bottle_captured())
            //cout << "Send to arduino " << dist_to_center << ", " << dist_to_corner << endl;

        if(is_aligned())
            //cout << "Fire !!!" << endl;

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
void extract_position(Mat& image, int& center, int& corner){
    // Initialize variables
    typedef Point_<uint16_t> Pixel1;

    bool stop = false;
    auto x_min = 0.0;
    double x_max = 0.0;
    double x = 0.0;
    double y = 0.0;

    /*Rect roi1(0, 0, 10, 200);
    Mat sub(image, roi1);
    cout << sub << endl;*/

    // Find non zero to find the highest pixel
    for(int row = 0; row < HEIGHT_IMAGE; row++){
        for(int col = 0; col < WIDTH_IMAGE; col++){
            if(image.at<Vec3b>(row, col)[0] >= 250) {
                cout << "row " << row << "col " << col << "x_min " << x_min << "x_max " << x_max << " " << (int) image.at<Vec3b>(row, col)[0] << endl;

                if (x_min == 0.0) {
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
            x_min = 0.0;
    }

    if(x_max != 0)
        x = x_min + (x_max-x_min)/2;
    else
        x = x_min;

    dist_to

    Pixel1 pix(x, y);
    circle(image, pix, 10, (0, 0, 255), 2);
}

// Extract position of beacon led
/*void extract_position(Mat& image, int& center, int& corner){
    // Initialize variables
    Mat locations;
    typedef Point_<uint8_t> Pixel;

    auto x_min = 0.0;
    double x_max = 0.0;
    double y = 0.0;
    bool stop = false;

    Rect roi1(0, 0, 10, 200);
    Mat sub(image, roi1);
    cout << sub << endl;

    // ------------------- With just one line -------------------
    // Find non zero to find the highest pixel
    /*for(int col = 0; col < WIDTH_IMAGE; col++){
        if(image.at<Vec3b>(row, col)[0] >= 100) {
            cout << "row " << row << "col " << col << "x_min " << x_min << "x_max " << x_max << endl;
            cout << (int)image.at<Vec3b>(row, col)[0] << endl;

            if (x_min == 0.0)
                x_min = col;
            else
                x_max = col;
        }
    }*/
    //-----------------------------------------------------------

    /*image.forEach<Pixel>([&](Pixel &pixel, const int position[]) -> void {
        if (pixel.x == 255 && x_min == 0.0) {
            x_min = position[0];
            stop = true;
        } else if (pixel.x == 255)
            x_max = position[0];

        if (position[0] == WIDTH_IMAGE - 1){
            cout << "Coucou" << endl;

            if (!stop) {
                x_min = 0.0;
                y = position[1];
            } else
                return;
        }
    });

    x_min = locations.at<Point>(0).x;
    x_max = locations.at<Point>(0).x;
    y = locations.at<Point>(0).y;
    Pixel pix(x_min + (x_max-x_min)/2, y);

    circle(image, pix, 10, (0, 0, 255), 2);
    cout << "x_min " << x_min << " x_max " << x_max << " y " << y << endl;
}*/

// If  a bottle is captured
bool is_bottle_captured(){
    return true;
}

// Check if the robot is aligned
bool is_aligned(){
    return true;
}
