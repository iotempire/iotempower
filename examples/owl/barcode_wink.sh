#!/usr/bin/env bash

# find directory
# $1 destination (can be usb0-usb9 or acm0-acm9 or address
dir=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

cd "$dir"

ulnoiot exec mqtt_action cgi/mixowl01 each . bash "$dir/send_animation.sh"