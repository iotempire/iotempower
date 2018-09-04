// devices.h
// list of all available device creation functions

#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <device.h>
#include <device-manager.h>

// use init_priority to make sure these get initialized last
#define ULNOIOT_DEVICE(Class_Name, device_name, ...) \
    Class_Name udev_main_ ## device_name __attribute__((init_priority(65535))) \
        = Class_Name(#device_name, __VA_ARGS__); \
    Class_Name& udev_ ## device_name = udev_main_ ## device_name


#include <output.h>
#define output(device_name, ...) \
    ULNOIOT_DEVICE(Output, device_name, __VA_ARGS__)
#define out(...) output(__VA_ARGS__)
#define led(...) output(__VA_ARGS__)
#define relais(...) output(__VA_ARGS__)


#include <input.h>
#define input(device_name, ...) \
    ULNOIOT_DEVICE(Input, device_name, __VA_ARGS__)
#define button(...) input(__VA_ARGS__)
#define contact(...) input(__VA_ARGS__)

#include <edge_counter.h>
#define edge_counter(device_name, ...) \
    ULNOIOT_DEVICE(Edge_Counter, device_name, __VA_ARGS__)

#include <pwm.h>
#define pwm(device_name, ...) \
    ULNOIOT_DEVICE(Pwm, device_name, __VA_ARGS__)

#include <servo.h>
#define servo(device_name, ...) \
    ULNOIOT_DEVICE(Servo, device_name, __VA_ARGS__)

#include <servo_switch.h>
#define servo_switch(device_name, ...) \
    ULNOIOT_DEVICE(Servo_Switch, device_name, __VA_ARGS__)

#include <hcsr04.h>
#define hcsr04(device_name, ...) \
    ULNOIOT_DEVICE(Hcsr04, device_name, __VA_ARGS__)

#include <analog.h>
#define analog(device_name, ...) \
    ULNOIOT_DEVICE(Analog, device_name, __VA_ARGS__)

#include <th.h>
#define dht(device_name, ...) \
    ULNOIOT_DEVICE(Dht, device_name, __VA_ARGS__)
#define dht11(...) dht(__VA_ARGS__)
#define dht22(...) dht(__VA_ARGS__)
#define ds18b20(device_name, ...) \
    ULNOIOT_DEVICE(Ds18b20, device_name, __VA_ARGS__)
#define dallas(...) ds18b20(__VA_ARGS__)

#include <rgb_single.h>
#define rgb_single(device_name, ...) \
    ULNOIOT_DEVICE(RGB_Single, device_name, __VA_ARGS__)

#include <rgb_strip.h>
#define rgb_strip(device_name, led_count, type, ...) \
    CRGB (name ## leds)[led_count]; \
    ULNOIOT_DEVICE(RGB_Strip, device_name, led_count, \
        FastLED.addLeds< type, __VA_ARGS__ >((name ## leds), led_count), __VA_ARGS__)

#include <rgb_matrix.h>
#define rgb_matrix(device_name, ...) \
    ULNOIOT_DEVICE(RGB_Matrix, device_name, __VA_ARGS__)

#include <animator.h>
#define animator(device_name, ...) \
    ULNOIOT_DEVICE(Animator, device_name, __VA_ARGS__)

#include <display.h>
#define display(device_name, ...) \
    ULNOIOT_DEVICE(Display, device_name, __VA_ARGS__)
#define display44780(device_name, ...) \
    ULNOIOT_DEVICE(Display44780, device_name, __VA_ARGS__)

#endif // _DEVICES_H_