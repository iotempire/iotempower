#!/usr/bin/env bash
# Run a raspberry pi as ulnoiot gateway (wifi router and mqtt_broker)
#
# To enable this,
# make sure ulnoiot-run script is porperly setup (for example in /home/pi/bin)
# add the following to the end of /etc/rc.local with adjusted location of the
# run-script:
# export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
# /home/pi/bin/ulnoiot exec /home/pi/ulnoiot/lib/system_boot/raspi-boot.sh"
#
# Also disable all network devices in /etc/network/interfaces apart lo and wlan1
# and make sure that wlan1 configuration looks like this (replace /home/pi/ulnoiot
# with the respective ULNOIOT_ROOT):
# allow-hotplug wlan1
# iface wlan1 inet manual
#    pre-up /home/pi/bin/ulnoiot exec /home/pi/ulnoiot/lib/system_boot/raspi-pre-up.sh
#    wpa-conf /run/uiot_wpa_supplicant.conf

[ "$ULNOIOT_ACTIVE" = "yes" ] || { echo "ulnoiot not active, aborting." 1>&2;exit 1; }


source "$ULNOIOT_ROOT/bin/read_boot_config"

# Try to guess user
if [[ $ULNOIOT_ROOT =~ '/home/([!/]+)/ulnoiot' ]]; then
    ULNOIOT_USER=${BASH_REMATCH[1]}
else
    ULNOIOT_USER=ulnoiot
fi

if [[ "ULNOIOT_AP_PASSWORD" ]]; then # pw was given, so start an accesspoint
    # start accesspoint and mqtt_broker
    (
        sleep 15 # let network devices start
        cd "$ULNOIOT_ROOT"
        tmux new-session -d -n AP -s UIoTSvrs \
                "./run" exec accesspoint \; \
            new-window -d -n MQTT  \
                "./run" exec mqtt_broker \; \
            new-window -d -n nodered  \
                su - $ULNOIOT_USER -c 'ulnoiot exec nodered_starter' \; \
            new-window -d -n cloudcmd  \
                su - $ULNOIOT_USER -c 'ulnoiot exec cloudcmd_starter' \; \
            new-window -d -n dongle  \
                su - $ULNOIOT_USER -c 'ulnoiot exec dongle_starter' \;
    ) &
fi # accesspoint check
