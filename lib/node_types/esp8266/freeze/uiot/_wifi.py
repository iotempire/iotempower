# Wifi Configuration
#
import machine
import network
import time
import uiot._cfg as _cfg
import unetrepl as nr

# activate wifi network
_ap = network.WLAN(network.AP_IF)
_ap.active(False)
_wlan = network.WLAN(network.STA_IF)
_wlan.active(True)

_wlan_laststate = False


# connect to wifi and really try to connect
def connect_blocking():
    global _wlan

    activate()

    # no scan of networks to allow connect to hidden essid
    # Try to connect
    tries = 15
    for i in range(tries):
        print("%d/%d. Trying to connect." % (i + 1, tries))
        machine.idle()
        time.sleep(1)
        if connected():
            break

    if connected():
        print('Wifi: connection succeeded!')
        print(_wlan.ifconfig())
    else:
        print('Wifi: connection failed, starting accesspoint!')
        accesspoint()
    nr.start(nostop=True)


def activate():
    global _wlan

    _ap.active(False)
    _wlan.active(True)

    if _cfg.config.wifi_name:
        _wlan.connect(_cfg.config.wifi_name, _cfg.config.wifi_pw)


def deactivate():
    global _wlan
    _ap.active(False)
    _wlan.active(False)


def monitor():
    # needs to be called on regular basis
    global _wlan_laststate
    if _wlan_laststate != connected():  # connection status change
        if _wlan_laststate:  # network became inactive
            pass  # shoudl be retried by esp in background
        else:  # network became active
            nr.start(nostop=True)  # start netrepl
        _wlan_laststate = not _wlan_laststate  # there was a change, so toggle
    # TODO: consider activating AP mode, if not active for long time


def accesspoint():
    global _wlan

    print('Wifi: activating accesspoint.')
    _wlan.active(False)
    _ap.active(True)


def connected():
    return _wlan.isconnected() and _wlan.status() == network.STAT_GOT_IP


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
            nr.stop()
            time.sleep(2)
            machine.reset()
        else:
            activate()

# when module loaded the first time start blocking to also bring up netrepl at
# right time
connect_blocking()
