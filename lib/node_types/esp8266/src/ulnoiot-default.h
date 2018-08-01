// global/static ulnoiot configuration values

#ifndef _ULNOIOT_DEFAULT_H_
#define _ULNOIOT_DEFAULT_H_

#define ULNOIOT_RECONFIG_MAGIC "uiotpassreset"
#define ULNOIOT_AP_RECONFIG_NAME "uiot-node"
#define ULNOIOT_AP_RECONFIG_PASSWORD "internetofthings"
#define ULNOIOT "ulnoiot"
#define ULNOIOT_FLASH_DEFAULT_PASSWORD ULNOIOT
#define ULNOIOT_MAX_DEVICES 32
#define ULNOIOT_MAX_SUBDEVICES 16
#define ULNOIOT_MAX_STRLEN 63
#define ULNOIOT_MAX_BUFLEN 127
#define MIN_PUBLISH_TIME_US 20000  // posting every 20ms (20000us) allowed -> only 50messages per second (else network stacks seems to run full)- TODO: check if this is too conservative or too much
#define LOG_LINE_MAX_LEN 128

#endif // _ULNOIOT_DEFAULT_H_