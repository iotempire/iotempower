import paho.mqtt.client as mqtt
import thread
import random
import time
#from time import sleep


def publish(client, topic, clock):
	client.publish(topic, clock)
	
broker ="192.168.12.1"
topic='internetofthings/'
subtopic='virtualClock'


client = mqtt.Client()
client.connect(broker, 1883, 60)

day_length=int(input("How long should the virtual day be in seconds "))
updateIntervel=int(input("How frequest the mqtt message should be sent(In seconds for 24 hr window). For better results enter value >60 " ))
#day_length=300
#updateIntervel=600
currentTimems=0
virtualSecond=day_length/86400.0
hours=0
mins=0
sec=0
print("Sending mqtt messages. press ctrl + C to stop.")
while True:
	thread.start_new_thread(publish, (client, topic+subtopic, str(hours)+":"+str(mins)+":"+str(sec), ) )
	time.sleep(virtualSecond*updateIntervel)
	sec = sec + updateIntervel
	if sec>=60:
		mins+=sec/60
		sec%=60
	if mins>=60:
		hours+=mins/60
		mins%=60
	if hours>=24:
		hours%=24