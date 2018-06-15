#!/bin/bash
# 
pkill -9 -f log_color.py
cd /home/pi/gith/skycolor/sense/
sudo python log_color.py > /var/log/log_color.log 2>&1 &

# startup anything else you want here
