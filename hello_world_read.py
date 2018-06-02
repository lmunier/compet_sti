#!/usr/bin/python
#-*- coding:Utf-8 -*
#lm110518.2110

# Script to test serial

import serial

port = serial.Serial("/dev/ttyAMA0",
	baudrate=9600,
#        parity=serial.PARITY_NONE,
#        stopbits=serial.STOPBITS_ONE,
#        bytesize=serial.EIGHTBITS,
        timeout=1)

while True:
	rcv = port.readline()
	str = repr(rcv)

#	if str != "b''":
	print(str)
