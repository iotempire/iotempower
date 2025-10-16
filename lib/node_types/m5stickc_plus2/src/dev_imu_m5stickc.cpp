// dev_imu_m5stickc.cpp
#include "dev_imu_m5stickc.h"

M5StickC_IMU::M5StickC_IMU(const char* name, bool enable_gyro, bool enable_accel, 
            bool enable_ypr, bool enable_temp)  : Device(name, 1000) { 
                // TODO: consider better timing and add filters
    
    _enable_gyro = enable_gyro;
    _enable_accel = enable_accel;
    _enable_ypr = enable_ypr;
    _enable_temp = enable_temp;

    if(enable_gyro) add_subdevice(new Subdevice(F("gyro"))); // 0
    if(enable_ypr) add_subdevice(new Subdevice(F("ypr"))); // 1
    if(enable_accel) add_subdevice(new Subdevice(F("acc"))); // 2
    if(enable_temp) add_subdevice(new Subdevice(F("temp"))); // 3
}

bool M5StickC_IMU::init() {
    //M5.Imu.Init();  
    measure();
    return true;
}


bool M5StickC_IMU::measure() {
    float accX;
    float accY;
    float accZ;

    float gyroX;
    float gyroY;
    float gyroZ;

    float pitch;
    float roll;
    float yaw;

    auto data = StickCP2.Imu.getImuData();
    accX = data.accel.x;      // accel x-axis value.
    accY = data.accel.y;      // accel y-axis value.
    accZ = data.accel.z;      // accel z-axis value.
    gyroX = data.gyro.x;      // gyro x-axis value.
    gyroX = data.gyro.y;      // gyro y-axis value.
    gyroX = data.gyro.z;      // gyro z-axis value.

    // Update running averages using EMA
    gyroX_avg = alpha * gyroX + (1 - alpha) * gyroX_avg;
    gyroY_avg = alpha * gyroY + (1 - alpha) * gyroY_avg;
    gyroZ_avg = alpha * gyroZ + (1 - alpha) * gyroZ_avg;

    accX_avg = alpha * accX + (1 - alpha) * accX_avg;
    accY_avg = alpha * accY + (1 - alpha) * accY_avg;
    accZ_avg = alpha * accZ + (1 - alpha) * accZ_avg;

    pitch_avg = alpha * pitch + (1 - alpha) * pitch_avg;
    roll_avg = alpha * roll + (1 - alpha) * roll_avg;
    yaw_avg = alpha * yaw + (1 - alpha) * yaw_avg;


    int value_nr=0;

    // TODO: revisit filtering and make configurable
    if(_enable_gyro) {
        // value(value_nr).printf("%.1f,%.1f,%.1f", (int)gyroX_avg, (int)gyroY_avg, (int)gyroZ_avg);
        value(value_nr).printf("%d,%d,%d", (int)gyroX_avg/2*2, (int)gyroY_avg/2*2, (int)gyroZ_avg/2*2);
        value_nr++;
    }
    if(_enable_accel) {
        value(value_nr).printf("%.1f,%.1f,%.1f", accX_avg, accY_avg, accZ_avg);
        value_nr++;
    }
    if(_enable_ypr) {
        // angles (yaw pitch roll/ypr)
        value(value_nr).printf("%d,%d,%d", (int)yaw_avg/2*2, (int)pitch_avg/2*2, (int)roll_avg/2*2);
        value_nr++;
    }
    if(_enable_temp) {
        value(value_nr).from((int)temp_avg/2*2);
    }

    return true;
}
