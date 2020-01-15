// gyro-mpu9250.cpp
#include "gyro-mpu9250.h"

Gyro_MPU9250::Gyro_MPU9250(const char *name) : I2C_Device(name)
{
    add_subdevice(new Subdevice(F("ypr"))); // 0
    add_subdevice(new Subdevice(F("acc"))); // 1
    add_subdevice(new Subdevice(F("mag"))); // 2
    add_subdevice(new Subdevice(F("temperature"))); // 3
    add_subdevice(new Subdevice(F("set"),true))->with_receive_cb(
        [&] (const Ustring& payload) {
            declination = payload.as_float();
            return true;
        });
    set_address(MPU9250_ADDRESS_AD0);
}

void Gyro_MPU9250::i2c_start()
{
    mpu9250 = new MPU9250(get_address(), get_wire(), get_clock());

    if (mpu9250)
    {
        // Who am i check
        byte c = mpu9250->readByte(get_address(), WHO_AM_I_MPU9250);

        if (c == 0x71) // WHO_AM_I should always be 0x71
        {
            ulog(F("MPU9250 is online..."));
            // Start by performing self test and reporting values
            mpu9250->MPU9250SelfTest(mpu9250->selfTest);
            // TODO: consider removing the following debug messages
            Serial.print(F("x-axis self test: acceleration trim within : "));
            Serial.print(mpu9250->selfTest[0], 1);
            Serial.println("% of factory value");
            Serial.print(F("y-axis self test: acceleration trim within : "));
            Serial.print(mpu9250->selfTest[1], 1);
            Serial.println("% of factory value");
            Serial.print(F("z-axis self test: acceleration trim within : "));
            Serial.print(mpu9250->selfTest[2], 1);
            Serial.println("% of factory value");
            Serial.print(F("x-axis self test: gyration trim within : "));
            Serial.print(mpu9250->selfTest[3], 1);
            Serial.println("% of factory value");
            Serial.print(F("y-axis self test: gyration trim within : "));
            Serial.print(mpu9250->selfTest[4], 1);
            Serial.println("% of factory value");
            Serial.print(F("z-axis self test: gyration trim within : "));
            Serial.print(mpu9250->selfTest[5], 1);
            Serial.println("% of factory value");

            // Calibrate gyro and accelerometers, load biases in bias registers
            mpu9250->calibrateMPU9250(mpu9250->gyroBias, mpu9250->accelBias);

            mpu9250->initMPU9250();
            // Initialize device for active mode read of accelerometer, gyroscope, and
            // temperature
            Serial.println("MPU9250 initialized for active data mode....");

            // Read the WHO_AM_I register of the magnetometer, this is a good test of
            // communication
            byte d = mpu9250->readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
            if (d != 0x48)
            {
                // Communication failed, stop here
                Serial.println(F("Communication to magnetometer of MPU9250 failed."));
            }
            else
            {
                // Get magnetometer calibration from AK8963 ROM
                mpu9250->initAK8963(mpu9250->factoryMagCalibration);
                // Initialize device for active mode read of magnetometer
                ulog(F("MPU9250/AK8963 initialized for active data mode...."));

                // Get sensor resolutions, only need to do this once
                mpu9250->getAres();
                mpu9250->getGres();
                mpu9250->getMres();

                // The next call delays for 4 seconds, and then records about 15 seconds of
                // data to calculate bias and scale.
                //    mpu9250->magCalMPU9250(mpu9250->magBias, mpu9250->magScale); // TODO: does this need to be activated
                Serial.println("AK8963 mag biases (mG)");
                Serial.println(mpu9250->magBias[0]);
                Serial.println(mpu9250->magBias[1]);
                Serial.println(mpu9250->magBias[2]);

                Serial.println("AK8963 mag scale (mG)");
                Serial.println(mpu9250->magScale[0]);
                Serial.println(mpu9250->magScale[1]);
                Serial.println(mpu9250->magScale[2]);

                _started = true;
            }
        }
        else
        {
            ulog(F("MPU9250 is not reacting correctly."));
        }
    }
    else
    {
        ulog(F("Can't reserve memory for MPU9250."));
    }
}

bool Gyro_MPU9250::measure()
{
    // If intPin goes high, all data registers have new data
    // On interrupt, check if data ready interrupt
    if (mpu9250->readByte(get_address(), INT_STATUS) & 0x01)
    {
        mpu9250->readAccelData(mpu9250->accelCount); // Read the x/y/z adc values

        // Now we'll calculate the acceleration value into actual g's
        // This depends on scale being set
        mpu9250->ax = (float)mpu9250->accelCount[0] * mpu9250->aRes; // - mpu9250->accelBias[0];
        mpu9250->ay = (float)mpu9250->accelCount[1] * mpu9250->aRes; // - mpu9250->accelBias[1];
        mpu9250->az = (float)mpu9250->accelCount[2] * mpu9250->aRes; // - mpu9250->accelBias[2];

        mpu9250->readGyroData(mpu9250->gyroCount); // Read the x/y/z adc values

        // Calculate the gyro value into actual degrees per second
        // This depends on scale being set
        mpu9250->gx = (float)mpu9250->gyroCount[0] * mpu9250->gRes;
        mpu9250->gy = (float)mpu9250->gyroCount[1] * mpu9250->gRes;
        mpu9250->gz = (float)mpu9250->gyroCount[2] * mpu9250->gRes;

        mpu9250->readMagData(mpu9250->magCount); // Read the x/y/z adc values

        // Calculate the magnetometer values in milliGauss
        // Include factory calibration per data sheet and user environmental
        // corrections
        // Get actual magnetometer value, this depends on scale being set
        mpu9250->mx = (float)mpu9250->magCount[0] * mpu9250->mRes * mpu9250->factoryMagCalibration[0] - mpu9250->magBias[0];
        mpu9250->my = (float)mpu9250->magCount[1] * mpu9250->mRes * mpu9250->factoryMagCalibration[1] - mpu9250->magBias[1];
        mpu9250->mz = (float)mpu9250->magCount[2] * mpu9250->mRes * mpu9250->factoryMagCalibration[2] - mpu9250->magBias[2];
    } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

    // Must be called before updating quaternions!
    mpu9250->updateTime();

    // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
    // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
    // (+ up) of accelerometer and gyro! We have to make some allowance for this
    // orientationmismatch in feeding the output to the quaternion filter. For the
    // MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
    // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
    // modified to allow any convenient orientation convention. This is ok by
    // aircraft orientation standards! Pass gyro rate as rad/s
    MahonyQuaternionUpdate(mpu9250->ax, mpu9250->ay, mpu9250->az, mpu9250->gx * DEG_TO_RAD,
                           mpu9250->gy * DEG_TO_RAD, mpu9250->gz * DEG_TO_RAD, mpu9250->my,
                           mpu9250->mx, mpu9250->mz, mpu9250->deltat);

    mpu9250->delt_t = millis() - mpu9250->count;

    // // Print acceleration values in milligs!
    // Serial.print("X-acceleration: ");
    // Serial.print(1000 * mpu9250->ax);
    // Serial.print(" mg ");
    // Serial.print("Y-acceleration: ");
    // Serial.print(1000 * mpu9250->ay);
    // Serial.print(" mg ");
    // Serial.print("Z-acceleration: ");
    // Serial.print(1000 * mpu9250->az);
    // Serial.println(" mg ");

    // // Print gyro values in degree/sec
    // Serial.print("X-gyro rate: ");
    // Serial.print(mpu9250->gx, 3);
    // Serial.print(" degrees/sec ");
    // Serial.print("Y-gyro rate: ");
    // Serial.print(mpu9250->gy, 3);
    // Serial.print(" degrees/sec ");
    // Serial.print("Z-gyro rate: ");
    // Serial.print(mpu9250->gz, 3);
    // Serial.println(" degrees/sec");

    // // Print mag values in degree/sec
    // Serial.print("X-mag field: ");
    // Serial.print(mpu9250->mx);
    // Serial.print(" mG ");
    // Serial.print("Y-mag field: ");
    // Serial.print(mpu9250->my);
    // Serial.print(" mG ");
    // Serial.print("Z-mag field: ");
    // Serial.print(mpu9250->mz);
    // Serial.println(" mG");

    mpu9250->tempCount = mpu9250->readTempData(); // Read the adc values
    // Temperature in degrees Centigrade
    mpu9250->temperature = ((float)mpu9250->tempCount) / 333.87 + 21.0;
    // // Print temperature in degrees Centigrade
    // Serial.print("Temperature is ");
    // Serial.print(mpu9250->temperature, 1);
    // Serial.println(" degrees C");

    // Define output variables from updated quaternion---these are Tait-Bryan
    // angles, commonly used in aircraft orientation. In this coordinate system,
    // the positive z-axis is down toward Earth. Yaw is the angle between Sensor
    // x-axis and Earth magnetic North (or true North if corrected for local
    // declination, looking down on the sensor positive yaw is counterclockwise.
    // Pitch is angle between sensor x-axis and Earth ground plane, toward the
    // Earth is positive, up toward the sky is negative. Roll is angle between
    // sensor y-axis and Earth ground plane, y-axis up is positive roll. These
    // arise from the definition of the homogeneous rotation matrix constructed
    // from quaternions. Tait-Bryan angles as well as Euler angles are
    // non-commutative; that is, the get the correct orientation the rotations
    // must be applied in the correct order which for this configuration is yaw,
    // pitch, and then roll.
    // For more see
    // http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    // which has additional links.
    mpu9250->yaw = atan2(2.0f * (*(getQ() + 1) * *(getQ() + 2) + *getQ() * *(getQ() + 3)), *getQ() * *getQ() + *(getQ() + 1) * *(getQ() + 1) - *(getQ() + 2) * *(getQ() + 2) - *(getQ() + 3) * *(getQ() + 3));
    mpu9250->pitch = -asin(2.0f * (*(getQ() + 1) * *(getQ() + 3) - *getQ() * *(getQ() + 2)));
    mpu9250->roll = atan2(2.0f * (*getQ() * *(getQ() + 1) + *(getQ() + 2) * *(getQ() + 3)), *getQ() * *getQ() - *(getQ() + 1) * *(getQ() + 1) - *(getQ() + 2) * *(getQ() + 2) + *(getQ() + 3) * *(getQ() + 3));
    mpu9250->pitch *= RAD_TO_DEG;
    mpu9250->yaw *= RAD_TO_DEG;

    mpu9250->yaw -= declination;
    mpu9250->roll *= RAD_TO_DEG;

    // Serial.print("Yaw, Pitch, Roll: ");
    // Serial.print(mpu9250->yaw, 2);
    // Serial.print(", ");
    // Serial.print(mpu9250->pitch, 2);
    // Serial.print(", ");
    // Serial.println(mpu9250->roll, 2);

    // Serial.print("rate = ");
    // Serial.print((float)mpu9250->sumCount / mpu9250->sum, 2);
    // Serial.println(" Hz");

    // With these settings the filter is updating at a ~145 Hz rate using the
    // Madgwick scheme and >200 Hz using the Mahony scheme even though the
    // display refreshes at only 2 Hz. The filter update rate is determined
    // mostly by the mathematical steps in the respective algorithms, the
    // processor speed (8 MHz for the 3.3V Pro Mini), and the magnetometer ODR:
    // an ODR of 10 Hz for the magnetometer produce the above rates, maximum
    // magnetometer ODR of 100 Hz produces filter update rates of 36 - 145 and
    // ~38 Hz for the Madgwick and Mahony schemes, respectively. This is
    // presumably because the magnetometer read takes longer than the gyro or
    // accelerometer reads. This filter update rate should be fast enough to
    // maintain accurate platform orientation for stabilization control of a
    // fast-moving robot or quadcopter. Compare to the update rate of 200 Hz
    // produced by the on-board Digital Motion Processor of Invensense's MPU6050
    // 6 DoF and MPU9150 9DoF sensors. The 3.3 V 8 MHz Pro Mini is doing pretty
    // well!

    mpu9250->count = millis();
    mpu9250->sumCount = 0;
    mpu9250->sum = 0;

    mpu9250->count = millis();

    value(0).printf("%.1f,%.1f,%.1f", 
        mpu9250->yaw, mpu9250->pitch, mpu9250->roll);
    value(1).printf("%.1f,%.1f,%.1f", 
        mpu9250->ax, mpu9250->ay, mpu9250->az);
    value(2).printf("%.1f,%.1f,%.1f", 
        mpu9250->mx, mpu9250->my, mpu9250->mz);
    value(3).printf("%.1f", mpu9250->temperature);

    return true;
}
