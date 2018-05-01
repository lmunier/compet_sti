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
ap.add_argument("-p", "--picamera", type=int, default=-1, help="whether or not the Raspberry Pi camera should be used")
args = vars(ap.parse_args())

# initialize the video stream and allow the camera
# sensor to warmup
print("[INFO] warming up camera...")

if args["picamera"] == 1:
	vs = VideoStream(usePiCamera=True).start()
elif args["picamera"] == 2:
	vs = cv2.VideoCapture(0)
elif args["picamera"] == 3:
	vs_raspi = VideoStream(usePiCamera=True).start()
	vs_webcam = cv2.VideoCapture(0)

time.sleep(2.0)

# loop over frames from the video stream
while True:
	# grab the frame from the video stream
	# with the picamera
	if args["picamera"] == 1:
		frame = vs.read()

		frameHSV = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
		edges = cv2.Canny(frame,200,200)

		# show the frames
		cv2.imshow("HSV", frameHSV)
		cv2.imshow("Edges", edges)
	elif args["picamera"] == 2:
		ret_val, frame = vs.read()
	elif args["picamera"] == 3:
		frame_raspi = vs_raspi.read()
		ret_val, frame_webcam = vs_webcam.read()

	if args["picamera"] == 3:
		cv2.imshow("Frame PI", frame_raspi)
		cv2.imshow("Frame webcam", frame_webcam)
	else:
		cv2.imshow("Frame", frame)

	key = cv2.waitKey(1) & 0xFF

	# if the `q` key was pressed, break from the loop
	if key == ord("q"):
		break

# do a bit of cleanup
print("[INFO] cleaning up...")
cv2.destroyAllWindows()
vs.stop()
