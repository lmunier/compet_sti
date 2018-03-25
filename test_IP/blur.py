import cv2
import numpy as np

img = cv2.imread('picture.jpg')
#kernel = np.ones((5,5),np.float32)/100
#dst = cv2.filter2D(img,-1,kernel)

# Convert BGR to HSV
hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

cv2.imwrite("test_blur.jpg", hsv)
