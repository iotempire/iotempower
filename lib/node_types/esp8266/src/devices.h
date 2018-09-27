// devices.h
// list of all available device creation functions

#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <device.h>
#include <device-manager.h>

// use init_priority to make sure these get initialized last
#define ULNOIOT_DEVICE(Class_Name, internal_name, ...) \
    Class_Name ulnoiot_dev_ ## internal_name __attribute__((init_priority(65535))) \
        = Class_Name(__VA_ARGS__); \
    Class_Name& internal_name = (Class_Name&) ulnoiot_dev_ ## internal_name

// convenience macros for device definition
// Internal name (IN) references the internal name created with DN
#define IN(name) uiotdev_##name
#define _(name) IN(name)

#define ULNOIOT_DEVICE_(name, macro_name, ...) macro_name(uiotdev_##name, #name, __VA_ARGS__)


#include <output.h>
#define output(internal_name, ...) \
    ULNOIOT_DEVICE(Output, internal_name, __VA_ARGS__)
#define out(...) output(__VA_ARGS__)
#define led(...) output(__VA_ARGS__)
#define relais(...) output(__VA_ARGS__)
#define output_(name,...) ULNOIOT_DEVICE_(name, output, __VA_ARGS__)
#define out_(...) output_(__VA_ARGS__)
#define led_(...) output_(__VA_ARGS__)
#define relais_(...) output_(__VA_ARGS__)


#include <input.h>
#define input(internal_name, ...) \
    ULNOIOT_DEVICE(Input, internal_name, __VA_ARGS__)
#define button(...) input(__VA_ARGS__)
#define contact(...) input(__VA_ARGS__)
#define input_(name,...) ULNOIOT_DEVICE_(name, input, __VA_ARGS__)
#define button_(...) input_(__VA_ARGS__)
#define contact_(...) input_(__VA_ARGS__)


#include <edge_counter.h>
#define edge_counter(internal_name, ...) \
    ULNOIOT_DEVICE(Edge_Counter, internal_name, __VA_ARGS__)
#define edge_counter_(name,...) ULNOIOT_DEVICE_(name, edge_counter, __VA_ARGS__)


#include <pwm.h>
#define pwm(internal_name, ...) \
    ULNOIOT_DEVICE(Pwm, internal_name, __VA_ARGS__)
#define pwm_(name,...) ULNOIOT_DEVICE_(name, pwm, __VA_ARGS__)


#include <servo.h>
#define servo(internal_name, ...) \
    ULNOIOT_DEVICE(Servo, internal_name, __VA_ARGS__)
#define servo_(name,...) ULNOIOT_DEVICE_(name, servo, __VA_ARGS__)


#include <servo_switch.h>
#define servo_switch(internal_name, ...) \
    ULNOIOT_DEVICE(Servo_Switch, internal_name, __VA_ARGS__)
#define servo_switch_(name,...) ULNOIOT_DEVICE_(name, servo_switch, __VA_ARGS__)


#include <hcsr04.h>
#define hcsr04(internal_name, ...) \
    ULNOIOT_DEVICE(Hcsr04, internal_name, __VA_ARGS__)
#define hcsr04_(name,...) ULNOIOT_DEVICE_(name, hcsr04, __VA_ARGS__)


#include <analog.h>
#define analog(internal_name, ...) \
    ULNOIOT_DEVICE(Analog, internal_name, __VA_ARGS__)
#define analog_(name,...) ULNOIOT_DEVICE_(name, analog, __VA_ARGS__)


#include <th.h>
#define dht(internal_name, ...) \
    ULNOIOT_DEVICE(Dht, internal_name, __VA_ARGS__)
#define dht11(...) dht(__VA_ARGS__)
#define dht22(...) dht(__VA_ARGS__)
#define dht_(name,...) ULNOIOT_DEVICE_(name, dht, __VA_ARGS__)
#define dht11_(...) dht_(__VA_ARGS__)
#define dht22_(...) dht_(__VA_ARGS__)

#define ds18b20(internal_name, ...) \
    ULNOIOT_DEVICE(Ds18b20, internal_name, __VA_ARGS__)
#define dallas(...) ds18b20(__VA_ARGS__)
#define ds18b20_(name,...) ULNOIOT_DEVICE_(name, ds18b20, __VA_ARGS__)
#define dallas_(...) ds18b20_(__VA_ARGS__)


#include <rgb_single.h>
#define rgb_single(internal_name, ...) \
    ULNOIOT_DEVICE(RGB_Single, internal_name, __VA_ARGS__)
#define rgb_single_(name,...) ULNOIOT_DEVICE_(name, rgb_single, __VA_ARGS__)


#include <rgb_strip.h>
#define rgb_strip(internal_name, mqtt_name_str, led_count, type, ...) \
    CRGB (internal_name ## leds)[led_count]; \
    ULNOIOT_DEVICE(RGB_Strip, internal_name, mqtt_name_str, led_count, \
        FastLED.addLeds< type, __VA_ARGS__ >((internal_name ## leds), led_count))
#define rgb_strip_(name,...) ULNOIOT_DEVICE_(name, rgb_strip, __VA_ARGS__)


#include <rgb_matrix.h>
#define rgb_matrix(internal_name, ...) \
    ULNOIOT_DEVICE(RGB_Matrix, internal_name, __VA_ARGS__)
#define rgb_matrix_(name,...) ULNOIOT_DEVICE_(name, rgb_matrix, __VA_ARGS__)


#include <animator.h>
#define animator(internal_name, ...) \
    ULNOIOT_DEVICE(Animator, internal_name, __VA_ARGS__)
#define animator_(name,...) ULNOIOT_DEVICE_(name, animator, __VA_ARGS__)


#include <display.h>
#define display(internal_name, ...) \
    ULNOIOT_DEVICE(Display, internal_name, __VA_ARGS__)
#define display_(name,...) ULNOIOT_DEVICE_(name, display, __VA_ARGS__)

#define display44780(internal_name, ...) \
    ULNOIOT_DEVICE(Display44780, internal_name, __VA_ARGS__)
#define display44780_(name,...) ULNOIOT_DEVICE_(name, display44780, __VA_ARGS__)


#include <gyro.h>
#define gyro6050(internal_name, ...) \
    ULNOIOT_DEVICE(Gyro_MPU6050, internal_name, __VA_ARGS__)
#define gyro_(name,...) ULNOIOT_DEVICE_(name, gyro, __VA_ARGS__)


#endif // _DEVICES_H_