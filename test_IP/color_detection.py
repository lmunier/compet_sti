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

    for i in ["MIN", "MAX"]:
        v = 0 if i == "MIN" else 255

        for j in range_filter:
            cv2.createTrackbar("%s_%s" % (j, i), "Trackbars", v, 255, callback)


def get_arguments():
    ap = argparse.ArgumentParser()
    ap.add_argument('-f', '--filter', required=True, help='Range filter. RGB or HSV')
    ap.add_argument('-w', '--webcam', type=int, default=-1, help='Use webcam')
    ap.add_argument('-p', '--picamera', type=int, default=-1, help='Whether or not the raspberry Pi camera should be used')
    ap.add_argument('-q', '--query', required=False, help="Path to the query image")

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
    state = True

    if args['webcam'] != -1:
        camera = cv2.VideoCapture(0)
    elif args['picamera'] != -1:
        camera = VideoStream(usePiCamera=args['picamera'] > 0).start()
        time.sleep(2.0)
    elif args['query']:
        # load the query image, compute the ratio of the old height to the new height, clone it, and resize it
        image = cv2.imread(args['query'])
        image = imutils.resize(image, height=300)
        image = cv2.convertScaleAbs(image, alpha=1.2, beta=-255)

    setup_trackbars(range_filter)

    # kernel to filter noise
    kernel = np.ones((3, 3), np.uint8)

    while state:
        if args['webcam'] != -1:
            ret, image = camera.read()

            if not ret:
                break
        elif args['picamera'] != -1:
            image = camera.read()

        # contrast increasing
        cv2.imshow("Original", image)

        if range_filter == 'RGB':
            frame_to_thresh = image.copy()
        else:
            frame_to_thresh = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

        v1_min, v2_min, v3_min, v1_max, v2_max, v3_max = get_trackbar_values(range_filter)

        thresh = cv2.inRange(frame_to_thresh, (v1_min, v2_min, v3_min), (v1_max, v2_max, v3_max))

        # # find contours in the mask and initialize the current
        # # (x, y) center of the ball
        # cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
        # contour = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        # center = None
        #
        # # only proceed if at least one contour was found
        # if len(cnts) > 0:
        #     # find the largest contour in the mask, then use
        #     # it to compute the minimum enclosing circle and
        #     # centroid
        #     c = max(cnts, key=cv2.contourArea)
        #     ((x, y), radius) = cv2.minEnclosingCircle(c)
        #     M = cv2.moments(c)
        #     center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
        #
        #     # only proceed if the radius meets a minimum size
        #     if radius > 10:
        #         # draw the circle and centroid on the frame,
        #         # then update the list of tracked points
        #         cv2.circle(image, (int(x), int(y)), int(radius), (0, 255, 255), 2)
        #         cv2.circle(image, center, 3, (0, 0, 255), -1)
        #         cv2.putText(image, "centroid", (center[0] + 10, center[1]), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 0, 255),
        #                     1)
        #         cv2.putText(image, "(" + str(center[0]) + "," + str(center[1]) + ")", (center[0] + 10, center[1] + 15),
        #                     cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 0, 255), 1)

        # show the frame to our screen
        opening = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel)
        opening = cv2.morphologyEx(opening, cv2.MORPH_CLOSE, kernel)
        cv2.imshow("Thresh", thresh)
        cv2.imshow("Opening", opening)

        if cv2.waitKey(1) & 0xFF is ord('q'):
            break


if __name__ == '__main__':
    main()
