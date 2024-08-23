// dev_scd4x.cpp
#include "dev_scd4x.h"

DFRobot_SCD4X DFR_SCD4X(&Wire, SCD4X_I2C_ADDR);

Scd4x::Scd4x(const char* name, const float temp_comp, const uint16_t altitude)
    : I2C_Device(name) {
    set_pollrate(5000);  // 5 seconds
    add_subdevice(new Subdevice(F("CO2ppm")));
    add_subdevice(new Subdevice(F("temp")));
    add_subdevice(new Subdevice(F("humidity")));
    set_address(0x62);
    _temp_comp = temp_comp;
    _altitude = altitude;
}

void Scd4x::i2c_start() {
    _in_init_phase = true;
    _started = true;
}

bool Scd4x::measure() {
    if (_in_init_phase) {         // configure the sensor
        if (DFR_SCD4X.begin()) {  // start sensor, return true if success
            DFR_SCD4X.enablePeriodMeasure(
                SCD4X_STOP_PERIODIC_MEASURE);  // stop measuring; 500ms delay
            DFR_SCD4X.setTempComp(
                _temp_comp);  // set temperature compensation w/ offset
            DFR_SCD4X.setSensorAltitude(
                _altitude);  // set altitude, for example, 57m ASL Tartu, EE
            DFR_SCD4X.setAutoCalibMode(true);  // enable auto calibration
            // DFR_SCD4X.persistSettings(); // Save the settings into EEPROM,
            // default to be in RAM; not recommended - initialised every time,
            // avoid wearing out the EEPROM
            DFR_SCD4X.enablePeriodMeasure(
                SCD4X_START_PERIODIC_MEASURE);  // start measuring; 500ms delay
            _in_init_phase = false;
        } else {
            ulog(F("SCD4X: Sensor not found"));
        }
        return false;
    } else {  // normal measure
        if (DFR_SCD4X.getDataReadyStatus()) {
            DFRobot_SCD4X::sSensorMeasurement_t data;
            DFR_SCD4X.readMeasurement(&data);
            value(0).from(data.CO2ppm);    // CO2 in ppm
            value(1).from(data.temp);      // Temperature in deg C
            value(2).from(data.humidity);  // Humidity as %RH
        } else {
            ulog(F("SCD4X: error reading sensor values"));
            return false;
        }
    }
    return true;
}
