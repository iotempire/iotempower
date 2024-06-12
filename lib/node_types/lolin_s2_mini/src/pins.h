// Pins on M5StickC

#define SDA 21
#define SCL 22
#define IRQ  35
#define FLASHBUTTON 0
#define ONBOARDLED 15
#define ONBOARDLED_FULL_GPIO 1
#ifndef A0
    #define A0 36
#endif

// test pins for IoTempower
#define IOT_TEST_INPUT FLASHBUTTON
#define IOT_TEST_OUTPUT ONBOARDLED
#define IOT_TEST_DIGITAL 26
