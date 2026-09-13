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

// Simple LED node
rgb_single(rgb1, D4, D3, D2);

void iotempower_start() {
    
    IN(rgb1).subdevice(1)->with_receive_cb([&](const Ustring& encrypted_payload) {
        String decrypted = aes_receive_secure(encrypted_payload.as_cstr());
        
        if (decrypted == "on") {
            ulog("AES accepted: turning ON");
            IN(rgb1).on();
            return true;
        } 
        else if (decrypted == "off") {
            ulog("AES accepted: turning OFF");
            IN(rgb1).off();
            return true;
        }
        
        ulog("AES On/Off command rejected or invalid.");
        return false; 
    });

    IN(rgb1).subdevice(5)->with_receive_cb([&](const Ustring& encrypted_payload) {
        String decrypted = aes_receive_secure(encrypted_payload.as_cstr());
        
        if (decrypted != "") {
            ulog("AES Color command accepted: %s", decrypted.c_str());
            
            Ustring u_decrypted(decrypted.c_str());
            
            IN(rgb1).set_colorstr(u_decrypted);
            return true;
        }
        
        ulog("AES Color command rejected.");
        return false;
    });
}