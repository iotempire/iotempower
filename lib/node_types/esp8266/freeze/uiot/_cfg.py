# Configuration file management
#

_file = "/config.py"

def write():
    global config
    f = open(_file, "w")
    f.write(
"""wifi_name = {}
wifi_pw = {}
netrepl = {}
mqtt_host = {}
mqtt_topic = {}
mqtt_user = {}
mqtt_pw = {}

""".format(
    config.wifi_name,
    config.wifi_pw,
    ) )
    f.close()


def wifi(name, password, save=True):
    # write wifi values to config file
    global config

    config.wifi_name = name
    config.wifi_pw = password
    if save:
        write()


def netrepl(key, save=True):
    global config
    config.key = key
    if save:
        write()


def mqtt(host, topic, user=None, pw=None, save=True):
    if user==None:
        user=""
    if pw==None:
        pw=""
    config.mqtt_host = host
    config.mqtt_topic = topic
    config.mqtt_user = user
    config.mqtt_pw = pw
    if save:
        write()



# Try to import config
try:
    import config

except ImportError:
    class config():
        pass

    config.wifi_name = ""
    config.wifi_pw = ""
    config.netrepl = ""
    config.mqtt_host = ""
    config.mqtt_topic = ""
    config.mqtt_user = ""
    config.mqtt_pw = ""
