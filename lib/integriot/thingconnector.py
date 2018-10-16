# This can create a list of thingis per mqtt connection
# TODO: allow other connections (COAP? UDP?)
#
# Author: Ulrich Norbisrath (http://ulno.net)
# Created: 2017-10-05


from integriot import thingi
import paho.mqtt.client as mqtt


class ThingConnector():
    def __init__(self, mqtt_host):
        # TODO: add username/password/tsl
        self.client = mqtt.Client()
        self.client.connect(mqtt_host)  # need to connect before subscribe
        # TODO: implement re-connect
        self.mqtt_host = mqtt_host
        self.thingis = {}
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

    def _add_thingi(self, d, full_topic):
        self.thingis[full_topic] = d

    def delete(self, topic):
        t = self._device_full_topic(topic)
        del self.thingis[t]

    def thingi(self, topic, subscriptions={}, qos=0):
        """
        Create new thing interface.

        :param topic:
        :param subscriptions: {"subtopic1":callback1[, "subtopic2":callback2]...}
            callbackn can be of the following forms:
            1. function -> call function if there is any new data for subtopicn
               function needs to take one param (message)
            2. (True,function) -> call function, if there is new different data
               for subtopicn
               function needs to take one param (message)
            3. {"data1":callbackn1[, "data2":callbackn2]...} -> call respective
               callback for specific data received
               callbacks take no params
            4. If callbacks is just one function (need to take topic and msg)
               install it as callback for all messages for the topic and all sub
               topics
        :return:
        """
        d = self.publisher(topic, qos)
        if callable(subscriptions):
            d.subscribe("", subscriptions)
        else:
            for t in subscriptions:
                item = subscriptions[t]
                if callable(item):
                    d.subscribe(t, item)
                elif type(item) is tuple:
                    b, cb = item
                    if b:
                        d.subscribe_change(t, cb)
                elif type(item) is dict:
                    for data in item:
                        d.subscribe_data(t, data, item[data])
        return d

    def publisher(self, topic, qos=0):
        """
        Create a thing interface to report values to or set values in (publish).
        A publisher usually does not need any callbacks/subscriptions.

        :param topic:
        :return:
        """
        t = self._device_full_topic(topic)
        d = thingi.Thingi(self.client, t, qos)
        self._add_thingi(d, t)
        return d

    def subscriber(self, topic, subscriptions={}, qos=0):
        """
        Create a thing interface for receiving values (subscribe).

        :param topic:
        :param subscriptions: {"subtopic1":callback1[, "subtopic2":callback2]...}
            callbackn can be of the following forms:
            1. function -> call function if there is any new data for subtopicn
               function needs to take one param (message)
            2. (True,function) -> call function, if there is new different data
               for subtopicn
               function needs to take one param (message)
            3. {"data1":callbackn1[, "data2":callbackn2]...} -> call respective
               callback for specific data received
               callbacks take no params
            4. If callbacks is just one function (need to take topic and msg)
               install it as callback for all messages for the topic and all sub
               topics
        :return:
        """
        return self.thingi(topic, subscriptions, qos=qos)

    def switch_publisher(self, topic, on_command="on", off_command="off",
                         set_topic="set", state_topic="", init_state=None, qos=0):
        """
        Create a thing interface to publish values as a switch (reporting on and
        off depending on status). This also supports a toggle method resulting
        in publishing the correct method based on an internal state.

        :param topic:
        :param on_command:
        :param off_command:
        :param set_topic:
        :param state_topic:
        :param init_state:
        :param qos:
        :return:
        """
        t = self._device_full_topic(topic)
        d = thingi.ThingiSwitch(self.client, t, on_command, off_command,
                              set_topic, state_topic, init_state, qos)
        self._add_thingi(d, t)
        return d
