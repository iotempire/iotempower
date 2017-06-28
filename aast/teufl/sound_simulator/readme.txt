++++++++++++++++++++++++++++++++++++++++++++
Sound Simulator
++++++++++++++++++++++++++++++++++++++++++++
The sound simulator needs 5 parameters, the name of the script, the ip address of the
mqtt server (broker), the topic to publish and the min and max value range of
the sound in db. The value used for the topic will be added to "sound", for example if 
the value is "sensor1" the final topic is "sound/sensor1".

Usage: python <script.py> <ip> <topic> <min sound value> <max sound value>
Example: python test.py 192.168.12.1 sensor1 0 12
