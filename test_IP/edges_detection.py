# -*-coding:Utf-8 -*
#lm250318.

# File to test edges detection with Image Processing

import cv2
import time
import argparse
import numpy as np
from imutils.video import VideoStream


def get_arguments():
    ap = argparse.ArgumentParser()
    ap.add_argument('-w', '--webcam', required=False,
                    help='Use webcam', action='store_true')
    ap.add_argument('-p', '--picamera', type=int, default=-1,
                    help='Whether or not the raspberry Pi camera should be used')

    args = vars(ap.parse_args())

    return args


def main():
    args = get_arguments()

    if args['webcam']:
        camera = cv2.VideoCapture(0)
    elif args['picamera']:
        camera = VideoStream(usePiCamera=args['picamera'] > 0).start()
        time.sleep(2.0)

    while True:
        if args['webcam']:
            ret, image = camera.read()

            if not ret:
                break
        elif args['picamera']:
            image = camera.read()

        cv2.imshow("Originale", image)

        # convert the image to grayscale, blur it, and find edges
        # in the image
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        #gray = cv2.bilateralFilter(gray, 11, 17, 17)
        edged = cv2.Canny(gray, 100, 200)

        cv2.imshow("Grayscaled", edged)

        if cv2.waitKey(1) & 0xFF is ord('q'):
            break


if __name__ == '__main__':
    main()
