#!/usr/bin/python
# -*-coding:Utf-8 -*
#lm020518.1510

# File to test haar-cascade filter on faces

import numpy as np
import cv2 as cv

face_cascade = cv.CascadeClassifier('/home/pi/opencv-3.4.1/data/haarcascades/haarcascade_frontalface_default.xml')
vs_webcam = cv.VideoCapture(0)

while True:
	ret_val, frame  = vs_webcam.read()
	gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)

	faces = face_cascade.detectMultiScale(gray, 1.3, 5)

	for (x, y, w, h) in faces:
		cv.rectangle(frame, (x, y), (x+w, y+h), (255, 0, 0), 2)
		roi_gray = gray[y:y+h, x:x+w]
		roi_color = frame[y:y+h, x:x+w]

	cv.imshow('Image', frame)

	key = cv.waitKey(1) & 0xFF

	# Quit if we press 'q'
	if key == ord('q'):
		break

cv.destroyAllWindows()
