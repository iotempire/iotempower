// i2c-socket.h
// Header File for creating an IoTempower i2c connector socket
// (for example to connect to an Arduino via i2c using the respective
// Arduino library on the Arduino's end)

#ifndef _IOTEMPOWER_I2CCONNECTOR_H_
#define _IOTEMPOWER_I2CCONNECTOR_H_

#include <i2c-device.h>

#define IOTEMPOWER_I2C_CONNECTOR_DEFAULT_ADDRESS 9

// buffer_size=35:counter (2) + size (1) + data (32)
#define IOTEMPOWER_I2C_CONNECTOR_BUFFER_SIZE 35  

class I2C_Socket : public I2C_Device {
    private:
        uint16_t last_count = 0xffff;
    public:
        I2C_Socket(const char* name,
            int client_address = IOTEMPOWER_I2C_CONNECTOR_DEFAULT_ADDRESS,
            int master_address = IOTEMPOWER_DEFAULT_I2C_MASTER_ADDRESS);
        void i2c_start();
        bool measure();
};

#endif // _IOTEMPOWER_I2CCONNECTOR_H_