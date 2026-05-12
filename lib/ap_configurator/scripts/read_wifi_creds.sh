#!/usr/bin/env bash


# Read previously defined AP credentials (if they exist)

source "$IOTEMPOWER_ROOT/bin/config_parser" || exit 1
iotempower_read_config wifi "$IOTEMPOWER_ROOT/etc/wifi_credentials" || exit 1

printf 'SSID=%s\n' "${SSID:-}"
printf 'Password=%s\n' "${Password:-}"
printf 'GatewayIP=%s\n' "${GatewayIP:-}"
