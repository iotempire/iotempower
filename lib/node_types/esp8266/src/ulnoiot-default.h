// global/static ulnoiot configuration values

#ifndef _ULNOIOT_DEFAULT_H_
#define _ULNOIOT_DEFAULT_H_

#define ULNOIOT_RECONFIG_MAGIC "uiotpassreset"
#define ULNOIOT_AP_RECONFIG_NAME "uiot-node"
#define ULNOIOT_AP_RECONFIG_PASSWORD ""
#define ULNOIOT "ulnoiot"
#define ULNOIOT_FLASH_DEFAULT_PASSWORD ULNOIOT
#define ULNOIOT_MAX_DEVICES 32
#define ULNOIOT_MAX_SUBDEVICES 16
#define ULNOIOT_MAX_STRLEN 63
#define ULNOIOT_MAX_BUFLEN 127
#define MIN_PUBLISH_TIME_MS 20  // posting every 20ms allowed -> only 50messages per second (else network stacks seems to run full)- TODO: check if this is too conservative or too much
#define LOG_LINE_MAX_LEN 128
#define ULNOIOT_MAX_LED_STRIPS 8
#define ULNOIOT_MAX_ANIMATOR_COMMANDS 16
#define ULNOIOT_MAX_TRIGGER 4294967295L
#define ULNOIOT_DO_LATER_MAP_SIZE 64
#define ULNOIOT_MAX_DO_LATER_INTERVAL (3600000L*24L) // 24 hours max delay, close to half overrun (72h)
#endif // _ULNOIOT_DEFAULT_H_