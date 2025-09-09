// Pins on CYD (Cheap Yellow Display)
// all based on https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display
// and https://github.com/hexeguitar/ESP32_TFT_PIO/blob/main/Pics/cyd_pinmapping.gif

// The CYD touch uses some non default
// SPI pins
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

#define LDR 34

// #define SDA 21
// #define SCL 22
#define FLASHBUTTON 0

#define BACKLIGHT 21

#define ONBOARDLED_BLUE 17
#define ONBOARDLED_RED 4
#define ONBOARDLED_GREEN 16

#define ONBOARDLED ONBOARDLED_BLUE
//#define ONBOARDLED_FULL_GPIO 1
#ifndef A0
    #define A0 35
#endif

