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
	args = vars(ap.parse_args())

	return args


def init_webcam(data):
	if data["camera"] == 'w':
		vs = cv.VideoCapture(0)
		time.sleep(1.0)
		vs.set(cv.CAP_PROP_FRAME_WIDTH, 160)
		vs.set(cv.CAP_PROP_FRAME_HEIGHT, 120)
	else:
		vs = VideoStream(usePiCamera=True).start()
		time.sleep(1.0)
		vs.resolution = (160, 120)

	if data["data"] == 'v':
		# Define the codec and create VideoWriter object
		fourcc = cv.VideoWriter_fourcc(*'XVID')
		out = cv.VideoWriter('output.avi',fourcc, 20.0, (160,120))
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

		# Write on the file and show preview
		out.write(image)
		cv.imshow("Preview", image)

		key = cv.waitKey(1) & 0xFF

		# Save if we press 's' or quit with 'q'
		if key == ord('s') and args["data"] is not 'v':
			file = '/home/pi/dataset/test_schorle/neg_schorle{}.jpg'.format(img)
			cv.imwrite(file, image)
			img += 1
		elif key == ord('q'):
			break

	# Release all components
	vs.release()
	if args["data"] == 'v':
		out.release()

	cv.destroyAllWindows()
