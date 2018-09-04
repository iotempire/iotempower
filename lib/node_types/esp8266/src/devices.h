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


#include <output.h>
#define output(internal_name, ...) \
    ULNOIOT_DEVICE(Output, internal_name, __VA_ARGS__)
#define out(...) output(__VA_ARGS__)
#define led(...) output(__VA_ARGS__)
#define relais(...) output(__VA_ARGS__)


#include <input.h>
#define input(internal_name, ...) \
    ULNOIOT_DEVICE(Input, internal_name, __VA_ARGS__)
#define button(...) input(__VA_ARGS__)
#define contact(...) input(__VA_ARGS__)

#include <edge_counter.h>
#define edge_counter(internal_name, ...) \
    ULNOIOT_DEVICE(Edge_Counter, internal_name, __VA_ARGS__)

#include <pwm.h>
#define pwm(internal_name, ...) \
    ULNOIOT_DEVICE(Pwm, internal_name, __VA_ARGS__)

#include <servo.h>
#define servo(internal_name, ...) \
    ULNOIOT_DEVICE(Servo, internal_name, __VA_ARGS__)

#include <servo_switch.h>
#define servo_switch(internal_name, ...) \
    ULNOIOT_DEVICE(Servo_Switch, internal_name, __VA_ARGS__)

#include <hcsr04.h>
#define hcsr04(internal_name, ...) \
    ULNOIOT_DEVICE(Hcsr04, internal_name, __VA_ARGS__)

#include <analog.h>
#define analog(internal_name, ...) \
    ULNOIOT_DEVICE(Analog, internal_name, __VA_ARGS__)

#include <th.h>
#define dht(internal_name, ...) \
    ULNOIOT_DEVICE(Dht, internal_name, __VA_ARGS__)
#define dht11(...) dht(__VA_ARGS__)
#define dht22(...) dht(__VA_ARGS__)
#define ds18b20(internal_name, ...) \
    ULNOIOT_DEVICE(Ds18b20, internal_name, __VA_ARGS__)
#define dallas(...) ds18b20(__VA_ARGS__)

#include <rgb_single.h>
#define rgb_single(internal_name, ...) \
    ULNOIOT_DEVICE(RGB_Single, internal_name, __VA_ARGS__)

#include <rgb_strip.h>
#define rgb_strip(internal_name, mqtt_name_str, led_count, type, ...) \
    CRGB (name ## leds)[led_count]; \
    ULNOIOT_DEVICE(RGB_Strip, internal_name, mqtt_name_str, led_count, \
        FastLED.addLeds< type, __VA_ARGS__ >((name ## leds), led_count))

#include <rgb_matrix.h>
#define rgb_matrix(internal_name, ...) \
    ULNOIOT_DEVICE(RGB_Matrix, internal_name, __VA_ARGS__)

#include <animator.h>
#define animator(internal_name, ...) \
    ULNOIOT_DEVICE(Animator, internal_name, __VA_ARGS__)

#include <display.h>
#define display(internal_name, ...) \
    ULNOIOT_DEVICE(Display, internal_name, __VA_ARGS__)
#define display44780(internal_name, ...) \
    ULNOIOT_DEVICE(Display44780, internal_name, __VA_ARGS__)

#endif // _DEVICES_H_