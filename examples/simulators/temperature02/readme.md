This is a temperature simulator written in java.
The simulator using the paho mqtt client.
The simulator sending the temperature every 5 seconds via MQTT-messages to the mqtt server.
To connect to the MQTT server, you have to state the ip address of the mqqt server in the empty editfield and press connect.
After the connection the temperature simulator sends every 5 seconds the current temperature stated with the slider, between 10 and 30 degrees.