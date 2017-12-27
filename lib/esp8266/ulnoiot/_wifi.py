# Wifi Configuration
#
import machine
import network
import time
# import webrepl
import unetrepl as nr

_ap = network.WLAN(network.AP_IF)
_ap.active(False)
_wlan = network.WLAN(network.STA_IF)
_wlan.active(True)
_config_file = "/wifi_cfg.py"
time.sleep(1)  # wait to become active


# connect to wifi
def connect():
    global wifi_cfg, _wlan

    _ap.active(False)
    _wlan.active(True)

    # removed scan of networks to allow connect to hidden essid
    # Try to connect
    _wlan.connect(wifi_cfg.name, wifi_cfg.password)
    tries = 15
    for i in range(tries):
        print("%d/%d. Trying to connect." % (i + 1, tries))
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
    nr.start(nostop=True)


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
    os.remove(_config_file)
    # TODO: clear internal wifi assignment
    accesspoint()


def remove():
    delete()


class SCAN:
    def __call__(self):
        global _wlan
        state = _wlan.active()
        if not state:
            _wlan.active(True)
        nets = _wlan.scan()
        _wlan.active(state)
        return nets

    def __repr__(self):
        l = ""
        for n in self.__call__():
            l = l + n[0].decode() + " %ddB\n" % n[3]
        return l


scan = SCAN()


class WIP:
    def __repr__(self):
        return config()[0]

    def __call__(self):
        return self.__repr__()


wip = WIP()


# write config and connect
def setup(name, password, reset=True):
    global wifi_cfg

    if name != wifi_cfg.name or \
            password != wifi_cfg.password:
        f = open(_config_file, "w")
        f.write("name=\"%s\"\npassword=\"%s\"" % (name, password))
        f.close()
        print("Updated wifi_cfg.")
        wifi_cfg.name = name
        wifi_cfg.password = password
        if reset:
            print("Resetting system in 3 seconds.")
            time.sleep(1)
            # webrepl.stop()
            nr.stop()
            time.sleep(2)
            machine.reset()
        else:
            connect()


# Try to find wifi_cfg
try:
    import wifi_cfg

    connect()
except ImportError:
    class wifi_cfg():
        pass


    wifi_cfg.name = ""
    wifi_cfg.password = ""
