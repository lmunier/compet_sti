#!/usr/bin/python
# -*- coding:Utf-8 -*-
#lm030518.1637

# This file help us to make our dataset with a lot of pictures


import io
import time
import argparse
import numpy as np
import cv2 as cv
import RPi.GPIO as GPIO
import picamera
import picamera.array

GPIO.setmode(GPIO.BCM)  # set board mode to Broadcom
GPIO.setup(17, GPIO.OUT)  # set up pin 17

from imutils.video import VideoStream
from imutils.video import FPS


img = 1

with picamera.PiCamera() as camera:
	camera.resolution = (640, 480)
	camera.start_preview()
	time.sleep(2)

	while(True):
		with picamera.array.PiRGBArray(camera) as stream:
			camera.capture(stream, format='bgr')
			image = stream.array

			key = input()

			if key == ord('s') or key == 's':
#				file = '/home/pi/dataset/data170518/data{}.jpg'.format(img)
				file = '/home/pi/environment{}.jpg'.format(img)
				cv.imwrite(file, image)
				img += 1
			elif key == ord('q') or key == 'q':
				break

			key = 0
