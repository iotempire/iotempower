#!/bin/bash


# Launcher for the AP Configurator app

# Usage:
# bash run.sh, or
# ./run.sh


echo "Testing sudo rights for this user..."
sudo echo "OK"
sudo echo "Launching app..."
python3 ./ap_configurator.py
