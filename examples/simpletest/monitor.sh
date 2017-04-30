#!/usr/bin/env bash
topic="test1"
#mosquitto_sub  -h 192.168.12.1 -t "$topic" | \
#    ts "%s" | tee -a "$HOME/tmp/$1.log"
mosquitto_sub  -h 192.168.12.1 -t "$topic/ht1/temperature" | \
    ts "%s" > "$HOME/tmp/temp1.log" &
id1=$!
mosquitto_sub  -h 192.168.12.1 -t "$topic/t2" | \
    ts "%s" | tee -a "$HOME/tmp/temp2.log" &
id2=$!
echo "Press return or ctrl-c to finish."
read tmp

kill $id1 $id2
