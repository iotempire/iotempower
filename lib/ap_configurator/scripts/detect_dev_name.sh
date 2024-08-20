#!/bin/bash


# Attempt to detect name of output Wireless device
# Code borrowed from the "accesspoint" script in IoTempower, author: ulno


OUTPUT_WIFI=""
if [[ "$IOTEMPOWER_AP_DEVICES" ]]; then
    available_wifi=$(echo $(ip link show|cut -d: -f2|grep " wl"|cut -b2-))
    for possible in $(echo "$IOTEMPOWER_AP_DEVICES"); do
        if [[ "$available_wifi" =~ .*"$possible".* ]]; then
            OUTPUT_WIFI="$possible"
            break
        fi
    done
else
    # Take first available
    OUTPUT_WIFI=$(ip link show|cut -d: -f2|grep " wl"|cut -b2-|tail -n1)
fi

echo $OUTPUT_WIFI
