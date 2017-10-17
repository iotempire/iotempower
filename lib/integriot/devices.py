# This can create a list of devices per mqtt connection
# TODO: allow other connections (COAP? UDP?)
#
# Author: Ulrich Norbisrath (http://ulno.net)
# Created: 2017-10-05


from integriot import device
import paho.mqtt.client as mqtt

class Devices():
    def __init__(self, mqtt_host):
        # TODO: add username/password/tsl
        self.client = mqtt.Client()
        self.client.connect(mqtt_host)  # need to connect before subscribe
        # TODO: implement re-connect
        self.mqtt_host = mqtt_host
        self.devices = {}
        self.topic_prefix = None  # can be set from outside

    def connect(self):
        return self.client.connect(self.mqtt_host)

    def _device_full_topic(self, topic):
        if self.topic_prefix:
            if self.topic_prefix.endswith("/"):
                t = self.topic_prefix + topic
            else:
                t = self.topic_prefix + "/" + topic
        else:
            t = topic
        return t

    def _add_device(self, d, full_topic):
        self.devices[full_topic] = d

    def delete(self,topic):
        t = self._device_full_topic(topic)
        del self.devices[t]

    def actor(self, topic, qos=0):
        t = self._device_full_topic(topic)
        d = device.MQTTDevice(self.client, t, qos)
        self._add_device(d, t)
        return d

    def sensor(self, topic, callbacks={}, qos=0):
        """
        Create new sensor device.

        :param topic:
        :param callbacks: {"subtopic1":callback1, ["subtopic2":callback2], ...}
            callbackn can be of the following forms:
            1. function -> call function if there is any new data for subtopicn
               function needs to take one param (message)
            2. (True,function) -> call function, if there is new different data
               for subtopicn
               function needs to take one param (message)
            3. {"data1":callbackn1,["data2":callbackn2]} -> call respective
               callback for specific data received
               callbacks take no params
            4. If callbacks is just one function (need to take topic and msg)
               install it as callback for all sub messages of current sensor
        :return:
        """
        d = self.actor(topic, qos)
        if callable(callbacks):
            d.add_callback("", callbacks)
        else:
            for t in callbacks:
                item = callbacks[t]
                if callable(item):
                    d.add_callback(t, item)
                elif type(item) is tuple:
                    b, cb = item
                    if b:
                        d.add_callback_change(t, cb)
                elif type(item) is dict:
                    for data in item:
                        d.add_callback_data(t, data, item[data])
        return d

    def switch(self, topic, on_command="on", off_command="off",
               set_topic="set", state_topic="", init_state=None, qos=0):
        t = self._device_full_topic(topic)
        d = device.MQTTSwitch(self.client, t, on_command, off_command,
                              set_topic, state_topic, init_state, qos)
        self._add_device(d, t)
        return d
    led = switch
    relay = switch
