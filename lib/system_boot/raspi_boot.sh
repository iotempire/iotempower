#!/usr/bin/env bash

# check if otg-mode was enabled
egrep '^\s*dtoverlay=dwc2\s*' /boot/config.txt && otg_on=1

# read boot/config.txt and evaluate uiot-part in there
egrep '^\s*uiot_.*=' /boot/config.txt | while read l; do
    eval set "$l"
done

if [[ ! "$otg_on"=1 ]]; then
    # otg is not activated, so we need to try eventually to get our
    # internet via wifi on wlan1
    if [[ "$uiot_wifi_name" ]]; then # a name was given
        ifdown wlan1 # take network down
        # If wifi_user is set (and uncommented), try to connect to enterprise network
        # without extra certificate (for example to an eduroam university network)
        # else try to connect to wpa network
        cfg="/usr/local/etc/uiot_wpa_supplicant.conf.orig"
        cp "$cfg.orig" "$cfg"
        if [[ "$uiot_wifi_user" ]]; then # a user was given, so try enterprise network

cat << EOF >> "$cfg"

network={
ssid="$uiot_wifi_name"
scan_ssid=1
eap=TTLS
identity="$uiot_wifi_user"
password="$uiot_wifi_password"
password="password"
phase1="peaplabel=0"
phase2="auth=MSCHAPV2"
key_mgmt=WPA-PSK
}
EOF

        else # no user was given, try wpa

cat << EOF >> "$cfg"

network={
ssid="$uiot_wifi_name"
scan_ssid=1
key_mgmt=WPA-PSK
psk="$uiot_wifi_password"
}
EOF
        fi # connect to which network type check

        # restart interface with new configuration
        ifup wlan1
    fi # wifi connect requested check

fi # otg check

if [[ "$uiot_ap_password" ]]; then # pw was given, so start an accesspoint
    if [[ ! "$uiot_ap_name" ]]; then # no ap name, so make one up
        add_id=1
        uiot_ap_name=iotempire
    fi

    # TODO: start accesspoint

fi # accesspoint check


