#!/bin/env bash
MY_PATH=`dirname "$0"`
cd "$MY_PATH"
MY_PATH=`pwd`
source .venv/bin/activate
while true; do
  python pulse_mqtt_volume.py
  sleep 2
done
