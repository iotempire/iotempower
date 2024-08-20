#!/bin/bash


# External OpenWRT router setup.
# Based on wifi_setup_openwrt script in IoTempower,
# adjusted for interactive use

nname=$1
npass=$2
baseip=$3


# IoTempower handling the security aspect of the password entry!
cat << EOF > $IOTEMPOWER_ROOT/etc/wifi_credentials
SSID=$nname
Password=$npass
GatewayIP=$baseip
EOF


echo $(wifi_openwrt_uci)
echo $(setup_systemconf)
