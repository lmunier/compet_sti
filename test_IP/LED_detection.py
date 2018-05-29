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
    lower = [170, 240, 170]
    upper = [255, 255, 215]

    args = get_arguments()
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

    while state:
        if args['webcam'] != -1:
            ret, image = camera.read()

            if not ret:
                break
        elif args['picamera'] != -1:
            image = camera.read()

        height, width, nb_color = image.shape
        print(width, height, nb_color)

        # create NumPy arrays from the boundaries
        lower = np.array(lower, dtype="uint8")
        upper = np.array(upper, dtype="uint8")

        # find the colors within the specified boundaries and apply
        # the mask
        mask = cv2.inRange(image, lower, upper)
        output = cv2.bitwise_and(image, image, mask=mask)

        cv2.imshow("images", output)

        gray = cv2.cvtColor(output, cv2.COLOR_BGR2GRAY)
        retval, binary = cv2.threshold(gray, 10, 255, cv2.THRESH_BINARY)
        nonzero = cv2.findNonZero(binary)

        #for element in nonzero:
        #    print(element)
        #print(nonzero[0][0][1])
        x = nonzero[0][0][0]
        y = nonzero[0][0][1]

        cv2.circle(output, (int(x), int(y)), 10, (0, 0, 255), 2)
        cv2.circle(output, (int(width/2), int(y)), 5, (0, 255, 255), 2)
        cv2.imshow("result", output)

        if cv2.waitKey(1) & 0xFF is ord('q'):
            break


if __name__ == '__main__':
    main()



