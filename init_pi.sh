#!/bin/bash
#lm290418.1251

# A little script to update our raspberry pi and install some great features

sudo apt-get -y upgrade
sudo apt-get -y update

# Install tmux to split screen in commandline
sudo apt-get install -y tmux

# Clean installations
sudo apt-get -y autoclean
sudo apt-get -y autoremove
