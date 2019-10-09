// global/static iotempower configuration values

#ifndef _IOTEMPOWER_DEFAULT_H_
#define _IOTEMPOWER_DEFAULT_H_

//#define IOTEMPOWER_RECONFIG_MAGIC "uiotpassreset"
#define IOTEMPOWER_AP_RECONFIG_NAME "uiot-node"
#define IOTEMPOWER_AP_RECONFIG_PASSWORD "iotempire"
#define IOTEMPOWER "iotempower"
#define IOTEMPOWER_FLASH_DEFAULT_PASSWORD IOTEMPOWER
#define IOTEMPOWER_MAX_DEVICES 32
#define IOTEMPOWER_MAX_SUBDEVICES 16
#define IOTEMPOWER_MAX_STRLEN 127
// max length of a network buffer
#define IOTEMPOWER_MAX_BUFLEN 1024
#define MIN_PUBLISH_TIME_MS 20  // posting every 20ms allowed -> only 50messages per second (else network stacks seems to run full)- TODO: check if this is too conservative or too much
#define LOG_LINE_MAX_LEN 128
#define IOTEMPOWER_MAX_LED_STRIPS 8
#define IOTEMPOWER_MAX_ANIMATOR_COMMANDS 16
#define IOTEMPOWER_MAX_TRIGGER 4294967295L
#define IOTEMPOWER_DO_LATER_MAP_SIZE 64
#define IOTEMPOWER_MAX_DO_LATER_INTERVAL (3600000L*24L) // 24 hours max delay, close to half overrun (72h)
#endif // _IOTEMPOWER_DEFAULT_H_