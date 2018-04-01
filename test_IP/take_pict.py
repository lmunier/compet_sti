import picamera

with picamera.PiCamera() as camera:
    camera.resolution = (1024, 768)
    camera.capture("test_resolution.jpg")
