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
    {"device_name": "servo", "example_syntax": "servo(example_name, IOT_TEST_DIGITAL, 800);"},
    {"device_name": "acoustic_distance", "example_syntax": "hcsr04(distance, IOT_TEST_DIGITAL, IOT_TEST_DIGITAL_2);"},
    {"device_name": "laser_distance", "example_syntax": "vl53l0x(example_name);"},
    {
        "device_name": "weight_sensor",
        "example_syntax": "hx711(example_name, IOT_TEST_DIGITAL, IOT_TEST_DIGITAL_2, 419.0, true);",
    },
    {"device_name": "light_sensor_1", "example_syntax": "bh1750(example_name);"},
    {"device_name": "light_sensor_2", "example_syntax": "tsl2561(example_name);"},
    {"device_name": "capacity_touch", "example_syntax": "mpr121(example_name);"},
    # {"device_name": "rgb_strip", "example_syntax": "rgb_strip(example_name, 7, WS2812B, IOT_TEST_DIGITAL, GRB);"},
]

isolated_combinations_to_test = [
    (board, device["device_name"], device["example_syntax"]) for device in devices for board in boards
]
# This additional tests are for the devices which are not supported by all devices
isolated_combinations_to_test += [
    (board, "rfid", "mfrc522(example_name);")
    for board in ["wemos_d1_mini", "nodemcu", "wroom_02", "esp32", "esp32miniki"]
]
isolated_combinations_to_test += [("wemos_d1_mini", "rgb_single", "rgb_single(r0, D6, D5, D0, true);")]

# Deployment test
gateway_host = "iotgateway"
default_username = "iot"
mqtt_listen_period = 15  # in seconds
local_bind_mqtt_port = 1884
private_key_file_path = "~/.ssh/id_rsa"
cases_for_deployment: list[tuple[str, list[str], list[tuple[str, str]]]] = [
    # Example: ("device_name", ['example_syntax'], [("{mqtt_topic_name}", "{default_message}")]),
    ("Wemos D1 Mini", ['led(led, LED_BUILTIN, "off", "on");'], [("testing-node/led", "on")]),
    ("Wemos D1 Mini", ['input(button, D3, "depressed", "pressed");'], [("testing-node/button", "depressed")]),
    (
        "Wemos D1 Mini",
        ['led(led, LED_BUILTIN, "off", "on");', 'input(button, D3, "depressed", "pressed");'],
        [("testing-node/led", "on"), ("testing-node/button", "depressed")],
    ),
]
