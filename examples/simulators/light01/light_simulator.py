import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
	print "Connected to mqtt server with the ip 192.168.12.1"
	client.subscribe("light/switch")

def on_message(client, userdata, msq):
	print "Light is now switched " + str(msq.payload)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("192.168.12.1")

client.loop_forever()