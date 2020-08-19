# base Thing Interface (Thingi) classes
#
# Author: Ulrich Norbisrath (http://ulno.net)
# Created: 2017-10-05

import paho.mqtt.client as mqtt

class _Thingi():
    def __init__(self):
        self.status = None


class Thingi(_Thingi):

    def __init__(self, mqtt_client, main_topic, qos=0):
        self.mqtt_client = mqtt_client
        self.main_topic = main_topic
        self.callbacks = {}  # callbacks for receiving
        if main_topic == "":
            t = "#"
        else:
            t = main_topic + "/#"
        self.mqtt_client.subscribe(t, qos)
        self.mqtt_client.message_callback_add(t, self._callback)
        self.last_received = {}

    def __del__(self):
        if self.main_topic == "":
            t = "#"
        else:
            t = self.main_topic + "/#"
        self.mqtt_client.message_callback_remove(t)

    def _add_callback_entry(self, sub_topic, callback, ignore_case=True):
        if sub_topic not in self.callbacks:
            self.callbacks[sub_topic] = []
        self.callbacks[sub_topic].append((callback, ignore_case))

    def subscribe(self, sub_topic="", callback=None, ignore_case=True):
        """
        Add a generic callback function for incoming data on a specific
        sub_topic.
        This is called any time data arrives on sub_topic.
        :param sub_topic: the subtopic to react on
        :param callback: a callback function of form func(message)
        :param ignore_case: if True, cast all received payload into lowercase
        :return:
        """
        self._add_callback_entry(sub_topic, lambda t, m: callback(m), ignore_case)

    def subscribe_change(self, sub_topic="", callback=None, ignore_case=True):
        """
        Add a callback function for incoming data on a specific
        sub_topic.
        This is called any time data arrives on sub_topic which is different to
        the data received last time.
        :param sub_topic: the subtopic to react on
        :param callback: a callback function of form func(message)
        :param ignore_case: if True, cast all received payload into lowercase
        :return:
        """

        def new_cb(t, m):
            if t not in self.last_received or self.last_received[t] != m:
                callback(m)

        self._add_callback_entry(sub_topic, new_cb, ignore_case)

    def subscribe_data(self, sub_topic, data, callback, ignore_case=True):
        """
        Add a callback function for specific incoming data on a sub_topic.
        This is called any time the specified data arrives on sub_topic.
        :param sub_topic: the subtopic to react on
        :param callback: a calback function of form func()
        :param ignore_case: if True, cast all received payload into lowercase
        :return:
        """

        def new_cb(t, m):
            if m == data:
                callback()

        self._add_callback_entry(sub_topic, new_cb, ignore_case)

    def unsubscribe(self, sub_topic):
        """
        Remove all callbacks associated to one sub_topic
        :param sub_topic:
        :return:
        """
        if sub_topic in self.callbacks:
            del self.callbacks[sub_topic]

    def _callback(self, client, userdata, message):
        if message is None:
            return
        t = message.topic
        if not t.startswith(self.main_topic):
            return
        sub_topic = t[len(self.main_topic) + 1:]
        if not sub_topic in self.callbacks:
            return
        p = message.payload.decode()  # TODO: might not work -> check

        for callback, ignore_case in self.callbacks[sub_topic]:
            lp = p
            if ignore_case:
                lp = lp.lower()
            callback(sub_topic, lp)  # call before resetting last_received
        self.last_received[sub_topic] = p  # remember last received

    def publish(self, sub_topic=None, message=None, qos=0):
        if message is None:
            message = sub_topic
            sub_topic = ""
        if message is None: # nothing to publish
            return
        if type(message) is not str:
            message = str(message)
        if sub_topic:
            t = self.main_topic + "/" + sub_topic
        else:
            t = self.main_topic
        ret = self.mqtt_client.publish(t, message, qos)
        if ret.rc != mqtt.MQTT_ERR_SUCCESS:
            print("Lost connection.") # TODO: would be nice if paho mqtt coudl reconnect itself
            raise ConnectionAbortedError("Lost connection in publish.") 
            return False
        return True


class ThingiSwitch(Thingi):
    def __init__(self, mqtt_client, main_topic,
                 on_command="on", off_command="off",
                 set_topic="set",
                 state_topic="",
                 init_state=None, qos=0):
        Thingi.__init__(self, mqtt_client, main_topic, qos)
        self.on_command = on_command
        self.off_command = off_command
        self.set_topic = set_topic
        self.state_topic = state_topic
        if init_state:
            self.publish(set_topic, init_state)
            self.state = init_state
        else:
            self.state = off_command
        if state_topic:
            full_set_topic = main_topic + "/" + set_topic
        else:
            full_set_topic = main_topic
        self.subscribe(full_set_topic, self.update_state_cb)

    def update_state_cb(self, topic, msg):
        self.update_state(msg)

    def update_state(self, new_state):
        if new_state == self.on_command or new_state == self.off_command:
            self.state = new_state

    def on(self):
        self.publish(self.set_topic, self.on_command)
        self.update_state(self.on_command)

    high = on

    def off(self):
        self.publish(self.set_topic, self.off_command)
        self.update_state(self.off_command)

    low = off

    def toggle(self):
        if self.state == self.on_command:
            self.off()
        else:
            self.on()
