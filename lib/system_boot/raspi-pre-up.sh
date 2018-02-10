#!/usr/bin/env bash
# Run a raspberry pi as ulnoiot gateway (wifi router and mqtt_broker)
#
# network pre-up script -> more documentation in raspi-boot.sh

[ "$ULNOIOT_ACTIVE" = "yes" ] || { echo "ulnoiot not active, aborting." 1>&2;exit 1; }

source "$ULNOIOT_ROOT/bin/read_boot_config"

if [[ ! "$otg_on" = 1 ]]; then
    # otg is not activated, so we need to try eventually to get our
    # internet via wifi on wlan1
    if [[ "$uiot_wifi_name" ]]; then # a name was given
        # If wifi_user is set (and uncommented), try to connect to enterprise network
        # without extra certificate (for example to an eduroam university network)
        # else try to connect to wpa network
        rm /run/wpa_supplicant/wlan1 &> /dev/null # makes problems sometimes
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
key_mgmt=WPA-EAP
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
    fi # wifi connect requested check

sleep 0.5 # somehow things seem to sometimes not work

fi # otg check

