/* setup.cpp
 * This is the configuration code for a IoTempower node.
 * Here, you define all the devices (and eventually their interactions)
 * connected to the node specified with this directory.
 * If you want to see more device configuration examples,
 * check $IOTEMPOWER_ROOT/examples/running-node-test-setup.cpp
 *
 * Or check out the command reference for potential devices you can add.
 * 
 * This whole comment block can be deleted
 * */
void iotempower_init() {
    eeprom_init();
}

// Simple button usage from quickstart
input(b1,D3, "up", "down"); //D3 for esp8266MOD and 18 for S2,  17 for esp32

void iotempower_start() {
    // We create a static variable to remember the last physical button state
    static String last_physical_state = "";

    IN(b1).filter_callback(*new Callback([](Device& dev) {
        
        String val_str = dev.value().as_cstr();
        
        if (val_str == last_physical_state) {
            return false; 
        }
        
        last_physical_state = val_str;
        
        String encrypted_hex = aes_publish_secure(val_str);
        dev.value().from(encrypted_hex.c_str());
        
        ulog("AES Encrypted message generated!");
        
        return true; 
    }));
}