#!/bin/bash

ANIMATION="\
s 1 black \
s 2 black \
f 1 red \
f 2 blue \
p 3000 \
f 1 yellow \
f 2 purple \
p 2000 \
f 1 black \
p 2000 \
f 2 black \
p 3000 \
"

ulnoiot exec mqtt_send owl01/eyes/animation "$ANIMATION"