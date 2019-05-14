// i2c-connector.cpp
#include "i2c-connector.h"

I2c_Connector::I2c_Connector(const char* name, int client_address,
            int master_address) :
    I2C_Device(name, client_address) {
    
    set_master(master_address);

    add_subdevice(new Subdevice("")); // 0
    add_subdevice(new Subdevice("set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            // TODO: check that correct i2c bus is selected
            Wire.beginTransmission(get_address());
            Wire.write('m'); // r request, m message
            for( int i=0; i<payload.length(); i++) {
                Wire.write((uint8_t)(payload.as_cstr()[i]));
            }
            Wire.endTransmission();
            return true;
        }
    );
    set_address(client_address);
}

void I2c_Connector::i2c_start() {
    _started = true;
}

#define I2C_CONNECTOR_RECEIVE_BUFFER_SIZE IOTEMPOWER_I2C_CONNECTOR_BUFFER_SIZE
static char i2c_connector_receive_buffer[I2C_CONNECTOR_RECEIVE_BUFFER_SIZE];
static int i2c_connector_receive_buffer_fill; 
// TODO: this is a singleton, doesn't matter if nothing runs in parallel
void i2c_connector_receive_service(size_t count) {
    int v;

    for(int i=0; i<count; i++) {
        v = Wire.read();
        if(v<0) break;
        if(i2c_connector_receive_buffer_fill >= I2C_CONNECTOR_RECEIVE_BUFFER_SIZE) {
            break;
        }
        i2c_connector_receive_buffer[i2c_connector_receive_buffer_fill] = (char)v;
        i2c_connector_receive_buffer_fill++;
    }
    Wire.flush();
    ulog("Received %d chars.",(int)count);
}


bool I2c_Connector::measure() {

    // seems not to work in new firmware
    // if(Wire.requestFrom(get_address(), (size_t) IOTEMPOWER_I2C_CONNECTOR_BUFFER_SIZE, false) == 0) {
    //     ulog("Reading from i2c not successful. Dev with address %d connected?", get_address());
    //     return false;
    // }

    // Send request first.
    Wire.beginTransmission(get_address());
    Wire.write('r'); // r request, m message
    Wire.write(get_master()); // return address
    Wire.endTransmission();
    
    // Wait for an answer - TODO: check if something else than delay can be used here
    delay(10);

    // Don't accept any more data
    Wire.onReceive(NULL);

    if(i2c_connector_receive_buffer_fill < 3) {
        ulog("No answer received");
        return false; // not even header
    }

    uint16_t count = (((uint8_t)i2c_connector_receive_buffer[0]) & 0xff) << 8;
    count += ((uint8_t)i2c_connector_receive_buffer[1]) & 0xff;

    uint16_t buf_len = i2c_connector_receive_buffer[2]; // should be length
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
    for(int i=3; i<i2c_connector_receive_buffer_fill; i++) {
        //v.add(i2c_connector_receive_buffer[i]); // TODO: a bit inefficient, optimize if necessary
        v.add('x'); // TODO: just for debug
    }
    return true;
}
