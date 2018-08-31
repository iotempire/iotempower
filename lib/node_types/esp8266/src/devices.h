// devices.h
// list of all available device creation functions

#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <device.h>
#include <device-manager.h>

bool add_device(Device& device);

#define CHECK_DEV(dev) \
    if((dev) != NULL) { \
        if(!add_device(*(dev))) { \
            delete (dev); \
            (dev) = NULL; \
            ulog("Couldn't allocate new device."); \
        } \
    } \
    return *(dev);


#include <output.h>
Output& output(const char* name, const int pinnr, 
        const char* high="on", const char* low="off") {
    Output* dev = new Output(name, pinnr, high, low);
    CHECK_DEV(dev)
}
#define out(...) output(__VA_ARGS__)
#define led(...) output(__VA_ARGS__)
#define relais(...) output(__VA_ARGS__)


#include <input.h>
// can also do with_pullup and with_threshold
Input& input(const char* name, int pinnr, 
        const char* high="on", const char* low="off") {
    Input* dev = new Input(name, pinnr, high, low);
    CHECK_DEV(dev)
}
#define button(...) input(__VA_ARGS__)
#define contact(...) input(__VA_ARGS__)

#include <edge_counter.h>
Edge_Counter& edge_counter(const char* name, int pinnr, bool rising=true, bool falling=true) {
    Edge_Counter* dev = new Edge_Counter(name, pinnr, rising, falling);
    CHECK_DEV(dev)
}

#include <pwm.h>
Pwm& pwm(const char* name, uint8_t pin, int frequency=1000) {
    Pwm* dev = new Pwm(name, pin, frequency);
    CHECK_DEV(dev)
}

#include <servo.h>
Servo& servo(const char* name, uint8_t pin) {
    Servo* dev = new Servo(name, pin);
    CHECK_DEV(dev)
}

#include <hcsr04.h>
Hcsr04& hcsr04(const char* name, uint8_t trigger_pin, uint8_t echo_pin, 
        unsigned long timeout_us=30000) {
    Hcsr04* dev = new Hcsr04(name, trigger_pin, echo_pin, timeout_us);
    CHECK_DEV(dev)
}

#include <analog.h>
Analog& analog(const char* name) {
    Analog* dev = new Analog(name);
    CHECK_DEV(dev)
}

#include <th.h>
Dht& dht(const char* name, int pinnr) {
    Dht* dev = new Dht(name, pinnr);
    CHECK_DEV(dev)
}
#define dht11(...) dht(__VA_ARGS__)
#define dht22(...) dht(__VA_ARGS__)

Ds18b20& ds18b20(const char* name, int pinnr) {
    Ds18b20* dev = new Ds18b20(name, pinnr);
    CHECK_DEV(dev)
}
#define dallas(...) ds18b20(__VA_ARGS__)


#include <rgb_single.h>
RGB_Single& rgb(const char* name, int pin_r, int pin_g, int pin_b, 
        bool invert_signal=false) {
    RGB_Single* dev = new RGB_Single(name, pin_r, pin_g, pin_b, invert_signal);
    CHECK_DEV(dev)
}

#include <rgb_strip.h>
// TODO: fix error checking in this module, this can crash horribly if there is no
// memory left
#define rgb_strip(name, led_count, type, ...) \
     *(([&] () { \
        RGB_Strip* dev = CREATE_RGB_STRIP(name, led_count, type, __VA_ARGS__); \
        if((dev) != NULL) { \
            if(!add_device(*(dev))) { \
                delete (dev); \
                (dev) = NULL; \
                ulog("Couldn't allocate new device."); \
            } \
        } \
        return (RGB_Strip*) dev; })())


#include <rgb_matrix.h>
RGB_Matrix& rgb_matrix(const char* name, int width, int height) {
    RGB_Matrix* dev = new RGB_Matrix(name, width, height);
    CHECK_DEV(dev)
}
RGB_Matrix& rgb_matrix(const char* name, RGB_Base& strip) {
    RGB_Matrix* dev = new RGB_Matrix(name, strip);
    CHECK_DEV(dev)
}

#include <animator.h>
Animator& animator(const char* name) {
    Animator* dev = new Animator(name);
    CHECK_DEV(dev)
}
Animator& animator(const char* name, RGB_Matrix &matrix) {
    Animator* dev = new Animator(name, matrix);
    CHECK_DEV(dev)
}

#include <display.h>
Display& display(const char* name, U8G2& u8g2_display,
        const uint8_t* font=font_medium) {
    Display* dev = new Display(name, u8g2_display, font);
    CHECK_DEV(dev)
}
// TODO: do we need to specify default address 0x3c? 
Display& display(const char* name, 
        const uint8_t* font=font_medium) {
    auto* d = new U8G2_SSD1306_128X64_NONAME_1_HW_I2C(U8G2_R0);
    Display* dev = NULL;
    if(!d) {
        ulog("u8g2 creation failed.");
    } else {
        dev = new Display(name, *d, font);
    }
    CHECK_DEV(dev)
}
Display& display(const char* name, uint8_t scl, uint8_t sda,
        const uint8_t* font=font_medium) {
    auto* d = new U8G2_SSD1306_128X64_NONAME_1_SW_I2C(U8G2_R0, scl, sda);
    Display* dev = NULL;
    if(!d) {
        ulog("u8g2 creation failed.");
    } else {
        dev = new Display(name, *d, font);
    }
    CHECK_DEV(dev)
}
Display_HD44780_I2C& display44780(const char* name, int w, int h, 
        uint8_t scl, uint8_t sda, int i2c_addr=0x27) {
    Display_HD44780_I2C* dev = new Display_HD44780_I2C(name, w, h, scl, sda, i2c_addr);
    CHECK_DEV(dev)
}
Display_HD44780_I2C& display44780(const char* name, int w, int h, 
        int i2c_addr=0x27) {
    Display_HD44780_I2C* dev = new Display_HD44780_I2C(name, w, h, i2c_addr);
    CHECK_DEV(dev)
}

#endif // _DEVICES_H_