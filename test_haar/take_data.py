#!/usr/bin/python
# -*- coding:Utf-8 -*-
#lm030518.1637

# This file help us to make our dataset with a lot of pictures


import time
import argparse
import numpy as np
import cv2 as cv

from imutils.video import VideoStream
from imutils.video import FPS


def take_args():
	ap = argparse.ArgumentParser()
	ap.add_argument("-d", "--data", default='p', help="Photo or Video to save")
	ap.add_argument("-c", "--camera", default='r', help="Raspicam or Webcam")
	ap.add_argument("-p", "--preview", default='n', help="Would you like to preview data ?")
	args = vars(ap.parse_args())

	return args


def init_webcam(data):
	if data["camera"] == 'w':
		vs = cv.VideoCapture(0)
		vs.set(cv.CAP_PROP_FRAME_WIDTH, 640)
		vs.set(cv.CAP_PROP_FRAME_HEIGHT, 480)
	else:
		vs = VideoStream(usePiCamera=True).start()
		vs.resolution = (640, 480)

	time.sleep(0.5)

	if data["data"] == 'v':
		# Define the codec and create VideoWriter object
		fourcc = cv.VideoWriter_fourcc(*'FMP4')
		out = cv.VideoWriter('output.h264',fourcc, 20.0, (640, 480))
	else:
		out = None

	return vs, out


if __name__ == "__main__":
	args = take_args()
	vs, out = init_webcam(args)
	img = 1

	while True:
		if args["camera"] == 'w':
			ret_val, image = vs.read()
		else:
			image = vs.read()

		image = cv.flip(image, flipCode=-1)

		# Write on the file
		if args["data"] == 'v':
			image = cv.flip(image, 0)
			out.write(image)

		# Preview if it is demanded
		if args["preview"] == 'y':
			cv.imshow("Preview", image)
			key = cv.waitKey(1) & 0xFF
		else:
			key = input("Please insert a key")

		# Save if we press 's' or quit with 'q'
		if key == ord('s') and args["data"] is not 'v':
			file = '/home/pi/dataset/data170518/data{}.jpg'.format(img)
			cv.imwrite(file, image)
			img += 1
		elif key == ord('q') or key == 'q':
			break

	# Release all components
	if args["camera"] == 'w':
		vs.release()
	else:
		vs.stop()

	if args["data"] == 'v':
		out.release()

	cv.destroyAllWindows()
