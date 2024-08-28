#!/bin/bash


# Read previously defined AP credentials (if they exist)

source $IOTEMPOWER_ROOT/etc/wifi_credentials

echo "$SSID,$Password,$GatewayIP"
