// devices.h
// list of all available device creation functions

#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <device.h>
#include <device-manager.h>

// use init_priority to make sure these get initialized last
#define ULNOIOT_DEVICE_(Class_Name, internal_name, gcc_va_args...) \
    Class_Name ulnoiot_dev_ ## internal_name __attribute__((init_priority(65535))) \
        = Class_Name(gcc_va_args); \
    Class_Name& internal_name = (Class_Name&) ulnoiot_dev_ ## internal_name

// convenience macros for device definition
// Internal name (IN) references the internal name created with DN
#define IN(name) ulnoiot_dev_##name

#define ULNOIOT_DEVICE(name, macro_name, ...) macro_name(ulnoiot_dev_##name, #name , ##__VA_ARGS__)


#include <output.h>
#define output_(internal_name, ...) \
    ULNOIOT_DEVICE_(Output, internal_name, ##__VA_ARGS__)
#define out_(gcc_va_args...) output_(gcc_va_args)
#define led_(gcc_va_args...) output_(gcc_va_args)
#define relais_(gcc_va_args...) output_(gcc_va_args)
#define output(name, ...) ULNOIOT_DEVICE(name, output_, ##__VA_ARGS__)
#define out(gcc_va_args...) output(gcc_va_args)
#define led(gcc_va_args...) output(gcc_va_args)
#define relais(gcc_va_args...) output(gcc_va_args)


#include <input.h>
#define input_(internal_name, ...) \
    ULNOIOT_DEVICE_(Input, internal_name, ##__VA_ARGS__)
#define button_(gcc_va_args...) input_(gcc_va_args)
#define contact_(gcc_va_args...) input_(gcc_va_args)
#define input(name, ...) ULNOIOT_DEVICE(name, input_, ##__VA_ARGS__)
#define button(gcc_va_args...) input(gcc_va_args)
#define contact(gcc_va_args...) input(gcc_va_args)


#include <edge_counter.h>
#define edge_counter_(internal_name, ...) \
    ULNOIOT_DEVICE_(Edge_Counter, internal_name, ##__VA_ARGS__)
#define edge_counter(name, ...) ULNOIOT_DEVICE(name, edge_counter_, ##__VA_ARGS__)


#include <pwm.h>
#define pwm_(internal_name, ...) \
    ULNOIOT_DEVICE_(Pwm, internal_name, ##__VA_ARGS__)
#define pwm(name, ...) ULNOIOT_DEVICE(name, pwm_, ##__VA_ARGS__)


#include <servo.h>
#define servo_(internal_name, ...) \
    ULNOIOT_DEVICE_(Servo, internal_name, ##__VA_ARGS__)
#define servo(name, ...) ULNOIOT_DEVICE(name, servo_, ##__VA_ARGS__)


#include <servo_switch.h>
#define servo_switch_(internal_name, ...) \
    ULNOIOT_DEVICE_(Servo_Switch, internal_name, ##__VA_ARGS__)
#define servo_switch(name, ...) ULNOIOT_DEVICE(name, servo_switch_, ##__VA_ARGS__)


#include <hcsr04.h>
#define hcsr04_(internal_name, ...) \
    ULNOIOT_DEVICE_(Hcsr04, internal_name, ##__VA_ARGS__)
#define hcsr04(name, ...) ULNOIOT_DEVICE(name, hcsr04_, ##__VA_ARGS__)


#include <analog.h>
#define analog_(internal_name, ...) \
    ULNOIOT_DEVICE_(Analog, internal_name, ##__VA_ARGS__)
#define analog(name, ...) ULNOIOT_DEVICE(name, analog_, ##__VA_ARGS__)


#include <th.h>
#define dht_(internal_name, ...) \
    ULNOIOT_DEVICE_(Dht, internal_name, ##__VA_ARGS__)
#define dht11_(gcc_va_args...) dht_(gcc_va_args)
#define dht22_(gcc_va_args...) dht_(gcc_va_args)
#define dht(name, ...) ULNOIOT_DEVICE(name, dht_, ##__VA_ARGS__)
#define dht11(gcc_va_args...) dht(gcc_va_args)
#define dht22(gcc_va_args...) dht(gcc_va_args)

#define ds18b20_(internal_name, ...) \
    ULNOIOT_DEVICE_(Ds18b20, internal_name, ##__VA_ARGS__)
#define dallas_(gcc_va_args...) ds18b20_(gcc_va_args)
#define ds18b20(name, ...) ULNOIOT_DEVICE(name, ds18b20_, ##__VA_ARGS__)
#define dallas(gcc_va_args...) ds18b20(gcc_va_args)


#include <rgb_single.h>
#define rgb_single_(internal_name, ...) \
    ULNOIOT_DEVICE_(RGB_Single, internal_name, ##__VA_ARGS__)
#define rgb_single(name, ...) ULNOIOT_DEVICE(name, rgb_single_, ##__VA_ARGS__)


#include <rgb_strip.h>
#define rgb_strip_(internal_name, mqtt_name_str, led_count, ...) \
    CRGB (internal_name ## leds)[led_count]; \
    ULNOIOT_DEVICE_(RGB_Strip, internal_name, mqtt_name_str, led_count, \
        FastLED.addLeds< __VA_ARGS__ >((internal_name ## leds), led_count))
#define rgb_strip(name, ...) ULNOIOT_DEVICE(name, rgb_strip_, ##__VA_ARGS__)


#include <rgb_matrix.h>
#define rgb_matrix_(internal_name, ...) \
    ULNOIOT_DEVICE_(RGB_Matrix, internal_name, ##__VA_ARGS__)
#define rgb_matrix(name, ...) ULNOIOT_DEVICE(name, rgb_matrix_, ##__VA_ARGS__)


#include <animator.h>
#define animator_(internal_name, ...) \
    ULNOIOT_DEVICE_(Animator, internal_name, ##__VA_ARGS__)
#define animator(name, ...) ULNOIOT_DEVICE(name, animator_, ##__VA_ARGS__)


#include <display.h>
#define display_(internal_name, ...) \
    ULNOIOT_DEVICE_(Display, internal_name, ##__VA_ARGS__)
#define display(name, ...) ULNOIOT_DEVICE(name, display_, ##__VA_ARGS__)

#define display44780_(internal_name, ...) \
    ULNOIOT_DEVICE_(Display_HD44780_I2C, internal_name, ##__VA_ARGS__)
#define display44780(name, ...) ULNOIOT_DEVICE(name, display44780_, ##__VA_ARGS__)


#include <gyro.h>
#define gyro6050_(internal_name, ...) \
    ULNOIOT_DEVICE_(Gyro_MPU6050, internal_name, ##__VA_ARGS__)
#define gyro(name, ...) ULNOIOT_DEVICE(name, gyro, ##__VA_ARGS__)


#endif // _DEVICES_H_