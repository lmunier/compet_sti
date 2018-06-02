#!/usr/bin/python
# -*-coding:Utf-8 -*
#lm020518.1510

# File to test haar-cascade filter on faces

import numpy as np
import cv2 as cv

#image = cv.imread('/home/pi/EPFL-Media-Library/image32_largeprvw.jpeg',0)
image = cv.imread('/home/pi/video_data/jpg/rivella/rivella1018.jpg')
bottle_cascade = cv.CascadeClassifier('/home/pi/compet_sti/test_haar/data/cascade.xml')
bottle = bottle_cascade.detectMultiScale(image, 1.3, 5)

for (x, y, w, h) in bottle:
    cv.rectangle(image, (x, y), (x+w, y+h), (255, 0, 0), 2)
    roi_color = image[y:y+h, x:x+w]

cv.imshow('Image', image)
cv.waitKey(0)
