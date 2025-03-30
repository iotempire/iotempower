// Pins on M5StickC

#define BUTTON_RIGHT 39
#define BUTTON_HOME 37
#define SDA 21
#define SCL 22
#define IR 9
#define IRQ  35
#define FLASHBUTTON BUTTON_HOME
#define ONBOARDLED 10
// TODO: re-investigate, why pwm doesn't work on this led 
#define ONBOARDLED_FULL_GPIO 0
#ifndef BUZZER
#define BUZZER 2
#endif

#ifndef A0
#define A0 36
#endif

// test pins for IoTempower
#define IOT_TEST_INPUT BUTTON_HOME
#define IOT_TEST_OUTPUT ONBOARDLED
