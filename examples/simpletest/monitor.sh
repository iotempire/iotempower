#!/usr/bin/env bash
topic="testfloor/devkit1-01/ht1/$1"
mosquitto_sub  -h 192.168.10.254 -t "$topic" \
    -u homeassistant -P internetofthings | \
    ts "%s" | tee -a "$HOME/tmp/$1.log"
