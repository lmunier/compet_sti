#!/usr/bin/python
# -*-coding:Utf-8 -*
#lm110518.1612

# Script to shutdown raspberry py when a button is hold

from gpiozero import Button
from signal import pause
import os

offGPIO = 21
holdTime = 5

def shutdown():
	os.system("sudo poweroff")

# Definition of button and shutdown
btn = Button(offGPIO, hold_time=holdTime, pull_up=True)
btn.when_held = shutdown
pause() # handle the button presses in the background
