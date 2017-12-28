#!/bin/bash
SERVER=192.168.12.1
mqtt_send lightboard00/imagelist \
http://$SERVER:9080/1.png \
http://$SERVER:9080/2.png \
http://$SERVER:9080/3.png \
http://$SERVER:9080/4.png \
http://$SERVER:9080/5.png \
http://$SERVER:9080/6.png \
http://$SERVER:9080/7.png \
http://$SERVER:9080/8.png
