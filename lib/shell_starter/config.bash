#@IgnoreInspection BashAddShebang
# enable local bin and python environment
# set all values based on a given IOTEMPOWER_ROOT
#
# This should only be sourced by a bash like shell.

### check if configuration has been read
if [ "$IOTEMPOWER_ACTIVE" != "yes" ]; then
export IOTEMPOWER_ACTIVE=yes

# IOTEMPOWER_ROOT needs to be set
if [ ! "$IOTEMPOWER_ROOT" ]; then
    echo "IOTEMPOWER_ROOT not set. Environment might not work." 1>&2
fi

# derived variables
export IOTEMPOWER_LOCAL="$IOTEMPOWER_ROOT/.local"
export PATH="$IOTEMPOWER_LOCAL/bin:$PATH"
export IOTEMPOWER_EXTERNAL="$IOTEMPOWER_LOCAL/external"
export IOTEMPOWER_VPYTHON="$IOTEMPOWER_LOCAL/vp"
export IOTEMPOWER_VPYTHON2="$IOTEMPOWER_LOCAL/vp2"
#export IOTEMPOWER_COMPILE_CACHE="$IOTEMPOWER_LOCAL/compile_cache/node/build"
export IOTEMPOWER_COMPILE_CACHE="$IOTEMPOWER_LOCAL/compile_cache"
export IOTEMPOWER_ROOT

# set some defaults
IOTEMPOWER_AP_DEVICES=$(ip link|cut -d: -f2|grep " wl"|cut -b2-)
IOTEMPOWER_AP_NAME="iotempire"
IOTEMPOWER_AP_ADDID=no
IOTEMPOWER_AP_PASSWORD="internetofthings"
IOTEMPOWER_AP_CHANNEL=random
IOTEMPOWER_AP_IP="192.168.12.1"
IOTEMPOWER_AP_HIDDEN=no
IOTEMPOWER_AP_BRIDGE=""
IOTEMPOWER_AP_HOSTNAME="$HOSTNAME"

IOTEMPOWER_MQTT_HOST="$IOTEMPOWER_AP_IP"
IOTEMPOWER_MQTT_USER=""
IOTEMPOWER_MQTT_PW=""
IOTEMPOWER_MQTT_BRIDGE_HOST=""
IOTEMPOWER_MQTT_BRIDGE_PORT="8883"
IOTEMPOWER_MQTT_BRIDGE_CERTPATH="/etc/ssl/certs"
IOTEMPOWER_MQTT_BRIDGE_TOPIC="bridge"
IOTEMPOWER_MQTT_BRIDGE_USER="bridgeuser"
IOTEMPOWER_MQTT_BRIDGE_PW="secretpw"

# if this is empty, no discovery done
IOTEMPOWER_MQTT_DISCOVERY_PREFIX="iotempower"


IOTEMPOWER_WEBREPL_PW="iotempower"

# first read local boot configuration
source $IOTEMPOWER_ROOT/bin/read_boot_config


# second read config in etc, can overwrite boot config
if [ -e "$IOTEMPOWER_ROOT/etc/iotempower.conf" ]; then
    source "$IOTEMPOWER_ROOT/etc/iotempower.conf"
fi


IOTEMPOWER_VERSION=$(cat "$IOTEMPOWER_ROOT/VERSION")

# export all
export IOTEMPOWER_VERSION
export IOTEMPOWER_MQTT_HOST
export IOTEMPOWER_AP_DEVICES
export IOTEMPOWER_AP_NAME
export IOTEMPOWER_AP_ADDID
export IOTEMPOWER_AP_PASSWORD
export IOTEMPOWER_AP_CHANNEL
export IOTEMPOWER_AP_IP
export IOTEMPOWER_AP_BRIDGE
export IOTEMPOWER_AP_HIDDEN
export IOTEMPOWER_AP_HOSTNAME

export IOTEMPOWER_MQTT_USER
export IOTEMPOWER_MQTT_PW
export IOTEMPOWER_MQTT_BRIDGE_HOST
export IOTEMPOWER_MQTT_BRIDGE_PORT
export IOTEMPOWER_MQTT_BRIDGE_CERTPATH
export IOTEMPOWER_MQTT_BRIDGE_TOPIC
export IOTEMPOWER_MQTT_BRIDGE_USER
export IOTEMPOWER_MQTT_BRIDGE_PW
export IOTEMPOWER_MQTT_DISCOVERY_PREFIX

# activate the virtual python environment
source "$IOTEMPOWER_VPYTHON/bin/activate" &> /dev/null
true # ignore result of this

# add pathes for firmware compilation
export IOTEMPOWER_FIRMWARE="$IOTEMPOWER_EXTERNAL/firmware"
export PATH="$IOTEMPOWER_FIRMWARE/bin:$PATH"
export PATH="$IOTEMPOWER_FIRMWARE/esp-open-sdk/xtensa-lx106-elf/bin:$PATH"

IOTEMPOWER_AP_NAME_FULL=$(accesspoint show)
export IOTEMPOWER_AP_NAME_FULL

fi ### IOTEMPOWER_ACTIVE
