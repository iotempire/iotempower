#!/bin/bash

ANIMATION="\
s 1 black \
s 2 black \
f 1 red \
f 2 blue \
p 3000 \
f 1 black \
p 500 \
f 2 black \
p 1000 \
r"

ulnoiot mqtt_send owl01/eyes/animation "$ANIMATION"