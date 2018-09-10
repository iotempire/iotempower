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

mqtt_send lightboard00/animation \
100 1 2 3 4 5 6 5 8 7 6 5 4 3 4 5 6 7 8 7 6 5 4 3 4 5 6 7 8 8 8 8 7 6 5 4 4 4 5 6 7 8
