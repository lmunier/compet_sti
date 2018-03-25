# import the necessary packages
from __future__ import print_function
from imutils.video import VideoStream
import numpy as np
import argparse
import imutils
import time
import cv2

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-o", "--output", required=True,
	help="path to output video file")
ap.add_argument("-p", "--picamera", type=int, default=-1,
	help="whether or not the Raspberry Pi camera should be used")
ap.add_argument("-f", "--fps", type=int, default=20,
	help="FPS of output video")
ap.add_argument("-c", "--codec", type=str, default="MJPG",
	help="codec of output video")
args = vars(ap.parse_args())

# initialize the video stream and allow the camera
# sensor to warmup
print("[INFO] warming up camera...")
vs = VideoStream(usePiCamera=args["picamera"] > 0).start()
time.sleep(2.0)

# loop over frames from the video stream
while True:
	# grab the frame from the video stream and resize it to have a
	# maximum width of 300 pixels
	frame = vs.read()
#	frame = imutils.resize(frame, width=300)

	# split the image into its RGB channels
	height, width, layers = frame.shape
	zeroImgMatrix = np.zeros((height, width), dtype="uint8")

	# The OpenCV image sequence is Blue(B),Green(G) and Red(R)
#	(B, G, R) = cv2.split(frame)

	# we would like to construct a 3 channel Image with only 1 channel filled
	# and other two channels will be filled with zeros
#	B = cv2.merge([B, zeroImgMatrix, zeroImgMatrix])
#	G = cv2.merge([zeroImgMatrix, G, zeroImgMatrix])
#	R = cv2.merge([zeroImgMatrix, zeroImgMatrix, R])

	blue = frame.copy()
	blue[:,:,1] = 0
	blue[:,:,2] = 0

	red = frame.copy()
	red[:,:,0] = 0
	red[:,:,1] = 0

	frameHSV = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
	edges = cv2.Canny(frame,200,200)

	# show the frames
	cv2.imshow("Frame", frame)
	cv2.imshow("HSV", frameHSV)
	cv2.imshow("Edges", edges)
	cv2.imshow("Blue", blue)
	cv2.imshow("Red", red)
#	cv2.imshow("Green", G)

	key = cv2.waitKey(1) & 0xFF

	# if the `q` key was pressed, break from the loop
	if key == ord("q"):
		break

# do a bit of cleanup
print("[INFO] cleaning up...")
cv2.destroyAllWindows()
vs.stop()
writer.release()
