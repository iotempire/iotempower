#!/usr/bin/env bash


# External OpenWRT router setup.
# Based on wifi_setup_openwrt script in IoTempower,
# adjusted for interactive use

nname=$1
npass=$2
baseip=$3

source "$IOTEMPOWER_ROOT/bin/config_parser" || exit 1

# IoTempower handling the security aspect of the password entry!
mkdir -p "$IOTEMPOWER_ROOT/etc"
{
    printf 'SSID=%s\n' "$(iotempower_quote_config_value "$nname")"
    printf 'Password=%s\n' "$(iotempower_quote_config_value "$npass")"
    printf 'GatewayIP=%s\n' "$(iotempower_quote_config_value "$baseip")"
} > "$IOTEMPOWER_ROOT/etc/wifi_credentials"


wifi_openwrt_uci || exit 1
setup_systemconf || exit 1
