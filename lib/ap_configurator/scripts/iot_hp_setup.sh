#!/bin/bash


# hostapd AP setup with steps.
# Based on accesspoint script in IoTempower, 
# adjusted for interactive use

nname=$1
npass=$2
baseip=$3
wdevice=$4


# Prepare for hostapd use after (presumably) NM
sudo sed -i '/^\[ifupdown\]/{N;s/managed=true/managed=false/;}' "/etc/NetworkManager/NetworkManager.conf"
# Remove a single '#' from line start if device name present 
sudo sed -i "/^\(#.*\)\($wdevice\)/ s/^#//" "/etc/network/interfaces"

sudo ifdown $wdevice && sudo ifup $wdevice
sudo service NetworkManager restart


export IOTEMPOWER_AP_NAME=$nname
export IOTEMPOWER_AP_PASSWORD=$npass
export IOTEMPOWER_AP_IP=$baseip
export IOTEMPOWER_AP_ADDID="no"

# Save to IoTempower config file as well,
# granted the file is protected well
cat << EOF > $IOTEMPOWER_ROOT/etc/wifi_credentials
SSID=$nname
Password=$npass
GatewayIP=$baseip
EOF


# Call the iotempower script from bin/,
# iot env must be activated!
echo $(accesspoint)
