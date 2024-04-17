#!/bin/env bash
topic="test1"
file1="$HOME/tmp/temp1-new.log"
file2="$HOME/tmp/temp2-new.log"
#mosquitto_sub  -h 192.168.12.1 -t "$topic" | \
#    ts "%.s" | tee -a "$HOME/tmp/$1.log"
#mosquitto_sub  -h 192.168.12.1 -t "$topic/ht1/temperature" | \
#    ts "%.s" > "$file1" &
#id1=$!
mosquitto_sub  -h 192.168.12.1 -t "$topic/t2" | \
    ts "%.s" > "$file2" &
id2=$!
echo "Monitoring to $file2."
echo "Press return or ctrl-c to finish."
read tmp

kill $id1 $id2
