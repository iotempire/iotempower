#!/usr/bin/env bash
# Run a raspberry pi as iotempower gateway (wifi router and mqtt_broker)
#
# To enable this,
# make sure iotempower-run script is porperly setup (for example in /home/iot/bin)
# add the following to the end of /etc/rc.local with adjusted location of the
# run-script:
# export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
# /home/iot/bin/iot exec /home/iot/iot/lib/system_boot/raspi-boot.sh
#
# Also disable all network devices in /etc/network/interfaces apart lo and wlan1
# and make sure that wlan1 configuration looks like this (replace /home/pi/iotempower
# with the respective IOTEMPOWER_ROOT):
# allow-hotplug wlan1
# iface wlan1 inet manual
#    pre-up /home/iot/bin/iot exec /home/iot/iot/lib/system_boot/raspi-pre-up.sh
#    wpa-conf /run/uiot_wpa_supplicant.conf

[ "$IOTEMPOWER_ACTIVE" = "yes" ] || { echo "IoTempower not active, aborting." 1>&2;exit 1; }


source "$IOTEMPOWER_ROOT/bin/read_boot_config"

# Try to guess user
if [[ $IOTEMPOWER_ROOT =~ /home/'([!/]+)'/iot ]]; then
    IOTEMPOWER_USER=${BASH_REMATCH[1]}
else
    IOTEMPOWER_USER=iot
fi

_WIFI_CONFIG="/boot/wifi.txt"
# true here means to start the command true, which does NOT start the acesspoint
_ACESSPOINT_START="true"
_MQTT_SCANIF="scanif"
if [[ -e "$_WIFI_CONFIG" ]]; then
    # actually start accesspoint
    _ACESSPOINT_START="accesspoint"
    _MQTT_SCANIF=""
fi

if [[ "IOTEMPOWER_AP_PASSWORD" ]]; then # pw was given, so start an accesspoint
    # start accesspoint and mqtt_broker
    (
        sleep 15 # let network devices start
        cd "$IOTEMPOWER_ROOT"
        tmux new-session -d -n MQTT -s IoTsvcs \
                su - $IOTEMPOWER_USER -c "iot exec mqtt_starter $_MQTT_SCANIF" \; \
                new-window -d -n AP ./run exec $_ACESSPOINT_START  \; \
            new-window -d -n nodered  \
                su - $IOTEMPOWER_USER -c 'iot exec nodered_starter' \; \
            new-window -d -n cloudcmd  \
                su - $IOTEMPOWER_USER -c 'iot exec cloudcmd_starter' \; \
            new-window -d -n dongle  \
                su - $IOTEMPOWER_USER -c 'iot exec dongle_starter' \;
    ) &
fi # accesspoint check
