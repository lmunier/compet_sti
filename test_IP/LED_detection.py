# python dynamic_color_tracking.py --filter HSV --webcam

import cv2
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
    # Definition of color tracking
    #lower = [170, 240, 170]
    #upper = [255, 255, 215]

#    lower = [0, 0, 254]
#    upper = [255, 10, 255]

    lower = [0, 0, 0]
    upper = [255, 110, 255]

    kernel = np.ones((3, 3), np.uint8)

    args = get_arguments()
    state = True
    x = 0
    y_up = 0
    y_down = 0

    if args['webcam'] != -1:
        camera = cv2.VideoCapture(0)
    elif args['picamera'] != -1:
        camera = VideoStream(usePiCamera=args['picamera'] > 0).start()
        time.sleep(2.0)
    elif args['query']:
        # load the query image, compute the ratio of the old height to the new height, clone it, and resize it
        image = cv2.imread(args['query'])
#        image = imutils.resize(image, height=320)


    while state:
        if args['webcam'] != -1:
            ret, image = camera.read()

            if not ret:
                break
        elif args['picamera'] != -1:
            image = camera.read()

        cv2.imshow("Image", image)
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

        height, width, color = image.shape
        print(height, width, color)
        # create NumPy arrays from the boundaries
        lower = np.array(lower, dtype="uint8")
        upper = np.array(upper, dtype="uint8")

        # find the colors within the specified boundaries and apply
        # the mask
        thresh = cv2.inRange(hsv, lower, upper)
        bitwise = cv2.bitwise_and(image, image, mask=thresh)

        opening = cv2.morphologyEx(bitwise, cv2.MORPH_OPEN, kernel)
        output = cv2.morphologyEx(opening, cv2.MORPH_CLOSE, kernel)
        cv2.imshow("images", output)

        for r in range(0, height-1):
            for c in range(0, width-1):
                if(output[r][c][0] >= 10):
                    x = c
                    y_up = r
                    break

            if x is not 0:
                break

        for r in range(0, height-1):
            for c in range(x-10, x+10):
                if(output[r][c][0] >= 10):
                    y_down = r

        print(x, y_up, y_down, y_up-y_down)
        cv2.circle(output, (int(x), int(y_up)), 10, (0, 0, 255), 2)
        cv2.circle(output, (int(x), int(y_down)), 5, (0, 255, 255), 2)
        cv2.imshow("result", output)

        if cv2.waitKey(1) & 0xFF is ord('q'):
            break


if __name__ == '__main__':
    main()



