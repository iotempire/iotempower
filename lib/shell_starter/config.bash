#@IgnoreInspection BashAddShebang
# enable local bin and python environment
# set all values based on a given ULNOIOT_ROOT
#
# This should only be sourced by a bash like shell.

### check if configuration has been read
if [ "$ULNOIOT_ACTIVE" != "yes" ]; then
export ULNOIOT_ACTIVE=yes

# ULNOIOT_ROOT needs to be set
if [ ! "$ULNOIOT_ROOT" ]; then
    echo "ULNOIOT_ROOT not set. Environment might not work." 1>&2
fi

# derived variables
export ULNOIOT_LOCAL="$ULNOIOT_ROOT/.local"
export PATH="$ULNOIOT_LOCAL/bin:$PATH"
export ULNOIOT_EXTERNAL="$ULNOIOT_LOCAL/external"
export ULNOIOT_VPYTHON="$ULNOIOT_LOCAL/vp"
export ULNOIOT_VPYTHON2="$ULNOIOT_LOCAL/vp2"
export ULNOIOT_COMPILE_CACHE="$ULNOIOT_LOCAL/compile_cache/node/build"
export ULNOIOT_ROOT

# set some defaults
ULNOIOT_AP_DEVICES=$(ip link|cut -d: -f2|grep " wl"|cut -b2-)
ULNOIOT_AP_NAME="iotempire"
ULNOIOT_AP_ADDID=no
ULNOIOT_AP_PASSWORD="internetofthings"
ULNOIOT_AP_CHANNEL=random
ULNOIOT_AP_IP="192.168.12.1"
ULNOIOT_AP_HIDDEN=no
ULNOIOT_AP_BRIDGE=""
ULNOIOT_AP_HOSTNAME="$HOSTNAME"

ULNOIOT_MQTT_HOST="$ULNOIOT_AP_IP"
ULNOIOT_MQTT_USER=""
ULNOIOT_MQTT_PW=""
ULNOIOT_MQTT_BRIDGE_HOST=""
ULNOIOT_MQTT_BRIDGE_PORT="8883"
ULNOIOT_MQTT_BRIDGE_CERTPATH="/etc/ssl/certs"
ULNOIOT_MQTT_BRIDGE_TOPIC="bridge"
ULNOIOT_MQTT_BRIDGE_USER="bridgeuser"
ULNOIOT_MQTT_BRIDGE_PW="secretpw"

ULNOIOT_WEBREPL_PW="ulnoiot"

if [ -e "$ULNOIOT_ROOT/etc/ulnoiot.conf" ]; then
    source "$ULNOIOT_ROOT/etc/ulnoiot.conf"
fi

source $ULNOIOT_ROOT/bin/read_boot_config

ULNOIOT_VERSION=$(cat "$ULNOIOT_ROOT/VERSION")

# export all
export ULNOIOT_VERSION
export ULNOIOT_MQTT_HOST
export ULNOIOT_AP_DEVICES
export ULNOIOT_AP_NAME
export ULNOIOT_AP_ADDID
export ULNOIOT_AP_PASSWORD
export ULNOIOT_AP_CHANNEL
export ULNOIOT_AP_IP
export ULNOIOT_AP_BRIDGE
export ULNOIOT_AP_HIDDEN
export ULNOIOT_AP_HOSTNAME

export ULNOIOT_MQTT_USER
export ULNOIOT_MQTT_PW
export ULNOIOT_MQTT_BRIDGE_HOST
export ULNOIOT_MQTT_BRIDGE_PORT
export ULNOIOT_MQTT_BRIDGE_CERTPATH
export ULNOIOT_MQTT_BRIDGE_TOPIC
export ULNOIOT_MQTT_BRIDGE_USER
export ULNOIOT_MQTT_BRIDGE_PW

# activate the virtual python environment
source "$ULNOIOT_VPYTHON/bin/activate" &> /dev/null
true # ignore result of this

# add pathes for firmware compilation
export ULNOIOT_FIRMWARE="$ULNOIOT_EXTERNAL/firmware"
export PATH="$ULNOIOT_FIRMWARE/bin:$PATH"
export PATH="$ULNOIOT_FIRMWARE/esp-open-sdk/xtensa-lx106-elf/bin:$PATH"

ULNOIOT_AP_NAME_FULL=$(accesspoint show)
export ULNOIOT_AP_NAME_FULL

fi ### ULNOIOT_ACTIVE
