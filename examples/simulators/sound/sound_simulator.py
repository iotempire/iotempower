import sys
import time
import paho.mqtt.client as mqtt
from random import randint

def on_connect(client, userdata, flags, rc):
	print "Connected to mqtt server with the ip " + ip

def on_publish(client,userdata,result):
	print "Published on topic " + topic 
	
def publish_sound():
	db = randint(min, max)
	client.publish(topic, db)

def convertable(input):
	try:
		int(input)
		return True
	except ValueError:
		return False
	
def usage():
	print >>sys.stderr, "Usage: python <script.py> <ip> <topic> <min sound value> <max sound value>\nExample: python test.py 192.168.12.1 sensor1 0 12"
	sys.exit()
	
if len(sys.argv) < 5:
	usage()

if not convertable(sys.argv[3]):
	usage()
	
if not convertable(sys.argv[4]):
	usage()

ip = str(sys.argv[1])
topic = "sound/"
topic = topic + sys.argv[2]
min = int(sys.argv[3])
max = int(sys.argv[4])

client = mqtt.Client()
client.on_connect = on_connect
client.on_publish = on_publish

client.connect(ip)

while 1:
	publish_sound()
	time.sleep(2)