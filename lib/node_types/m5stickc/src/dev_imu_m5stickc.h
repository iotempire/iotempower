// dev_imu_m5stickc.h
// Header file for controlling the m5stickc MPU6886 IMU sensor

#ifndef _IOTEMPOWER_M5STICKC_IMU_H_
#define _IOTEMPOWER_M5STICKC_IMU_H_

#include <platform_includes.h>
#include <device.h>

class M5StickC_IMU : public Device {
    private:
        // bool _calibrate = false;
        // unsigned long starttime;
        // bool discard_done = false;
        bool _enable_gyro = true;
        bool _enable_ypr = true;
        bool _enable_accel = true;
        bool _enable_temp = true;

        // for averaging use exponential moving average (EMA)
        float accX_avg = 0;
        float accY_avg = 0;
        float accZ_avg = 0;

        float gyroX_avg = 0;
        float gyroY_avg = 0;
        float gyroZ_avg = 0;

        float pitch_avg = 0;
        float roll_avg = 0;
        float yaw_avg = 0;

        float temp_avg = 0;
        
        static const int WINDOW_SIZE = 20; // Define window size as a class constant
        const float alpha = 2.0 / (WINDOW_SIZE + 1); // Use window size directly to define alpha
    protected:
        bool init();
    public:
        M5StickC_IMU(const char* name, bool enable_gyro=true, 
            bool enable_accel=true, bool enable_ypr=true, bool enable_temp=true);
        void start() {
            if (init()) {
                _started = true;
            }
        }
        bool measure();

};

#endif // _IOTEMPOWER_M5STICKC_IMU_H_