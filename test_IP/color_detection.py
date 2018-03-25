# python dynamic_color_tracking.py --filter HSV --webcam

import cv2
import time
import argparse
import numpy as np
from imutils.video import VideoStream


DELAY_BLUR = 100
MAX_KERNEL_LENGTH = 9


def callback(value):
    pass


def setup_trackbars(range_filter):
    cv2.namedWindow("Trackbars", 0)

    for i in ["MIN", "MAX"]:
        v = 0 if i == "MIN" else 255

        for j in range_filter:
            cv2.createTrackbar("%s_%s" % (j, i), "Trackbars", v, 255, callback)


def get_arguments():
    ap = argparse.ArgumentParser()
    ap.add_argument('-f', '--filter', required=True,
                    help='Range filter. RGB or HSV')
    ap.add_argument('-w', '--webcam', required=False,
                    help='Use webcam', action='store_true')
    ap.add_argument('-p', '--picamera', type=int, default=-1,
                    help='Whether or not the raspberry Pi camera should be used')
    args = vars(ap.parse_args())

    if not args['filter'].upper() in ['RGB', 'HSV']:
        ap.error("Please speciy a correct filter.")

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
    range_filter = args['filter'].upper()

    if args['webcam']:
        camera = cv2.VideoCapture(0)
    elif args['picamera']:
        camera = VideoStream(usePiCamera=args['picamera'] > 0).start()
        times.sleep(2.0)

    setup_trackbars(range_filter)

    while True:
        if args['webcam']:
            ret, image = camera.read()

            if not ret:
                break
        elif args['picamera']:
            image = camera.read()

        if range_filter == 'RGB':
            frame_to_thresh = image.copy()
        else:
            frame_to_thresh = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

        v1_min, v2_min, v3_min, v1_max, v2_max, v3_max = get_trackbar_values(range_filter)

        # # Smoothing
        # dst = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        # dst = cv2.inRange(dst, (v1_min, v2_min, v3_min), (v1_max, v2_max, v3_max))
        #
        #
        # for i in range(1, MAX_KERNEL_LENGTH, 2):
        #     dst = cv2.medianBlur(dst, i)
        #
        # cv2.imshow("Smoothing", dst)

        thresh = cv2.inRange(frame_to_thresh, (v1_min, v2_min, v3_min), (v1_max, v2_max, v3_max))

        kernel = np.ones((5, 5), np.uint8)
        mask = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel)
        mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)

        # find contours in the mask and initialize the current
        # (x, y) center of the ball
        cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
        contour = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        #cv2.imshow("Contour", contour)
        #center = None

        # only proceed if at least one contour was found
        if len(cnts) > 0:
            # find the largest contour in the mask, then use
            # it to compute the minimum enclosing circle and
            # centroid
            c = max(cnts, key=cv2.contourArea)
            ((x, y), radius) = cv2.minEnclosingCircle(c)
            M = cv2.moments(c)
            center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))

            # only proceed if the radius meets a minimum size
            if radius > 10:
                # draw the circle and centroid on the frame,
                # then update the list of tracked points
                cv2.circle(image, (int(x), int(y)), int(radius), (0, 255, 255), 2)
                cv2.circle(image, center, 3, (0, 0, 255), -1)
                cv2.putText(image, "centroid", (center[0] + 10, center[1]), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 0, 255),
                            1)
                cv2.putText(image, "(" + str(center[0]) + "," + str(center[1]) + ")", (center[0] + 10, center[1] + 15),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 0, 255), 1)

        # show the frame to our screen
        cv2.imshow("Original", image)

        opening = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel)
        opening = cv2.morphologyEx(opening, cv2.MORPH_CLOSE, kernel)
        cv2.imshow("Thresh", thresh)
        cv2.imshow("Opening", opening)
        #cv2.imshow("Mask", mask)

        # create a CLAHE object (Arguments are optional).
        #clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(4, 4))
        #image_equ = image.copy()

        #clahe_saturation = cv2.equalizeHist(frame_to_thresh[1])
        #image_equ[1] = clahe_saturation

        #clahe_luminosite = cv2.equalizeHist(frame_to_thresh[2])
        #image_equ[2] = clahe_luminosite

        #cv2.imshow("Equalization", image_equ)

        # Test detection with mask
        #image_detect = cv2.bitwise_and(image, image, mask=thresh)
        #cv2.imshow("Test detection", image_detect)

        if cv2.waitKey(1) & 0xFF is ord('q'):
            break


if __name__ == '__main__':
    main()
