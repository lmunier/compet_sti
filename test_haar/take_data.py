#!/usr/bin/python
# -*- coding:Utf-8 -*-
#lm030518.1637

# This file help us to make our dataset with a lot of pictures


import numpy as np
import cv2 as cv
from imutils.video import FPS


def init_webcam():
	vs_webcam = cv.VideoCapture(0)
	vs_webcam.set(cv.CAP_PROP_FRAME_WIDTH, 48)
	vs_webcam.set(cv.CAP_PROP_FRAME_HEIGHT, 24)

	return vs_webcam

if __name__ == "__main__":
	vs_webcam = init_webcam()
	img = 1

	while True:
		ret_val, image = vs_webcam.read()

		cv.imshow("Preview", image)

		key = cv.waitKey(1) & 0xFF

		# Save if we press 's' or quit with 'q'
		if key == ord('s'):
			file = '/home/pi/dataset/test_schorle/neg_schorle{}.jpg'.format(img)
			cv.imwrite(file, image)
			img += 1
		elif key == ord('q'):
			break

	cv.destroyAllWindows()
