#!/bin/sh

BROKER_ADDRESS="192.168.12.1"
BROKER_PORT=1883
TEMP_FILE="/tmp/mqtt_clients"

# Clear the temporary file
> $TEMP_FILE

# Function to update client timestamp
update_client() {
    local client_id="$1"
    local timestamp=$(date +%s)
    # Append or update the client timestamp
    grep -v "^$client_id " $TEMP_FILE > $TEMP_FILE.tmp
    echo "$client_id $timestamp" >> $TEMP_FILE.tmp
    mv $TEMP_FILE.tmp $TEMP_FILE
}

# Function to count active clients
count_active_clients() {
    local current_time=$(date +%s)
    local timeout=30  # 30 seconds timeout
    local active_count=0

    while read line; do
        set -- $line
        local last_seen=$2
        if [ $(($current_time - $last_seen)) -le $timeout ]; then
            active_count=$((active_count + 1))
        fi
    done < $TEMP_FILE

    echo "Active clients: $active_count"
}

# Subscribe to all topics and process messages
mosquitto_sub -h $BROKER_ADDRESS -p $BROKER_PORT -t "#" | while read line; do
    local client_id=$(echo "$line" | cut -d '/' -f1)  # Assuming client_id is the first part of the topic
    update_client "$client_id"
    count_active_clients
done
