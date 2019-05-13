#!/usr/bin/env bash
# Run a raspberry pi as iotempower gateway (wifi router and mqtt_broker)
#
# To enable this,
# make sure iotempower-run script is porperly setup (for example in /home/pi/bin)
# add the following to the end of /etc/rc.local with adjusted location of the
# run-script:
# export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
# /home/pi/bin/iotempower exec /home/pi/iotempower/lib/system_boot/raspi-boot.sh"
#
# Also disable all network devices in /etc/network/interfaces apart lo and wlan1
# and make sure that wlan1 configuration looks like this (replace /home/pi/iotempower
# with the respective IOTEMPOWER_ROOT):
# allow-hotplug wlan1
# iface wlan1 inet manual
#    pre-up /home/pi/bin/iotempower exec /home/pi/iotempower/lib/system_boot/raspi-pre-up.sh
#    wpa-conf /run/uiot_wpa_supplicant.conf

[ "$IOTEMPOWER_ACTIVE" = "yes" ] || { echo "IoTempower not active, aborting." 1>&2;exit 1; }


source "$IOTEMPOWER_ROOT/bin/read_boot_config"

# Try to guess user
if [[ $IOTEMPOWER_ROOT =~ '/home/([!/]+)/iotempower' ]]; then
    IOTEMPOWER_USER=${BASH_REMATCH[1]}
else
    IOTEMPOWER_USER=iotempower
fi

if [[ "IOTEMPOWER_AP_PASSWORD" ]]; then # pw was given, so start an accesspoint
    # start accesspoint and mqtt_broker
    (
        sleep 15 # let network devices start
        cd "$IOTEMPOWER_ROOT"
        tmux new-session -d -n AP -s UIoTSvrs \
                "./run" exec accesspoint \; \
            new-window -d -n MQTT  \
                "./run" exec mqtt_broker \; \
            new-window -d -n nodered  \
                su - $IOTEMPOWER_USER -c 'iot exec nodered_starter' \; \
            new-window -d -n cloudcmd  \
                su - $IOTEMPOWER_USER -c 'iot exec cloudcmd_starter' \; \
            new-window -d -n dongle  \
                su - $IOTEMPOWER_USER -c 'iot exec dongle_starter' \;
    ) &
fi # accesspoint check
