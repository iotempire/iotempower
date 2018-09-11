// gyro.cpp
#include "gyro.h"

Gyro_MPU6050::Gyro_MPU6050(const char* name, bool calibrate_on_start) :
    I2C_Device(name) {
    _calibrate = calibrate_on_start;
    add_subdevice(new Subdevice("angles")); // 0
    add_subdevice(new Subdevice("gyro")); // 1
    add_subdevice(new Subdevice("acc")); // 2
    add_subdevice(new Subdevice("temperature")); // 3
}

void Gyro_MPU6050::start() {
    mpu6050 = new MPU6050(Wire);
    if(mpu6050) {
        mpu6050->begin();
        if(_calibrate) mpu6050->calcGyroOffsets(true);
        _started = true;
        measure();
    } else {
        ulog("Can't reserve memory for mpu6050.");
    }
}


bool Gyro_MPU6050::measure() {
    mpu6050->update();

    unsigned long current_time = millis();
    if(current_time - last_read < _read_interval) return false;
    last_read = current_time; 

    // angles
    measured_value(0).printf("%f,%f,%f",
        mpu6050->getAngleX(),
        mpu6050->getAngleY(),
        mpu6050->getAngleZ());

    // gyro
    measured_value(1).printf("%f,%f,%f %f,%f,%f",
        mpu6050->getGyroX(),
        mpu6050->getGyroY(),
        mpu6050->getGyroZ(),
        mpu6050->getGyroAngleX(),
        mpu6050->getGyroAngleY(),
        mpu6050->getGyroAngleZ());

    // acceleration
    measured_value(2).printf("%f,%f,%f %f,%f",
        mpu6050->getAccX(),
        mpu6050->getAccY(),
        mpu6050->getAccZ(),
        mpu6050->getAccAngleX(),
        mpu6050->getAccAngleY());
  
    // temperature
    measured_value(3).from(mpu6050->getTemp());

    return true;
}
