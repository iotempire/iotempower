#!/bin/env bash

[ "$IOTEMPOWER_ACTIVE" = "yes" ] || { echo "IoTempower not active, aborting." 1>&2;exit 1; }

source "$IOTEMPOWER_VPYTHON/bin/activate"

# make sure to run in right environment and use correct python
sudo $(which python) run.py
