#!/usr/bin/python
#-*- coding:Utf-8 -*
#lm110518.2110

# Script to test serial

import serial

port = serial.Serial("/dev/ttyAMA0",
	baudrate=115200
#        parity=serial.PARITY_NONE,
#        stopbits=serial.STOPBITS_ONE,
#        bytesize=serial.EIGHTBITS,
#        timeout=1)
)

if port.isOpen():
	print("It was opened")
	port.close()

port.open()

while True:
	port.write(input().encode())
