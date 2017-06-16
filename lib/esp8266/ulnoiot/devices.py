# Device manager
# author: ulno
# created: 2017-04-07
#
# manage a list of installed devices and enable
# sending or reacting to mqtt

import gc
#import ussl - no decent ssl possible on micropython esp8266
#gc.collect
from umqtt.simple import MQTTClient as _MQTTClient
gc.collect()
import machine
import time
import ubinascii
gc.collect()
import ulnoiot._wifi as _wifi
gc.collect()

_devlist = {}
_timestack = {}

#### global MQTT variables
_client_id = ubinascii.hexlify(machine.unique_id())  # make unique
_broker = None
_topic = None
_user = None
_password = None
_client = None
_report_ip = True
_port = None
_last_publish = 0
#_ssl = False
MIN_PUBLISH_TIME_US = 2000 # posting every 2ms (2000us) allowed

##### Devices
def create_device(import_name,class_name,name,*args,**kwargs):
    global _devlist

    gc.collect()
    x="from ulnoiot.{} import {}".format(import_name,class_name)
    exec(x)
    _Class=eval(class_name)
    gc.collect()
    _devlist[name]=_Class(name,*args,**kwargs)
    gc.collect()
    return _devlist[name]

def contact(name,*args,**kwargs):
    return create_device("_contact","Contact",name,*args,**kwargs)
button = contact
input = contact

def analog(name,*args,**kwargs):
    return create_device("_analog","Analog",name,*args,**kwargs)

def trigger(name,*args,**kwargs):
    return create_device("_trigger","Trigger",name,*args,**kwargs)

def out(name,*args,**kwargs):
    return create_device("_output","Output",name,*args,**kwargs)
led = out
switch = out
output = out

def servo(name,*args,**kwargs):
    return create_device("_servo","Servo",name,*args,**kwargs)

def servo_switch(name,*args,**kwargs):
    return create_device("_servo_switch","Servo_Switch",name,*args,**kwargs)

def rgb(name,*args,**kwargs):
    return create_device("_rgb","RGB",name,*args,**kwargs)

def rgb_multi(name,*args,**kwargs):
    return create_device("_rgb_multi","RGB_Multi",name,*args,**kwargs)
neopixel=rgb_multi

####### HT temperature/humidity with
def dht11(name,*args,**kwargs):
    return create_device("_ht","DHT11",name,*args,**kwargs)

def dht22(name,*args,**kwargs):
    return create_device("_ht","DHT22",name,*args,**kwargs)

def ds18x20(name,*args,**kwargs):
    return create_device("_ht","DS18X20",name,*args,**kwargs)

####### display
def display(name,*args,**kwargs):
    return create_device("_display","Display",name,*args,**kwargs)

import ulnoiot.new_devices # allow adding new devices

######## Devices End

######## utils
def do_later(time_delta_s,callback,id=None):
    delta = int(time_delta_s*1000)
    later = time.ticks_add(time.ticks_ms(),delta)
    if id is None:
        id = hash((hash(callback),later))
    _timestack[id] = (later,callback)
    return id

def linecat(filename,frm=1,to=0):
    from ulnoiot import _line_edit
    _line_edit.linecat(filename,frm=frm,to=to)

def lineedit(filename,linenr,insert=False):
    from ulnoiot import _line_edit
    _line_edit.lineedit(filename,linenr,insert)

def delete(name):
    _devlist.pop(name)
######## end utils

####### mqtt stuff
def _publish_status(device_list=None,ignore_time=False):
    global _client,_last_publish

    if device_list is None:
        device_list = _devlist.values()

    current_time=time.ticks_us()
    if ignore_time or \
            time.ticks_diff(current_time,_last_publish)>=MIN_PUBLISH_TIME_US:
        _last_publish = current_time
        try:
            for d in device_list:
                v = d.value()
                if v is not None:
                    rt = (_topic + "/" + d.name).encode()
                    for s in d.getters:
                        if s=="":
                            t=rt
                        else:
                            t=rt+"/"+s.encode()
                        my_value = d.getters[s]()
                        print('Publishing', t, my_value)
                        _client.publish(t, str(my_value).encode())
        except Exception as e:
            print('Trouble publishing, re-init network.')
            print(e)
            _init_mqtt()


#### Setup and execution
def mqtt(broker_host, topic, *args, user=None, password=None,
         client_id=None, report_ip=True, 
         port=None):
# ssl not really possible    ,ssl=False):
    global _broker, _topic, _user, _password, _client_id, _report_ip
    global _port
#    global_ssl

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
    _report_ip = report_ip
    if port is None:
        port = 1883
#        if ssl == True:
#            port = 8883
#        else:
#            port = 1883
    _port = port
#    _ssl = ssl


def _subscription_cb(topic, msg):
    global _topic

    topic = topic.decode();
    msg = msg.decode();
    for d in _devlist.values():
        root_topic = _topic + "/" + d.name
        for st in d.setters:
            if st == "":
                t = root_topic
            else:
                t = root_topic + "/" + st
            if topic == t:
                print("Received \"%s\" for topic %s"%(msg, topic))
                d.run_setter(st,msg)


def _init_mqtt():
    global _client, _port
#    global _ssl
    global _broker, _topic, _user, _password, _client_id

    print("Trying to connect to mqtt broker.")
    _wifi.connect()
    gc.collect()
    try:
        _client = _MQTTClient(_client_id, _broker, user=_user,
                             password=_password,port=_port,ssl=False)
        _client.set_callback(_subscription_cb)
        _client.connect()
        print("Connected to",_broker)
        t = _topic.encode() + b"/#"
        _client.subscribe(t)
        print("Subscribed to topic and subtopics of", _topic)
        if _report_ip:
            t = (_topic + "/ip").encode()
            _client.publish(t, str(_wifi.config()[0]), retain=True)
    except Exception as e:
        print("Trouble to init mqtt:", e)


def _poll_subscripton():
    global _client
    try:
        _client.check_msg()  # non blocking
    except:
        print("Trouble to receive from mqtt.")


def run(updates=5, sleepms=1, poll_rate_inputs=4, poll_rate_network=10):
    # updates: send out a status every this amount of seconds.
    #          If 0, never send time based status updates only when change happened.
    # sleepms: going o sleep for how long between each loop run
    # poll_rate_network: how often to evaluate incoming network commands
    # poll_rate_inputs: how often to evaluate inputs
    _init_mqtt()
    if updates == 0:
        overflow = 1000
    else:
        overflow = updates * 1000
    overflow *= poll_rate_network * poll_rate_inputs / sleepms
    overflow = int(overflow)
#    print("Overflow:",overflow)
    counter = 0

    while True:
        if counter % poll_rate_network == 0:
            _poll_subscripton()
        if counter % poll_rate_inputs == 0:
            device_list = []
            for d in _devlist.values():
                if d.update():
                    device_list.append(d)
            if len(device_list)>0:
                _publish_status(device_list)
        if updates != 0 and counter % (updates * 1000) == 0:
            print("Publishing full update.")
            _publish_status(ignore_time=True)
        # execute things on timestack
        now = time.ticks_ms()
        for id in list(_timestack):
            t,cb = _timestack[id]
            if time.ticks_diff(now,t)>=0:
                del(_timestack[id])
                cb(id)

        time.sleep_ms(sleepms)
        counter += 1
        if counter >= overflow:
            counter = 0


transmit = run
