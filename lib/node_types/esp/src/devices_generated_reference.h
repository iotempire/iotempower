// File is obsolete and is now generated from devices.ini 
// to devices-generated.h
//
// TODO: remove file when not needed anymore

// IOTEMPOWER_COMMAND: output out led relais > none
#ifdef IOTEMPOWER_COMMAND_OUTPUT
#include <output.h>
#define output_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Output, internal_name, ##__VA_ARGS__)
#define out_(gcc_va_args...) output_(gcc_va_args)
#define led_(gcc_va_args...) output_(gcc_va_args)
#define relais_(gcc_va_args...) output_(gcc_va_args)
#define output(name, ...) IOTEMPOWER_DEVICE(name, output_, ##__VA_ARGS__)
#define out(gcc_va_args...) output(gcc_va_args)
#define led(gcc_va_args...) output(gcc_va_args)
#define relais(gcc_va_args...) output(gcc_va_args)
#endif // IOTEMPOWER_COMMAND_OUTPUT

// IOTEMPOWER_COMMAND: input button contact > none
#ifdef IOTEMPOWER_COMMAND_INPUT
#include <input.h>
#define input_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Input, internal_name, ##__VA_ARGS__)
#define button_(gcc_va_args...) input_(gcc_va_args)
#define contact_(gcc_va_args...) input_(gcc_va_args)
#define input(name, ...) IOTEMPOWER_DEVICE(name, input_, ##__VA_ARGS__)
#define button(gcc_va_args...) input(gcc_va_args)
#define contact(gcc_va_args...) input(gcc_va_args)
#endif // IOTEMPOWER_COMMAND_INPUT

// IOTEMPOWER_COMMAND: edge_counter > none
#ifdef IOTEMPOWER_COMMAND_EDGE_COUNTER
#include <edge_counter.h>
#define edge_counter_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Edge_Counter, internal_name, ##__VA_ARGS__)
#define edge_counter(name, ...) IOTEMPOWER_DEVICE(name, edge_counter_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_EDGE_COUNTER

// IOTEMPOWER_COMMAND: pwm > none
#ifdef IOTEMPOWER_COMMAND_PWM
#include <pwm.h>
#define pwm_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Pwm, internal_name, ##__VA_ARGS__)
#define pwm(name, ...) IOTEMPOWER_DEVICE(name, pwm_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_PWM

// IOTEMPOWER_COMMAND: servo > none
#ifdef IOTEMPOWER_COMMAND_SERVO
#include <emp_servo.h>
#define servo_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Emp_Servo, internal_name, ##__VA_ARGS__)
#define servo(name, ...) IOTEMPOWER_DEVICE(name, servo_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_SERVO

// IOTEMPOWER_COMMAND: servo_switch > none
#ifdef IOTEMPOWER_COMMAND_SERVO_SWITCH
#include <servo_switch.h>
#define servo_switch_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Servo_Switch, internal_name, ##__VA_ARGS__)
#define servo_switch(name, ...) IOTEMPOWER_DEVICE(name, servo_switch_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_SERVO_SWITCH

// IOTEMPOWER_COMMAND: hcsr04 ultra_sonic_distance > none
#ifdef IOTEMPOWER_COMMAND_HCSR04
#include <distance-hcsr04.h>
#define hcsr04_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Hcsr04, internal_name, ##__VA_ARGS__)
#define hcsr04(name, ...) IOTEMPOWER_DEVICE(name, hcsr04_, ##__VA_ARGS__)
#define ultra_sonic_distance(gcc_va_args...) hcsr04(gcc_va_args)
#endif // IOTEMPOWER_COMMAND_HCSR04


// IOTEMPOWER_COMMAND: vl53l0x > none
#ifdef IOTEMPOWER_COMMAND_VL53L0X
#include <distance-vl53l0x.h>
#define vl53l0x_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Distance_Vl53l0x, internal_name, ##__VA_ARGS__)
#define vl53l0x(name, ...) IOTEMPOWER_DEVICE(name, vl53l0x_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_VL53L0X

// IOTEMPOWER_COMMAND: hx711 > none
#ifdef IOTEMPOWER_COMMAND_HX711
#include <emp_hx711.h>
#define hx711_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Hx711, internal_name, ##__VA_ARGS__)
#define hx711(name, ...) IOTEMPOWER_DEVICE(name, hx711_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_HX711


// IOTEMPOWER_COMMAND: analog > none
#ifdef IOTEMPOWER_COMMAND_ANALOG
#include <analog.h>
#define analog_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Analog, internal_name, ##__VA_ARGS__)
#define analog(name, ...) IOTEMPOWER_DEVICE(name, analog_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_ANALOG

// IOTEMPOWER_COMMAND: dht dht11 dht22 dallas ds18b20 > none
#ifdef IOTEMPOWER_COMMAND_DHT
#include <th.h>
#define dht11_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Dht11, internal_name, ##__VA_ARGS__)
#define dht_(gcc_va_args...) dht11_(gcc_va_args)
#define dht22_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Dht22, internal_name, ##__VA_ARGS__)
#define dht11(name, ...) IOTEMPOWER_DEVICE(name, dht11_, ##__VA_ARGS__)
#define dht(gcc_va_args...) dht11(gcc_va_args)
#define dht22(name, ...) IOTEMPOWER_DEVICE(name, dht22_, ##__VA_ARGS__)

#define ds18b20_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Ds18b20, internal_name, ##__VA_ARGS__)
#define dallas_(gcc_va_args...) ds18b20_(gcc_va_args)
#define ds18b20(name, ...) IOTEMPOWER_DEVICE(name, ds18b20_, ##__VA_ARGS__)
#define dallas(gcc_va_args...) ds18b20(gcc_va_args)
#endif // IOTEMPOWER_COMMAND_DHT

// IOTEMPOWER_COMMAND: bmp180 bmp085 > none
#ifdef IOTEMPOWER_COMMAND_BMP180
#include <barometer-bmp180.h>
#define bmp180_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Barometer_BMP180, internal_name, ##__VA_ARGS__)
#define bmp180(name, ...) IOTEMPOWER_DEVICE(name, bmp180_, ##__VA_ARGS__)
#define bmp085_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Barometer_BMP180, internal_name, ##__VA_ARGS__)
#define bmp085(name, ...) IOTEMPOWER_DEVICE(name, bmp085_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_BMP180

// IOTEMPOWER_COMMAND: bmp280 > none
#ifdef IOTEMPOWER_COMMAND_BMP280
#include <barometer-bmp280.h>
#define bmp280_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Barometer_BMP280, internal_name, ##__VA_ARGS__)
#define bmp280(name, ...) IOTEMPOWER_DEVICE(name, bmp280_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_BMP280

// IOTEMPOWER_COMMAND: i2c_socket > none
#ifdef IOTEMPOWER_COMMAND_I2C_SOCKET
#include <i2c-socket.h>
#define i2c_socket_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(I2C_Socket, internal_name, ##__VA_ARGS__)
#define i2c_socket(name, ...) IOTEMPOWER_DEVICE(name, i2c_socket_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_I2C_SOCKET

// IOTEMPOWER_COMMAND: bh1750 > none
#ifdef IOTEMPOWER_COMMAND_BH1750
#include <lux-bh1750.h>
#define bh1750_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Lux_BH1750, internal_name, ##__VA_ARGS__)
#define bh1750(name, ...) IOTEMPOWER_DEVICE(name, bh1750_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_BH1750

// IOTEMPOWER_COMMAND: tsl2561 > none
#ifdef IOTEMPOWER_COMMAND_TSL2561
#include <lux-tsl2561.h>
#define tsl2561_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Lux_TSL2561, internal_name, ##__VA_ARGS__)
#define tsl2561(name, ...) IOTEMPOWER_DEVICE(name, tsl2561_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_TSL2561

// IOTEMPOWER_COMMAND: rgb_single > none
#ifdef IOTEMPOWER_COMMAND_RGB_SINGLE
#include <rgb_single.h>
#define rgb_single_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(RGB_Single, internal_name, ##__VA_ARGS__)
#define rgb_single(name, ...) IOTEMPOWER_DEVICE(name, rgb_single_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_RGB_SINGLE

// FastLed based rgb strip
// IOTEMPOWER_COMMAND: rgb_strip > none
#ifdef IOTEMPOWER_COMMAND_RGB_STRIP
#include <rgb_strip.h>
#define rgb_strip_(internal_name, mqtt_name_str, led_count, ...) \
    CRGB (internal_name ## leds)[led_count]; \
    IOTEMPOWER_DEVICE_(RGB_Strip, internal_name, mqtt_name_str, led_count, \
        FastLED.addLeds< __VA_ARGS__ >((internal_name ## leds), led_count))
#define rgb_strip(name, ...) IOTEMPOWER_DEVICE(name, rgb_strip_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_RGB_STRIP

// NeoPixelBus based rgb strip
// IOTEMPOWER_COMMAND: rgb_strip_bus > rgb_strip
#ifdef IOTEMPOWER_COMMAND_RGB_STRIP
#include <rgb_strip_bus.h>
#define F_RGB NeoRgbFeature
#define F_GRB NeoGrbFeature
#define F_BRG NeoBrgFeature
#define F_RGBW NeoRgbwFeature
#define rgb_strip_bus_(internal_name, mqtt_name_str, led_count, color_feature, control_method, pin) \
    RGB_Strip_Bus_Template<color_feature, control_method> (internal_name ## leds)(led_count, pin); \
    IOTEMPOWER_DEVICE_(RGB_Strip_Bus, internal_name, mqtt_name_str, led_count, \
        (internal_name ## leds))
#define rgb_strip_bus(name, ...) IOTEMPOWER_DEVICE(name, rgb_strip_bus_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_RGB_STRIP_BUS

// IOTEMPOWER_COMMAND: rgb_matrix > rgb_strip_bus rgb_strip
#ifdef IOTEMPOWER_COMMAND_RGB_MATRIX
#include <rgb_matrix.h>
#define rgb_matrix_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(RGB_Matrix, internal_name, ##__VA_ARGS__)
#define rgb_matrix(name, ...) IOTEMPOWER_DEVICE(name, rgb_matrix_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_RGB_MATRIX

// after rgb because of WAIT macro overload
// IOTEMPOWER_COMMAND: gesture_apds9960 > none
#ifdef IOTEMPOWER_COMMAND_GESTURE_APDS9960
#include <gesture-apds9960.h>
#define gesture_apds9960(name, ...) IOTEMPOWER_DEVICE(name, gesture_apds9960_, ##__VA_ARGS__)
#define gesture_apds9960_(internal_name, ...) \
	IOTEMPOWER_DEVICE_(Gesture_Apds9960, internal_name, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_GESTURE_APDS_9960

// IOTEMPOWER_COMMAND: animator > none
#ifdef IOTEMPOWER_COMMAND_ANIMATOR
#include <animator.h>
#define animator_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Animator, internal_name, ##__VA_ARGS__)
#define animator(name, ...) IOTEMPOWER_DEVICE(name, animator_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_ANIMATOR

// IOTEMPOWER_COMMAND: display display44780 > none
#ifdef IOTEMPOWER_COMMAND_DISPLAY
#include <display.h>
#define display_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Display, internal_name, ##__VA_ARGS__)
#define display(name, ...) IOTEMPOWER_DEVICE(name, display_, ##__VA_ARGS__)

#define display44780_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Display_HD44780_I2C, internal_name, ##__VA_ARGS__)
#define display44780(name, ...) IOTEMPOWER_DEVICE(name, display44780_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_DISPLAY

// IOTEMPOWER_COMMAND: gyro9250 > none
#ifdef IOTEMPOWER_COMMAND_GYRO9250
#include <gyro-mpu9250.h>
#define gyro9250_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Gyro_MPU9250, internal_name, ##__VA_ARGS__)
#define gyro9250(name, ...) IOTEMPOWER_DEVICE(name, gyro9250_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_GYRO9250

// IOTEMPOWER_COMMAND: gyro6050 gyro > none
#ifdef IOTEMPOWER_COMMAND_GYRO6050
#include <gyro.h>
#define gyro6050_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Gyro_MPU6050, internal_name, ##__VA_ARGS__)
#define gyro6050(name, ...) IOTEMPOWER_DEVICE(name, gyro6050_, ##__VA_ARGS__)
#define gyro_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Gyro_MPU6050, internal_name, ##__VA_ARGS__)
#define gyro(name, ...) IOTEMPOWER_DEVICE(name, gyro_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_GYRO6050

// IOTEMPOWER_COMMAND: mfrc522 rfid > none
#ifdef IOTEMPOWER_COMMAND_MFRC522
#include <mfrc522.h>
#define mfrc522_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(Mfrc522, internal_name, ##__VA_ARGS__)
#define mfrc522(name, ...) IOTEMPOWER_DEVICE(name, mfrc522_, ##__VA_ARGS__)
#define rfid(gcc_va_args...) mfrc522(gcc_va_args)
#endif // IOTEMPOWER_COMMAND_MFRC522

// IOTEMPOWER_COMMAND: mpr121 > none
#ifdef IOTEMPOWER_COMMAND_MPR121
#include <mpr121.h>
#define mpr121_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(MPR121, internal_name, ##__VA_ARGS__)
#define mpr121(name, ...) IOTEMPOWER_DEVICE(name, mpr121_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_MPR121

// IOTEMPOWER_COMMAND: ir_receiver > none
#ifdef IOTEMPOWER_COMMAND_IR_RECEIVER
#include <IRReceiver.h>
#define ir_receiver_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(IRReceiver, internal_name, ##__VA_ARGS__)
#define ir_receiver(name, ...) IOTEMPOWER_DEVICE(name, ir_receiver_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_IR_RECEIVER

// IOTEMPOWER_COMMAND: ir_sender > none
#ifdef IOTEMPOWER_COMMAND_IR_SENDER
#include <IRSender.h>
#define ir_sender_(internal_name, ...) \
    IOTEMPOWER_DEVICE_(IRSender, internal_name, ##__VA_ARGS__)
#define ir_sender(name, ...) IOTEMPOWER_DEVICE(name, ir_sender_, ##__VA_ARGS__)
#endif // IOTEMPOWER_COMMAND_IR_SENDER

