# Wifi Configuration
#
import machine
import network
import time
import uiot._cfg as _cfg
import unetrepl as nr


_ap = network.WLAN(network.AP_IF)
_ap.active(False)
_wlan = network.WLAN(network.STA_IF)
_wlan.active(True)


# connect to wifi
def connect():
    global _wlan

    _ap.active(False)
    _wlan.active(True)

    # removed scan of networks to allow connect to hidden essid
    # Try to connect
    if _cfg.config.wifi_name:
        _wlan.connect(_cfg.config.wifi_name, _cfg.config.wifi_pw)
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
    if name != _cfg.config.wifi_name or \
            password != _cfg.config.wifi_pw:
        _cfg.wifi(name, password)
        print("Updated wifi config.")
        if reset:
            print("Resetting system in 3 seconds.")
            time.sleep(1)
            # webrepl.stop()
            nr.stop()
            time.sleep(2)
            machine.reset()
        else:
            connect()
