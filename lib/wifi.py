# Wifi Configuration
#
import machine
import network
import time

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

    nets = _wlan.scan()
    for net in nets:
        ssid = net[0].decode()
        if ssid == wifi_config.name:
            print('Wifi: network %s found.' % ssid)
            _wlan.connect(ssid, wifi_config.password)
            tries=15
            for i in range(tries):
                print("%d/%d. Trying to connect." %(i+1, tries))
                machine.idle()
                time.sleep(1)
                if _wlan.isconnected(): break
            break

    if _wlan.isconnected():
        print('Wifi: connection succeeded!')
        print(_wlan.ifconfig())
    else:
        print('Wifi: connection failed!')
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
    
def scan():
    nets = _wlan.scan()
    l=[]
    for n in nets:
        l.append( n.ssid )
    return l


# write config and connect
def setup( name,  password ):
    global wifi_config

    f=open("wifi_config.py", "w")
    f.write("name=\"%s\"\npassword=\"%s\"" % (name,password))
    f.close()
    wifi_config.name = name
    wifi_config.password = password
    connect()
    
# Try to find wifi_config
try:
    import wifi_config
    connect()
except ImportError:
    class wifi_config():
        pass
