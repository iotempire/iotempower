# Installation test
packages = [
    #    {"name": "virtualenv", "package_manager": "binary", "module": "general"}, # can't be tested this way on modern python envs
    {"name": "git", "package_manager": "binary", "module": "general"},
    {"name": "jq", "package_manager": "binary", "module": "general"},
    {"name": "make", "package_manager": "binary", "module": "general"},
    {"name": "curl", "package_manager": "binary", "module": "general"},
    {"name": "mosquitto_sub", "package_manager": "binary", "module": "general"},
    {"name": "mosquitto_pub", "package_manager": "binary", "module": "general"},
    {"name": "node", "package_manager": "binary", "module": "general"},  # TODO: check min version
    #    {"name": "haveged", "package_manager": "binary", "module": "general"}, # not needed on termux, macos
    #    {"name": "python3-dev", "package_manager": "binary", "module": "general"}, # FIXME: could be quieried with query script
    {"name": "terminal-kit", "package_manager": "npm", "module": "general"},
    {"name": "g++", "package_manager": "binary", "module": "cloud_commander"},
    {"name": "gritty", "package_manager": "npm", "module": "cloud_commander"},
    {"name": "cloudcmd", "package_manager": "npm", "module": "cloud_commander"},
    {"name": "node-red", "package_manager": "npm", "module": "node_red"},
    #    {"name": "debian-keyring", "package_manager": "binary", "module": "caddy"},
    #    {"name": "apt-transport-https", "package_manager": "binary", "module": "caddy"},
    #    {"name": "debian-archive-keyring", "package_manager": "binary", "module": "caddy"},
    {"name": "caddy", "package_manager": "binary", "module": "caddy"},  # TODO: also better chek min version here
    {"name": "mosquitto", "package_manager": "binary", "module": "mosquitto"},
]

# Compilation test
boards = [
    "wemos_d1_mini",
    "sonoff",
    "nodemcu",
    "olimex",
    # "m5stickc_plus",
    "m5stickc",
    "wroom_02",
    "esp-m",
    "sonoff_pow",
    "esp8266",
    "esp32",
    "esp32minikit",
]
devices = [
    {"device_name": "input", "example_syntax": 'input(lower, IOT_TEST_INPUT, "released", "pressed");'},
    {"device_name": "output", "example_syntax": 'led(yellow, IOT_TEST_OUTPUT, "turn on", "turn off");'},
    {"device_name": "analog", "example_syntax": "analog(example_name);"},
    {"device_name": "bmp180", "example_syntax": "bmp180(example_name);"},
    {"device_name": "bmp280", "example_syntax": "bmp280(example_name);"},
    {"device_name": "dallas", "example_syntax": "dallas(example_name, IOT_TEST_DIGITAL);"},
    {"device_name": "gyro6050", "example_syntax": "gyro6050(example_name);"},
    {"device_name": "gyro9250", "example_syntax": "gyro9250(example_name);"},
    {"device_name": "gesture_apds9960", "example_syntax": "gesture_apds9960(example_name);"},
    {"device_name": "sgp30", "example_syntax": "sgp30(example_name);"},
    {"device_name": "edge_counter", "example_syntax": "edge_counter(example_name, IOT_TEST_DIGITAL);"},
    {"device_name": "dht", "example_syntax": "dht(example_name, IOT_TEST_DIGITAL);"},
    {"device_name": "ds18b20", "example_syntax": "ds18b20(example_name, IOT_TEST_DIGITAL);"},
]

isolated_combinations_to_test = [
    (board, device["device_name"], device["example_syntax"]) for device in devices for board in boards
]

rooms = ["room1", "room2", "room3"]
nodes = ["node1", "node2"]
devices = ["input", "output", "analog", "bmp180"]
