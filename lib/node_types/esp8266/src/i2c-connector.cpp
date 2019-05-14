// i2c-connector.cpp
#include "i2c-connector.h"

I2c_Connector::I2c_Connector(const char* name, int address) :
    I2C_Device(name) {
    
    add_subdevice(new Subdevice("")); // 0
    add_subdevice(new Subdevice("set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            // TODO: check that correct i2c bus is selected
            Wire.beginTransmission(get_address());
            for( int i=0; i<payload.length(); i++) {
                Wire.write((uint8_t)(payload.as_cstr()[i]));
            }
            Wire.endTransmission();
            return true;
        }
    );
    set_address(address);
}

void I2c_Connector::i2c_start() {
    _started = true;
}

int myread() {
    int value = -1;
    for(int i=0; i<10; i++) {
        if(Wire.available() == 0) {
            delayMicroseconds(10000);
            Serial.print("W");
        } else {
            value = Wire.read();
            break;
        }
    }
    // Serial.print('R');
    // Serial.print(value);
    // Serial.print(' ');
    return value;
}

bool I2c_Connector::measure() {
    int buffer;
    int buf_len;
    uint16_t count;

    if(Wire.requestFrom(get_address(), (size_t) IOTEMPOWER_I2C_CONNECTOR_BUFFER_SIZE, false) == 0) {
        ulog("Reading from i2c not successful. Dev with address %d connected?", get_address());
        return false;
    }

    buffer = myread();
    count = (buffer & 0xff) << 8;

    buffer = myread();
    count += buffer & 0xff;

    buf_len = myread(); // should be length
    ulog("count: %d.", count);
    if(count == 0xffff) {
        ulog("No data after request received.", last_count);
        return false;
    }
    if(count - last_count > 255) {
        last_count =  count;
        ulog("Received far off package with nr: %d", last_count);
        return false; // too much distance
    }
    last_count = count;
    Ustring& v = measured_value();
    v.clear();
    ulog("Trying to read %d chars.", buf_len);
    for(int i=0; i<buf_len; i++) {
        Serial.print("R");
        Serial.print(i);
        Serial.print(" ");
        buffer = myread();
        if(buffer<0) break;
        //v.add((char)buffer); // TODO: a bit inefficient, optimize if necessary
        v.add('x'); // TODO: a bit inefficient, optimize if necessary
    }
    return true;
}
