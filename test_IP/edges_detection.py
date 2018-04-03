# -*-coding:Utf-8 -*
#lm250318.

# File to test edges detection with Image Processing

import cv2
import time
import argparse
import numpy as np

from imutils.video import VideoStream
import imutils


def get_arguments():
    ap = argparse.ArgumentParser()
    ap.add_argument('-w', '--webcam', type=int, default=-1, help='Use webcam')
    ap.add_argument('-p', '--picamera', type=int, default=-1, help='Whether or not the raspberry Pi camera should be used')
    ap.add_argument('-q', '--query', required=False, help="Path to the query image")

    args = vars(ap.parse_args())

    return args


def main():
    args = get_arguments()
    state = True

    if args['webcam'] != -1:
        camera = cv2.VideoCapture(0)
    elif args['picamera'] != -1:
        camera = VideoStream(usePiCamera=args['picamera'] > 0).start()
        camera.resolution = (640, 480)
        camera.brightness = 25
        camera.framerate = 30
        time.sleep(2.0)
    elif args['query']:
        # load the query image, compute the ratio of the old height to the new height, clone it, and resize it
        image = cv2.imread(args['query'])
        image = imutils.resize(image, height=300)

    # kernel to filter noise
    kernel = np.ones((3, 3), np.uint8)

    while state:
        if args['webcam'] != -1:
            ret, image = camera.read()

            if not ret:
                break
        elif args['picamera'] != -1:
            image = camera.read()
        elif args['query']:
            state = False

        # show original image
        originale = image.copy()
        cv2.imshow("Originale", originale)

        # convert the image to grayscale, blur it, and find edges
        # in the image
        image = cv2.convertScaleAbs(image, beta=-255)
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

        thresh = cv2.inRange(hsv, (30, 70, 40), (150, 255, 255))
        opening = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel)
        opening = cv2.morphologyEx(opening, cv2.MORPH_CLOSE, kernel)

        hsv = cv2.bilateralFilter(opening, 15, 17, 17)  # bilateral filter to keep edges
        cv2.imshow("HSV", hsv)
        edged = cv2.Canny(hsv, 30, 200)

        # find contours in the edged image, keep only the largest
        # ones, and initialize our screen contour
        im2, cnts, hierarchy = cv2.findContours(edged.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        cnts.sort(key=cv2.contourArea, reverse=True)
        screenCnt = None

        # loop over our contours to find number of bottles in image
        nb_bottles = 0

        for c in cnts:
            # approximate the contour
            peri = cv2.arcLength(c, True)
            area = cv2.contourArea(c, True)
            approx = cv2.approxPolyDP(c, 0.03 * peri, True)

            # if our approximated contour has four points, then we can assume that we have found our screen
            if 50 <= area <= 5000 and 4 <= len(approx) <= 10 and cv2.isContourConvex(approx):
                nb_bottles += 1
                cv2.drawContours(image, [approx], -1, (0, 255, 0), 3)

        cv2.imshow("Edges", edged)
        cv2.imshow("Image with rectangle", image)
        print("Number of bottles: {}".format(nb_bottles))

        if args['query']:
            cv2.waitKey(0)

        if cv2.waitKey(1) & 0xFF is ord('q'):
            break


if __name__ == '__main__':
    main()
