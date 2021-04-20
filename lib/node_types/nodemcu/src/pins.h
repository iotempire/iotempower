// Most are in pins_arduino.h
// Pins on Wemos D1 Mini

//#define Pin(p) p


// Wemos D1 mini
// //// left (antenna front and up, usb back down)
// // RST
// // A0 ADC 0
// #define D0 16
#define WAKE D0
// #define D5 14
#define CLK D5
// #define D6 12
#define MISO D6
// #define D7 13
#define MOSI D7
// #define D8 15
#define CS D8
#define SS D8 // or sometimes SDA (for example on RFID-RC522)
// // 3.3V

// //// right
// #define TX 1
#define UART0 TX
// #define RX 3
#define DMA RX
// #define D1 5
// #define SCL D1
// #define D2 4
// #define SDA D2
// #define D3 0
#define FLASHBUTTON D3
// #define D4 2
#define ONBOARDLED D4
#define UART1 D4
// // GND
// // 5V


//// Nodemcu
// Most are in pins_arduino.h
// Pins on ESP-M Nodemcu board
// based on:
// https://samnicholls.net/wp-content/uploads/2016/11/esp8266_devkit_horizontal-01.png
// https://alexbloggt.com/wp-content/uploads/2015/10/nodemcu_pinout_700.png
// These are all internal for the esp8266 and modify the internal flash
// sdd1 = Pin(8)
// sdcmd = Pin(11)
// sdd0 = Pin(10)
// sdclk = Pin(6)
// gnd
// 3.3V
// EN
// RST
// GND
// Vin

// right
// d0 = Pin(16)
#define WAKE D0
// d1 = Pin(5)
// scl = d1
// d2 = Pin(4)
// sda = d2
// d3 = Pin(0)
//#define FLASHBUTTON D3
// d4 = Pin(2)
//#define ONBOARDLED D4
// 3.3V
// GND
// d5 = Pin(14)
//#define CLK D5
// d6 = Pin(12)
//#define MISO D6
// miso = d6
// d7 = Pin(13)
//#define MOSI D7
// d8 = Pin(15)
//#define CS D8
// rx = Pin(3)
// tx = Pin(1)
// GND
// 3.3V


// left (antenna is up, usb port down)
// a0 = machine.ADC(0)
// reserved
// reserved
// #define D10 10
#define SD3 = D10
#define SDD3 = S10
// #define D9 9
#define SD2 = D9
#define SDD2 = D9
