#!/bin/bash
MY_PATH=`dirname "$0"`
cd "$MY_PATH"
MY_PATH=`pwd`
source .venv/bin/activate
exec python pulse_mqtt_volume.py
