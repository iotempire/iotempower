import paho.mqtt.client as mqtt
import threading
import random
import time
from tkinter import *
import datetime

##### Flags #####
TEMP_RISE = True

BROKER ="192.168.12.1"
TOPIC ='internetofthings/sim/'
SUBTOPIC ='temperature'

def show_values():
    input_temp = w2.get()
    while TRUE:
        while True:
            if TEMP_RISE == TRUE:
                input_temp = input_temp + random.uniform(0, 0.3)
            if input_temp < 200:
                break
            print("Temp out of range, type again")

        thread1 = threading.Thread(target=publish, args=(client, TOPIC + SUBTOPIC, round(input_temp, 1)))
        thread1.start()
        time.sleep(4)

def publish(client, topic, temperatur):
    client.publish(topic, temperatur)
    print("published data in " + TOPIC + SUBTOPIC + " " + str(temperatur) + "°")

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

def on_closing():
        master.destroy()

client = mqtt.Client()
client.on_message=on_message
client.connect(BROKER, 1883, 60)

master = Tk()
w2 = Scale(master, from_=-50, to=150, length=600, orient=HORIZONTAL)
w2.pack()
Button(master, text='Publish temperature', command=show_values).pack()
mainloop()