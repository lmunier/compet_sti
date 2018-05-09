#!/bin/bash
#lm290418.1327

# This file will optimize our opencv installation

# Purge to obtain ~1Gb of space
sudo apt-get -y purge wolfram-engine
sudo apt-get -y purge libreoffice*
sudo apt-get -y clean
sudo apt-get -y autoremove

# Install independencies
sudo apt-get -y update && sudo apt-get -y upgrade
sudo apt-get -y install build-essential cmake pkg-config
sudo apt-get -y install libjpeg-dev libtiff5-dev libjasper-dev libpng12-dev
sudo apt-get -y install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev
sudo apt-get -y install libxvidcore-dev libx264-dev
sudo apt-get -y install libgtk2.0-dev libgtk-3-dev
sudo apt-get -y install libcanberra-gtk*
sudo apt-get -y install libatlas-base-dev gfortran
sudo apt-get -y install python2.7-dev python3-dev

# Download opencv source code
cd ~
wget -O opencv.zip https://github.com/Itseez/opencv/archive/3.4.1.zip
unzip opencv.zip
wget -O opencv_contrib.zip https://github.com/Itseez/opencv_contrib/archive/3.4.1.zip
unzip opencv_contrib.zip

# Create a virtual environnement for python
wget https://bootstrap.pypa.io/get-pip.py
sudo python get-pip.py
sudo python3 get-pip.py
sudo pip install virtualenv virtualenvwrapper
sudo rm -rf ~/.cache/pip

# Add some line in ~/.profile: virtualenv and virtualenvwrapper
echo "" >> ~/.profile
echo "# virtualenv and virtualenvwrapper" >> ~/.profile
echo "export WORKON_HOME=$HOME/.virtualenvs" >> ~/.profile
echo "source /usr/local/bin/virtualenvwrapper.sh" >> ~/.profile

# Change python path to python3.5
sudo rm /usr/bin/python
sudo ln -s /usr/bin/python3.5 /usr/bin/python

# Apply change
source ~/.profile
