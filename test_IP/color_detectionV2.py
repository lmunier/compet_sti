# python dynamic_color_tracking.py --filter HSV --webcam

import cv2
import argparse
import numpy as np
from imutils.video import VideoStream
import imutils


DELAY_BLUR = 100
MAX_KERNEL_LENGTH = 9


def callback(value):
    pass


def setup_trackbars(range_filter):
    cv2.namedWindow("Trackbars", 0)
    v = [0, 0]

    for i in ["MIN", "MAX"]:
        f = 0
        for j in range_filter:
            if range_filter == "HSV":
                if f == 0:
                    v[0] = 40
                    v[1] = 255
                elif f == 1:
                    v[0] = 0
                    v[1] = 150
                else:
                    v[0] = 125
                    v[1] = 230
            elif range_filter == "BGR":
                if f == 0:
                    v[0] = 70
                    v[1] = 255
                elif f == 1:
                    v[0] = 0
                    v[1] = 255
                else:
                    v[0] = 0
                    v[1] = 255

            f += 1
            if i == "MIN":
                cv2.createTrackbar("%s_%s" % (j, i), "Trackbars", v[0], 255, callback)
            if i == "MAX":
                cv2.createTrackbar("%s_%s" % (j, i), "Trackbars", v[1], 255, callback)


def get_arguments():
    ap = argparse.ArgumentParser()
    ap.add_argument('-s', '--source', default='r', help="Chose source (raspicam, webcam or file")

    args = vars(ap.parse_args())

    return args


def get_trackbar_values(range_filter):
    values = []

    for i in ["MIN", "MAX"]:
        for j in range_filter:
            v = cv2.getTrackbarPos("%s_%s" % (j, i), "Trackbars")
            values.append(v)
    return values


def main():
    args = get_arguments()
    range_filter_HSV = "HSV"
    range_filter_RGB = "BGR"
    state = True

    if args['source'] == 'w':
        camera = cv2.VideoCapture(0)
        camera.set(cv2.CAP_PROP_BRIGHTNESS, 0.5)
    elif args['source'] == 'r':
        camera = VideoStream(usePiCamera=args['picamera'] > 0).start()
        time.sleep(2.0)
    elif args['source'] != ('r' or 'w'):
        # load the query image, compute the ratio of the old height to the new height, clone it, and resize it
        image = cv2.imread(args['source'])
        image = imutils.resize(image, height=300)
        image = cv2.blur(image, (3, 3))
        #image = cv2.convertScaleAbs(image, alpha=1.2, beta=-255)

    setup_trackbars(range_filter_HSV)
    setup_trackbars(range_filter_RGB)

    # kernel to filter noise
    kernel = np.ones((3, 3), np.uint8)


    while state:
        if args['source'] == 'w':
            ret, image = camera.read()
            image = cv2.blur(image, (5, 5))

            if not ret:
                break
        elif args['source'] == 'r':
            image = camera.read()
            image = cv2.blur(image, (5, 5))


        # contrast increasing
        cv2.imshow("Original", image)

        RGB_v1_min, RGB_v2_min, RGB_v3_min, RGB_v1_max, RGB_v2_max, RGB_v3_max = get_trackbar_values(range_filter_RGB)
        HSV_v1_min, HSV_v2_min, HSV_v3_min, HSV_v1_max, HSV_v2_max, HSV_v3_max = get_trackbar_values(range_filter_HSV)

        RGB_thresh = cv2.inRange(image, (RGB_v1_min, RGB_v2_min, RGB_v3_min), (RGB_v1_max, RGB_v2_max, RGB_v3_max))

        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        HSV_thresh = cv2.inRange(hsv, (HSV_v1_min, HSV_v2_min, HSV_v3_min), (HSV_v1_max, HSV_v2_max, HSV_v3_max))

        rgb_hsv = cv2.bitwise_and(image, image, mask=RGB_thresh)
        rgb_hsv = cv2.cvtColor(rgb_hsv, cv2.COLOR_BGR2HSV)
        rgb_hsv = cv2.inRange(rgb_hsv, (HSV_v1_min, HSV_v2_min, HSV_v3_min), (HSV_v1_max, HSV_v2_max, HSV_v3_max))

        # Show all images
        cv2.imshow("RGB", RGB_thresh)
        cv2.imshow("HSV", HSV_thresh)
        cv2.imshow("rgb_hsv", rgb_hsv)

        # show the frame to our screen
        opening = cv2.morphologyEx(rgb_hsv, cv2.MORPH_OPEN, kernel)
        opening = cv2.morphologyEx(opening, cv2.MORPH_CLOSE, kernel)
        cv2.imshow("Opening", opening)

        if cv2.waitKey(1) & 0xFF is ord('q'):
            break


if __name__ == '__main__':
    main()
