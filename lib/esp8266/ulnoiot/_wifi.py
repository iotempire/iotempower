# Wifi Configuration
#
import machine
import network
import time
import webrepl

_ap = network.WLAN(network.AP_IF)
_ap.active(False)
_wlan = network.WLAN(network.STA_IF)
_wlan.active(True)
time.sleep(1) # wait to become active

# connect to wifi
def connect():
    global wifi_config,  _wlan

    _ap.active(False)
    _wlan.active(True)

    # removed scan of networks to allow conenct to hidden
    # Try to connect
    _wlan.connect(wifi_config.name, wifi_config.password)
    tries=15
    for i in range(tries):
        print("%d/%d. Trying to connect." %(i+1, tries))
        machine.idle()
        time.sleep(1)
#                if _wlan.isconnected(): break
        if _wlan.status() == network.STAT_GOT_IP: break

    if _wlan.isconnected() and _wlan.status() == network.STAT_GOT_IP:
        print('Wifi: connection succeeded!')
        print(_wlan.ifconfig())
    else:
        print('Wifi: connection failed, starting accesspoint!')
        accesspoint()

def accesspoint():
    global _wlan

    print('Wifi: activating accesspoint.')
    _wlan.active(False)
    _ap.active(True)

def connected():
    return _wlan.isconnected()
    
def config():
    return _wlan.ifconfig()
    
def delete():
    import os
    os.remove("wifi_config.py")
    # TODO: clear internal wifi assignment
    accesspoint()

def remove():
    delete()

class SCAN:
    def __repr__(self):
        global _wlan
        state = _wlan.active()
        if not state:
            _wlan.active(True)
        nets = _wlan.scan()
        _wlan.active(state)
        return nets
    def __call__(self):
        l=""
        for n in self.__repr__():
            l=l+n[0].decode()+" %ddB\n"%n[3]
        return l
scan = SCAN()

class WIP:
    def __repr__(self):
        return config()[0]
    def __call__(self):
        return self.__repr__()
wip = WIP()

# write config and connect
def setup( name,  password, reset=True ):
    global wifi_config

    if name != wifi_config.name or \
          password != wifi_config.password:
        f=open("wifi_config.py", "w")
        f.write("name=\"%s\"\npassword=\"%s\"" % (name,password))
        f.close()
        print("Updated wifi_config.")
        wifi_config.name = name
        wifi_config.password = password
        if reset:
            print("Resetting system in 3 seconds.")
            time.sleep(1)
            webrepl.stop()
            time.sleep(2)
            machine.reset()
        else:
            connect()

# Try to find wifi_config
try:
    import wifi_config
    connect()
except ImportError:
    class wifi_config():
        pass
    wifi_config.name=""
    wifi_config.password=""
