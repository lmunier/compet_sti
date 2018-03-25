import cv2
import numpy as np

img = cv2.imread('picture.jpg',0)
edges = cv2.Canny(img,100,200)

cv2.imwrite("edges_detect.jpg", edges)
