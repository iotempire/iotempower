// Pins on Wemos D1 Mini

#define Pin(p) p

//// left (antenna front and up, usb back down)
// RST
// a0 machine.ADC(0)
#define d0 Pin(16)
#define d5 Pin(14)
#define clk d5
#define d6 Pin(12)
#define miso d6
#define d7 Pin(13)
#define mosi d7
#define d8 Pin(15)
#define cs d8
// 3.3V

//// right
#define tx Pin(1)
#define rx Pin(3)
#define d1 Pin(5)
#define d2 Pin(4)
#define d3 Pin(0)
#define d4 Pin(2)
#define onboardled d4
// GND
// 5V
