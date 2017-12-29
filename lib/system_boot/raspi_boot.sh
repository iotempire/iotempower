#!/usr/bin/env bash
# Run a raspberry pi as ulnoiot gateway (wifi router and mqtt_broker)
#
# To enable this, add the following to the end of /etc/rc.local with adjusted ULNOIOT_ROOT:
# export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
# export ULNOIOT_ROOT="/home/pi/ulnoiot"
# "$ULNOIOT_ROOT/lib/system_boot/raspi_boot.sh"
#
# Also disable all network devices in /etc/network/interfaces apart lo and wlan1
# and make sure that wlan1 configuration looks like this:
# allow-hotplug wlan1
# iface wlan1 inet manual
#    wpa-conf /run/uiot_wpa_supplicant.conf


[ ! "$ULNOIOT_ROOT" ] || { echo "ULNOIOT_ROOT not set, aborting." 1>&2;exit 1; }

# needs to be still read as started from rc.local and it does not know bash's source
# this activates ulnoiot environment
source "$ULNOIOT_ROOT/lib/shell_starter/ulnoiot.bash"

# check if otg-mode was enabled
egrep '^\s*dtoverlay=dwc2\s*' /boot/config.txt && otg_on=1

# read boot/config.txt and evaluate uiot-part in there
eval "$(egrep '^\s*uiot_.*=' /boot/config.txt)"

if [[ ! "$otg_on" = 1 ]]; then
    # otg is not activated, so we need to try eventually to get our
    # internet via wifi on wlan1
    if [[ "$uiot_wifi_name" ]]; then # a name was given
        ifdown wlan1 # take network down
        # If wifi_user is set (and uncommented), try to connect to enterprise network
        # without extra certificate (for example to an eduroam university network)
        # else try to connect to wpa network
        cfg="/run/uiot_wpa_supplicant.conf"
        cp "$ULNOIOT_ROOT/etc/ulnoiot_wpa_supplicant.conf.base" "$cfg"
        chown root.root "$cfg"
        chmod 600 "$cfg"
        if [[ "$uiot_wifi_user" ]]; then # a user was given, so try enterprise network

cat << EOF >> "$cfg"

network={
ssid="$uiot_wifi_name"
scan_ssid=1
eap=TTLS
identity="$uiot_wifi_user"
password="$uiot_wifi_password"
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

    # overwrite some configuration variables
    #ULNOIOT_AP_DEVICES="wlan1 wlan0"

    # Name of accesspoint to create
    export ULNOIOT_AP_NAME="$uiot_ap_name"

    if [[ "$add_id" = 1 ]]; then
        export ULNOIOT_AP_ADDID=yes
    else
        export ULNOIOT_AP_ADDID=no
    fi

    ULNOIOT_AP_PASSWORD="$uiot_ap_password"

    # channel, bridge, and mqtt data taken straight from ulnoiot.conf

    # start accesspoint and mqtt_broker
    tmux new-session -n AP -s UIoTSvrs ulnoiot exec accesspoint \; new-window -n MQTT ulnoiot exec mqtt_broker

fi # accesspoint check
