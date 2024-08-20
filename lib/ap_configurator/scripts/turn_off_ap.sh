#!/bin/bash


# Turn off any running APs created by the app

ssid=$1

nmcli connection delete $ssid
sudo service NetworkManager restart

sudo pkill create_ap
sudo pkill hostapd

ps -ax | grep "$ssid" | grep -v grep | awk '{print $1}' | xargs -r sudo kill -9
