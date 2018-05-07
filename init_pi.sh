#!/bin/bash
#lm290418.1251

# A little script to update our raspberry pi and install some great features

sudo apt-get -y upgrade
sudo apt-get -y update

# Install tmux to split screen in commandline
sudo apt-get install -y tmux

# Install library to convert video files
sudo apt-get install -y ffmpeg

# Install libraries to use webcam
sudo apt-get install -y fswebcam

# Install libraries to use raspicam
pip install picamera[array]

# Install some features to work on opencv
pip install imutils

# Clean installations
sudo apt-get -y autoclean
sudo apt-get -y autoremove
