# Device manager
# author: ulno
# created: 2017-04-07
#
# manage a list of installed devices and enable
# sending or reacting to mqtt

import gc
import machine
from machine import Pin
import ulnoiot._wifi as _wifi
import time
import ubinascii
from umqtt.simple import MQTTClient

_devlist = {}

#### global MQTT variables
_client_id = ubinascii.hexlify(machine.unique_id())  # make unique
_broker = None
_topic = None
_user = None
_password = None
client = None


class Device(object):
    def __init__(self, name, pin, value_map=None,
                 settable=False, ignore_command_case=True):
        global _topic
        self.topic = _topic + "/" + name
        self.name = name
        self.pin = pin
        self.command_topic = None
        self.ignore_case = ignore_command_case
        if settable:
            self.command_topic = self.topic + "/set"
            self.commands = {}
        self.value_map = value_map

    def is_settable(self):
        return self.command_topic is not None

    def add_command(self, command_name, callback):
        if self.ignore_case: command_name = command_name.lower()
        self.commands[command_name] = callback

    def delete_command(self, command_name):
        if self.ignore_case: command_name = command_name.lower()
        self.commands.pop(command_name)

    def command(self, command_str):
        if self.ignore_case:
            command_str = command_str.lower()
        command_run = self.commands.get(command_str)
        if command_run is not None:
            command_run()
        else:
            print("Device %s cannot run command %s." % (self.name, command_str))

    def value(self):
        return None

    def mapped_value(self):
        v = self.value()
        if v == None:
            return None
        else:
            return self.value_map[v]

    def _update(self):
        pass  # usually does nothing, can be overrriden to actualy update values, called by update

    def update(self):
        # returns True if the update caused a change in value
        oldval = self.value()
        self._update()
        return oldval != self.value()


class Contact(Device):
    # Handle contact or button like devices
    def __init__(self, name, pin, *args,
                 report_high="on", report_low="off",
                 pullup=True, threshold=0):
        if len(args) > 0:
            report_high = args[0]
            if len(args) > 1:
                report_low = args[1]
        Device.__init__(self, name, pin,
                        value_map={True: report_high.encode(), False: report_low.encode()})
        pin.init(Pin.IN);
        if pullup:
            pin.init(Pin.PULL_UP)
        else:
            pin.init(Pin.OPEN_DRAIN)
        self.debouncer = 0
        self.threshold = threshold + 1

    def value(self):
        return self.debouncer >= self.threshold

    def _update(self):
        # Needs to be read in a polling scenario on a regular basis (very frequent)
        if self.pin() == 1:
            self.debouncer += 1
            if self.debouncer > self.threshold * 2:
                self.debouncer = self.threshold * 2
        else:
            self.debouncer -= 1
            if self.debouncer < 0:
                self.debouncer = 0


Button = Contact


def contact(name, pin, *args, report_high="on", report_low="off",
            pullup=True,threshold=0):
    if len(args) > 0:
        report_high = args[0]
        if len(args) > 1:
            report_low = args[1]
    _devlist[name] = Contact(name, pin,
                            report_high=report_high,
                            report_low=report_low,
                            pullup=pullup,
                            threshold=threshold)
button = contact


class Output(Device):
    # Handle output devices
    def __init__(self, name, pin, *args, high_command='on', low_command='off', ignore_case=True):
        if len(args) > 0:
            high_command = args[0]
            if len(args) > 1:
                low_command = args[1]
        Device.__init__(self, name, pin,
                        settable=True, ignore_command_case=ignore_case,
                        value_map={1: high_command.encode(), 0: low_command.encode()})
        pin.init(Pin.OUT)
        self.add_command(high_command, self.high)
        self.add_command(low_command, self.low)

    def high(self):
        self.pin.high()

    def low(self):
        self.pin.low()

    on = high
    off = low

    def value(self):
        return self.pin()


def out(name, pin, *args, high_command='on', low_command='off', ignore_case=True):
    if len(args) > 0:
        high_command = args[0]
        if len(args) > 1:
            low_command = args[1]
    _devlist[name] = Output(name, pin, high_command=high_command, low_command=low_command, ignore_case=ignore_case)
led = out


class HT(Device):
    # Handle humidity and temperature from DS11
    def __init__(self, name, pin, *args,
                 report_high="on", report_low="off",
                 pullup=True, threshold=0):
        if len(args) > 0:
            report_high = args[0]
            if len(args) > 1:
                report_low = args[1]
        Device.__init__(self, name, pin,
                        value_map={True: report_high.encode(), False: report_low.encode()})
        pin.init(Pin.IN);
        if pullup:
            pin.init(Pin.PULL_UP)
        else:
            pin.init(Pin.OPEN_DRAIN)
        self.debouncer = 0
        self.threshold = threshold + 1

    def value(self):
        return self.debouncer >= self.threshold

    def _update(self):
        # Needs to be read in a polling scenario on a regular basis (very frequent)
        if self.pin() == 1:
            self.debouncer += 1
            if self.debouncer > self.threshold * 2:
                self.debouncer = self.threshold * 2
        else:
            self.debouncer -= 1
            if self.debouncer < 0:
                self.debouncer = 0




def delete(name):
    _devlist.pop(name)


def publish_status():
    global client

    try:
        for d in _devlist.values():
            v = d.mapped_value()
            print('Publish status', d.topic, v)
            client.publish(d.topic, d.mapped_value())
    except:
        print('Trouble publishing, re-init network.')
        init_client()


#### Setup and execution
def mqtt(broker_host, topic, *args, user=None, password=None, client_id=None):
    global _broker, _topic, _user, _password, _client_id

    if len(args) > 0:
        user = args[0]
        if len(args) > 1:
            password = args[1]

    _broker = broker_host
    _topic = topic
    if client_id is not None:
        _client_id = client_id.encode() + b"_" + _client_id;
    _user = user
    _password = password


def callback(topic, msg):
    topic = topic.decode();
    msg = msg.decode();
    for d in _devlist.values():
        if topic == d.command_topic:
            print("Received \"%s\" for topic %s"%(msg, topic))
            d.command(msg)

# display
#    elif config.display and topic == text_command_topic:
#      print("Received text in callback:", msg)
#      try:
#        msg = msg.decode()
#        if msg == "&&clear":
#          dp.clear()
#        else:
#          dp.println( msg )
#      except:
#        pass


def init_client():
    global client
    global _broker, _topic, _user, _password, _client_id

    print("Trying to connect to mqtt broker.")
    _wifi.connect()
    try:
        client = MQTTClient(_client_id, _broker, user=_user,
                            password=_password)
        client.set_callback(callback)
        client.connect()
        print("Connected to",_broker)
        t = _topic.encode() + b"/#"
        client.subscribe(t)
        print("Subscribed to topic and subtopics of", _topic)
    except Exception as e:
        print("Trouble to init mqtt:", e)


def receive_sub():
    global client
    try:
        client.check_msg()  # non blocking
    except:
        print("Trouble to receive from mqtt.")


def run(updates=5, sleepms=1, poll_rate_inputs=4, poll_rate_network=10):
    # updates: send out a status every this amount of seconds.
    #          If 0, never send time based status updates only when change happened.
    # sleepms: going o sleep for how long between each loop run
    # poll_rate_network: how often to evaluate incoming network commands
    # poll_rate_inputs: how often to evaluate inputs
    init_client()
    if updates == 0:
        overflow = 1000
    else:
        overflow = updates * 1000
    overflow *= poll_rate_network * poll_rate_inputs / sleepms
    overflow = int(overflow)
    print("Overflow:",overflow)
    counter = 0

    while True:
        if counter % poll_rate_network == 0:
            receive_sub()
        if counter % poll_rate_inputs == 0:
            change_happened = False
            for d in _devlist.values():
                if d.update():
                    change_happened = True
            if change_happened:
                publish_status()
        if updates != 0 and counter % (updates * 1000) == 0:
            print("Publishing.")
            publish_status()

        time.sleep_ms(sleepms)
        counter += 1
        if counter >= overflow:
            counter = 0


transmit = run
