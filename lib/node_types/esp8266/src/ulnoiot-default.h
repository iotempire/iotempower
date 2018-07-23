// global/static ulnoiot configuration values

#define ULNOIOT_RECONFIG_MAGIC "uiotpassreset"
#define ULNOIOT_AP_RECONFIG_NAME "uiot-node"
#define ULNOIOT_AP_RECONFIG_PASSWORD "internetofthings"
#define ULNOIOT "ulnoiot"
#define ULNOIOT_FLASH_DEFAULT_PASSWORD ULNOIOT
#define ULNOIOT_MAX_DEVICES 32
#define ULNOIOT_MAX_SUBDEVICES 16
#define ULNOIOT_MAX_STRLEN 31
#define ULNOIOT_MAX_BUFLEN 255
#define MIN_PUBLISH_TIME_US 100000  // posting every 100ms (100000us) allowed -> only 10messages per second (else network stacks seesm to run full)- TODO: check if this is too conservative
