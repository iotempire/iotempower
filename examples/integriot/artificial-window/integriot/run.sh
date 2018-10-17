#!/usr/bin/env bash

[ "$ULNOIOT_ACTIVE" = "yes" ] || { echo "ulnoiot not active, aborting." 1>&2;exit 1; }

source "$ULNOIOT_VPYTHON/bin/activate"

# make sure to run in right environment and use correct python
sudo $(which python) run.py
