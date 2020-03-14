// gyro.cpp
#include <MPU6050_6Axis_MotionApps20.h>
#include "gyro.h"


// call function to keep connection up
void maintain_mqtt(); // TODO: check if calls really necessary

Gyro_MPU6050::Gyro_MPU6050(const char* name, bool calibrate_on_start) :
    I2C_Device(name) {
    _calibrate = calibrate_on_start;
    add_subdevice(new Subdevice(F("ypr"))); // 0
    add_subdevice(new Subdevice(F("acc"))); // 1
    set_address(0x68);
}

void Gyro_MPU6050::i2c_start() {
    mpu6050 = new MPU6050(get_address());
//    mpu6050 = new MPU6050(0x68);

    if(mpu6050) {
        mpu6050->initialize();
        int test;
        for(test=10; test>0; test--) {
            ulog(F("Testing MPU6050 connection..."));
            if(mpu6050->testConnection()) break;
            delay(200);
        }
        if(test<=0) {
            ulog(F("MPU6050 connection failed"));
            return;
        }
        ulog(F("MPU6050 connection successful."));

        // load and configure the DMP
        ulog(F("Initializing DMP..."));
        uint8_t devStatus = mpu6050->dmpInitialize();

        // supply your own gyro offsets here, scaled for min sensitivity
        mpu6050->setXGyroOffset(220);
        mpu6050->setYGyroOffset(76);
        mpu6050->setZGyroOffset(-85);
        mpu6050->setZAccelOffset(1788); // 1688 factory default for my test chip

        // make sure it worked (returns 0 if so)
        if (devStatus == 0) {
            // turn on the DMP, now that it's ready
            mpu6050->setDMPEnabled(true);

            // // enable Arduino interrupt detection
            // Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
            // attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);

            // get expected DMP packet size for later comparison
            packetSize = mpu6050->dmpGetFIFOPacketSize();
        } else {
            // ERROR!
            // 1 = initial memory load failed
            // 2 = DMP configuration updates failed
            // (if it's going to break, usually the code will be 1)
            ulog(F("DMP Initialization failed (code %d)"), devStatus);
            return;
        }

        //if(_calibrate) mpu6050->calcGyroOffsets(true);
        _started = true;
        starttime = millis();
        measure();
    } else {
        ulog(F("Can't reserve memory for mpu6050."));
    }
}


bool Gyro_MPU6050::measure() {
    // get current FIFO count
    uint16_t fifoCount = mpu6050->getFIFOCount(); // this reads from i2c
    maintain_mqtt(); // make sure connection doesn't drop
    // ulog(F("fifocount: %d  packetsize: %d"), fifoCount, packetSize);
    if(fifoCount < packetSize) return false; // nothing ready

    uint8_t mpuIntStatus = mpu6050->getIntStatus(); // this reads from i2c
    maintain_mqtt(); // make sure connection doesn't drop

    // check for overflow (can happen as we poll)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu6050->resetFIFO(); // this writed to i2c
        maintain_mqtt(); // make sure connection doesn't drop
        ulog(F("MPU6050 FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // read a packet from FIFO
        mpu6050->getFIFOBytes(fifoBuffer, packetSize); // read from i2c
        maintain_mqtt(); // make sure connection doesn't drop

        if(!discard_done) {
            // discard all for first 15s
            if(millis() - starttime < 15000) return false;
            discard_done = true;
        }

        // the rest is just math and should be very fast and doesn't need calls to mqtt_maintain

        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        //fifoCount -= packetSize;
        // TODO: shall we iterate?

        //float euler[3];         // [psi, theta, phi]    Euler angle container
        float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
        Quaternion q;           // [w, x, y, z]         quaternion container
        VectorInt16 aa;         // [x, y, z]            accel sensor measurements
        VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
        VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
        VectorFloat gravity;    // [x, y, z]            gravity vector

        // #ifdef OUTPUT_READABLE_QUATERNION
        //     // display quaternion values in easy matrix form: w x y z
        //     mpu6050->dmpGetQuaternion(&q, fifoBuffer);
        //     Serial.print(F("quat\t"));
        //     Serial.print(q.w);
        //     Serial.print(F("\t"));
        //     Serial.print(q.x);
        //     Serial.print(F("\t"));
        //     Serial.print(q.y);
        //     Serial.print(F("\t"));
        //     Serial.println(q.z);
        // #endif

        // #ifdef OUTPUT_READABLE_EULER
        //     // display Euler angles in degrees
        //     mpu6050->dmpGetQuaternion(&q, fifoBuffer);
        //     mpu6050->dmpGetEuler(euler, &q);
        //     Serial.print(F("euler\t"));
        //     Serial.print(euler[0] * 180/M_PI);
        //     Serial.print(F("\t"));
        //     Serial.print(euler[1] * 180/M_PI);
        //     Serial.print(F("\t"));
        //     Serial.println(euler[2] * 180/M_PI);
        // #endif

        mpu6050->dmpGetQuaternion(&q, fifoBuffer);
        mpu6050->dmpGetGravity(&gravity, &q);

        // angles (yaw pitch roll/ypr)
        mpu6050->dmpGetYawPitchRoll(ypr, &q, &gravity);
        value(0).printf("%.1f,%.1f,%.1f",
            ypr[0] * 180/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI);

        // #ifdef OUTPUT_READABLE_REALACCEL
        //     // display real acceleration, adjusted to remove gravity
        //     mpu6050->dmpGetQuaternion(&q, fifoBuffer);
        //     mpu6050->dmpGetAccel(&aa, fifoBuffer);
        //     mpu6050->dmpGetGravity(&gravity, &q);
        //     mpu6050->dmpGetLinearAccel(&aaReal, &aa, &gravity);
        //     Serial.print(F("areal\t"));
        //     Serial.print(aaReal.x);
        //     Serial.print(F("\t"));
        //     Serial.print(aaReal.y);
        //     Serial.print(F("\t"));
        //     Serial.println(aaReal.z);
        // #endif

        // initial world-frame acceleration, adjusted to remove gravity
        // and rotated based on known orientation from quaternion
        mpu6050->dmpGetQuaternion(&q, fifoBuffer);
        mpu6050->dmpGetAccel(&aa, fifoBuffer);
        mpu6050->dmpGetGravity(&gravity, &q);
        mpu6050->dmpGetLinearAccel(&aaReal, &aa, &gravity);
//        value(1).printf("%d,%d,%d", aa.x, aa.y, aa.z);
        mpu6050->dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
        value(1).printf("%d,%d,%d", aaWorld.x/20, aaWorld.y/20, aaWorld.z/20);
    }

    return true;
}
