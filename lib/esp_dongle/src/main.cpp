// firmware for ulnoiot ESP dongle (UED)
//
// author: ulno
// created: 2019-02-22

#define BUFFER_LEN 1460
#define DISPLAY_BUFFER_LEN 128
#define SERVER_PORT 28266
#define RXBUFFER_SIZE 1024
#define SMALL_STRING_LEN 33

#define FLASH 0
#define SPIFFS 100
#define AUTH 200

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <functional>

// used external hardware
#include <Wire.h>
#include <U8g2lib.h>

// config defaults
#include "ulnoiot-default.h"

// simple command line TODO: would it be better to use bitlash or hobbitlash?
#include <SimpleCLI.h>
using namespace simplecli;
// create an instance of it
SimpleCLI* cli;

char buffer[BUFFER_LEN];
int buffer_fill = 0;

char gw_ssid[SMALL_STRING_LEN+1]={0};
char gw_uptime[SMALL_STRING_LEN+1]={0};
char gw_mem_free[SMALL_STRING_LEN+1]={0};
char gw_load[SMALL_STRING_LEN+1]={0};

// small display shield for Wemos
U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0); // R0 no rotation, R1 - 90°
bool display_present = false;

// The following is taken from: http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html
// Matthew Ford 1st August 2017 (original 28th September 2014)
// (c) Forward Computing and Control Pty. Ltd. NSW Australia
// https://github.com/esp8266/Arduino/issues/1025#issuecomment-319199959
int i2c_clear_bus() {
#if defined(TWCR) && defined(TWEN)
    TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the sda_pin and scl_pin pins directly
#endif

    const uint8_t sda_pin = SDA;
    const uint8_t scl_pin = SCL;

    pinMode(sda_pin, INPUT_PULLUP); // Make sda_pin (data) and scl_pin (clock) pins Inputs with pullup.
    pinMode(scl_pin, INPUT_PULLUP);

    delay(250); // assert master

    boolean SCL_LOW = (digitalRead(scl_pin) == LOW); // Check is scl_pin is Low.
    if (SCL_LOW) { //If it is held low Arduino cannot become the I2C master. 
        return 1; //I2C bus error. Could not clear scl_pin clock line held low
    }

    boolean SDA_LOW = (digitalRead(sda_pin) == LOW);  // vi. Check sda_pin input.
    int clockCount = 20; // > 2x9 clock

    while (SDA_LOW && (clockCount > 0)) { //  vii. If sda_pin is Low,
        clockCount--;
        // Note: I2C bus is open collector so do NOT drive scl_pin or sda_pin high.
        pinMode(scl_pin, INPUT); // release scl_pin pullup so that when made output it will be LOW
        pinMode(scl_pin, OUTPUT); // then clock scl_pin Low
        delayMicroseconds(10); //  for >5uS
        pinMode(scl_pin, INPUT); // release scl_pin LOW
        pinMode(scl_pin, INPUT_PULLUP); // turn on pullup resistors again
        // do not force high as slave may be holding it low for clock stretching.
        delayMicroseconds(10); //  for >5uS
        // The >5uS is so that even the slowest I2C devices are handled.
        SCL_LOW = (digitalRead(scl_pin) == LOW); // Check if scl_pin is Low.
        int counter = 20;
        while (SCL_LOW && (counter > 0)) {  //  loop waiting for scl_pin to become High only wait 2sec.
            counter--;
            delay(100);
            SCL_LOW = (digitalRead(scl_pin) == LOW);
        }
        if (SCL_LOW) { // still low after 2 sec error
            return 2; // I2C bus error. Could not clear. scl_pin clock line held low by slave clock stretch for >2sec
        }
        SDA_LOW = (digitalRead(sda_pin) == LOW); //   and check sda_pin input again and loop
    }
    if (SDA_LOW) { // still low
        return 3; // I2C bus error. Could not clear. sda_pin data line held low
    }

    // else pull sda_pin line low for Start or Repeated Start
    pinMode(sda_pin, INPUT); // remove pullup.
    pinMode(sda_pin, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
    // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
    /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
    delayMicroseconds(10); // wait >5uS
    pinMode(sda_pin, INPUT); // remove output low
    pinMode(sda_pin, INPUT_PULLUP); // and make sda_pin high i.e. send I2C STOP control.
    delayMicroseconds(10); // x. wait >5uS
    pinMode(sda_pin, INPUT); // and reset pins as tri-state inputs which is the default state on reset
    pinMode(scl_pin, INPUT);
    return 0; // all ok
}

//std::function<void()> display_func = NULL;

void display(std::function<void()> display_func) {
    if(!display_present) return;
    u8g2.firstPage();
    do {
        yield();
        if(display_func) display_func();
    } while ( u8g2.nextPage() );
}

void display_init() {
    if(!display_present) return;
    display( [&] {
        u8g2.setFont(u8g2_font_7x14B_tr);
        u8g2.drawStr(7, 18, "UlnoIoT");
        u8g2.drawStr(10, 39, "Dongle");
    });
}

int current_menu = -1;

void display_text(const char* text) {
    if(!display_present) return;
    current_menu = -1; // clear menu shown
//    u8g2.setFont(u8g2_font_profont12_mf);
    u8g2.setFont(u8g2_font_5x8_mf);
    int8_t char_height = u8g2.getMaxCharHeight();
    int8_t char_width = u8g2.getMaxCharWidth();
    char charstr[2]=" ";
    u8g2.firstPage();
    do {
        //yield();
        int x = 0;
        int y = char_height-1;
        while(*text) {
            if(text[0]=='\\' && text[1]=='n') {
                text++;
                x=0;
                y+=char_height;
            } else if(text[0]=='\n') {
                x=0;
                y+=char_height;
            } else {
                charstr[0] = text[0];
                u8g2.drawStr(x, y, charstr);
                x+=char_width;
            }
            text++;
        }
    } while ( u8g2.nextPage() );   
}


void display_text(const String& text) {
    if(!display_present) return;
    char display_buffer[128];
    display_buffer[127] = 0;
    text.toCharArray(display_buffer, 127);
    display_text(display_buffer);
}


void display_ulnoiot() {
    if(!display_present) return;
    String display_string = "  UlnoIoT\n  Dongle\n\n";
    String ssid = String(gw_ssid);
    int l = (int)ssid.length();

    if(gw_ssid[0]) {
        display_text(display_string 
            + "\nWiFi: " + ssid.substring(0,max(8,l-13)) 
            + "\n" + ssid.substring(max(0,l-13)));
    } else {
        display_text( display_string + "No gateway\ndetected.");
    }
}


void display_gw_info() {
    if(!display_present) return;
    String display_string = "UlnoIoTDongle\n";
    String ssid = String(gw_ssid);
    int l = (int)ssid.length();

    if(gw_ssid[0]) {
        display_text(display_string 
            + "MFr: " + String(gw_mem_free)
            + "\nUP: " + String(gw_uptime) 
            + "\nLD: " + String(gw_load)
            + "\nWiFi: " + ssid.substring(0,max(8,l-13)) 
            + "\n" + ssid.substring(max(0,l-13)));
    } else {
        display_text(display_string 
            + "\nNo gateway\ndetected.\nMake sure it\nis running.");
    }
}


void display_esp_info() {
    if(!display_present) return;
    String display_string = "Internal Info\n";

    display_text(display_string 
        + "\nMFM: " + String(ESP.getFreeHeap())
        + "\nMFB: " + String(ESP.getMaxFreeBlockSize())
        + "\nID: " + String(ESP.getChipId(), HEX)
        );
}


int display_menu(int current_menu, int menu_pos, int menu_advance) {
    if(!display_present || current_menu<0) return 0;

    int menu_length=0;

    menu_pos += menu_advance;
    if(menu_pos<=0) menu_pos=0;
    
    switch(current_menu) {
        case 0: // main menu
            menu_length = 3;
            if(menu_pos>=menu_length) menu_pos=0;
            display_text(String("*Dongle Menu*\n") 
                + (menu_pos==0?">":" ") + "Show info\n"
                + (menu_pos==1?">":" ") + "Show intern\n"
                + (menu_pos==2?">":" ") + "Shutdown gw\n"
                + "<down select>");
            break;
        case 1: // shut_down menu
            menu_length = 3;
            if(menu_pos>=menu_length) menu_pos=0;
            display_text(String("Shutdown gw?\n") 
                + (menu_pos==0?">":" ") + "Cancel, back\n"
                + (menu_pos==1?">":" ") + "No\n"
                + (menu_pos==2?">":" ") + "Shutdown gw!\n"
                + "<down select>");
            break;
        default:
            menu_pos = 0;
            break;
    }

    return menu_pos;
}

void prompt() {
    Serial.print("UED>"); // ulnoiot esp dongle
    Serial.flush();
}

bool ota_serve(int firmware_size, const char* firmware_md5) {

    // TODO: change quickly to something web-based and at least sha256 auth

    // based on espota of ESP8266 for Arduino
    WiFiServer wifi_server(SERVER_PORT);
    WiFiUDP invite_client;
    const int send_buffer_len = 128;
    char send_buffer[send_buffer_len];
    char display_string[128];

    // Start local server listening for image request
    wifi_server.begin();
    Serial.print("Listening for answers on UDP port: ");
    Serial.println(SERVER_PORT+1);
    invite_client.begin(SERVER_PORT+1);

    // Sending invitation
    Serial.println("Sending invitation for firmware OTA to node.");
    snprintf(send_buffer, 128, "%d %d %d %s\n", 
        FLASH, SERVER_PORT, firmware_size, firmware_md5);
    Serial.print("Sending as invite: ");
    Serial.println(send_buffer);
    const IPAddress invite_client_ip(192,168,4,1);
    // debug const IPAddress invite_client_ip(192,168,12,49);
    invite_client.beginPacket(invite_client_ip, 8266);
    invite_client.write(send_buffer, strnlen(send_buffer, send_buffer_len));
    if(!invite_client.endPacket()) {
        Serial.println("!error sending invite package");
    }
    uint16_t local_port =  invite_client.localPort();
    Serial.print("Local port:");
    Serial.println(local_port);
    Serial.println("Waiting for answer from node.");
    int buf_size = 0;
    for(int i=0; i<1000; i++) { // wait for up to 10s
        buf_size = invite_client.parsePacket();
        if(buf_size>0) break;
        delay(10);
    }
    if(buf_size == 0) {
        Serial.println("!error no answer to invite");
        invite_client.stop();
        wifi_server.stop();
        return false;
    }
    invite_client.read(buffer, buf_size);
    if (buf_size >= BUFFER_LEN) buf_size = BUFFER_LEN-1;
    buffer[buf_size] = 0; // 0 terminate

    Serial.print("answer: ");
    Serial.println(buffer);
    if(strcmp(buffer,"OK")) { // answer is not OK
        if(memcmp(buffer, "AUTH ", 5)) { // does not start with AUTH (or OK)
            Serial.println("!error wrong answer to invite");
            invite_client.stop();
            wifi_server.stop();
            return false;
        }

        char *nonce = buffer + 5; // hexdigest nonce comes directly after AUTH
        
        // compute cnonce
        char cnonce_text[128];
        snprintf(cnonce_text, 128, "firmware.bin%u%s192.168.4.1", firmware_size, firmware_md5);
        // Serial.print("cnonce text: ");
        // Serial.println(cnonce_text);
        char cnonce[33];
        MD5Builder cnonce_md5; 
        cnonce_md5.begin();
        cnonce_md5.add(cnonce_text); 
        cnonce_md5.calculate(); 
        cnonce_md5.getChars(cnonce);
        
        // compute passmd5
        char passmd5[33];
        MD5Builder passmd5_md5;
        passmd5_md5.begin();
        passmd5_md5.add(ULNOIOT_FLASH_DEFAULT_PASSWORD); 
        passmd5_md5.calculate(); 
        passmd5_md5.getChars(passmd5);
        // Serial.print("passmd5: ");
        // Serial.println(passmd5);
        
        // compute result
        char result_text[128];
        snprintf(result_text, send_buffer_len, "%s:%s:%s", passmd5, nonce, cnonce );
        // Serial.print("result text: ");
        // Serial.println(result_text);

        char result[33];
        MD5Builder result_md5;
        result_md5.begin();
        result_md5.add(result_text); 
        result_md5.calculate(); 
        result_md5.getChars(result);

        // Do authentication
        snprintf(send_buffer, send_buffer_len, "%d %s %s\n", AUTH, cnonce, result);
        // Serial.print("Sending: ");
        // Serial.println(send_buffer);
        invite_client.beginPacket(invite_client_ip, 8266);
        invite_client.write(send_buffer, strnlen(send_buffer, send_buffer_len));
        if(!invite_client.endPacket()) {
            Serial.println("!error sending auth package");
            invite_client.stop();
            wifi_server.stop();
            return false;
        }

        // wait for AUTH answer
        for(int i=0; i<1000; i++) { // wait for up to 10s
            buf_size = invite_client.parsePacket();
            if(buf_size>0) break;
            delay(10);
        }
        if(buf_size == 0) {
            Serial.println("!error no answer to auth answer");
            invite_client.stop();
            wifi_server.stop();
            return false;
        }
        invite_client.read(buffer, buf_size);
        if (buf_size >= BUFFER_LEN) buf_size = BUFFER_LEN-1;
        buffer[buf_size] = 0; // 0 terminate
        Serial.print("Auth answer: ");
        Serial.println(buffer);

        if(strcmp(buffer,"OK")) { // answer is not OK
            Serial.println("!error receiving positive auth confirmation");
            invite_client.stop();
            wifi_server.stop();
            return false;
        }
    }

    // Finally done with UDP
    invite_client.stop();

    // answer is OK or we have successfully authenticated
    Serial.println("Waiting 10s for node to open connection.");

    WiFiClient client;
    // wait for node connection
    for(int i=0; i<1000; i++) { // wait for up to 10s
        client = wifi_server.available();
        if(client) break;
        delay(10);
    }
    if(!client) {
        Serial.println("!error no response from node");
        wifi_server.stop();
        return false;
    }
    
    Serial.println("!upload start");

    buffer_fill = 0;
    int bytes_sent = 0;
    int bytes_read = 0;
    char next_char;
    bool something_received;
    bool ok_found = false;

    display_text(" *Adoption*\n\nSending\n0%");
    unsigned long last_action = millis();
    unsigned long current;
    while(bytes_sent < firmware_size) {
        //yield();
        current = millis();
        if(current - last_action > 10000) {
            Serial.print("!error not receiving enough firmware data. bytes_read: ");
            Serial.print(bytes_read);
            Serial.print(" bytes_sent: ");
            Serial.println(bytes_sent);
            wifi_server.stop();
            return false;
        }
        if(bytes_read < firmware_size) { // if not read all, read more
            if(buffer_fill < BUFFER_LEN) { // but only if still space in buffer
                if(Serial.available() > 0) {
                    next_char = Serial.read();
                    last_action = current;
                    if(next_char >=0) {
                        buffer[buffer_fill] = (unsigned char)next_char;
                        buffer_fill ++;
                        bytes_read ++;
                        if(bytes_read < firmware_size && (bytes_read % RXBUFFER_SIZE) == 0) {
                            Serial.println("!send more");
                            Serial.flush();
                        }
                    }
                }
            }
        }
        // time to send it out
        if(buffer_fill == BUFFER_LEN || bytes_read == firmware_size) {
            Serial.print("Block: ");
            Serial.println(bytes_sent);
            Serial.flush();
            // trigger send
            if(!client.write(buffer, buffer_fill)) {
                Serial.println("!error problems sending firmware data");
                wifi_server.stop();
                return false;
            }
            //yield(); 
            // delay(1); // let things finish - delay crashes serial???
            last_action = current;
            bytes_sent += buffer_fill;
            snprintf(display_string, 128, "*Adoption*\n\nSending\n%d%%",
                (int)(bytes_sent*100/firmware_size));
            display_text(display_string);
            // try to receive something for max 10s
            something_received = false;
            unsigned long start = millis(); 
            while(true) {
                if(millis() - start > 10000) break;
                //Serial.flush()
                //optimistic_yield(100);
                int available = client.available();
                if(available>=0) {
                    something_received = true;
                    client.read((uint8_t*)buffer, available);
                    for(int j=0; j<available; j++) {
                        if(buffer[j] == 'O') {
                            ok_found = true;
                            break;
                        } 
                    }
                    break;
                }
            }
            if(!something_received) {
                Serial.println("!error no confirmation data");
                Serial.flush();
                wifi_server.stop();
                return false;
            }
            buffer_fill = 0;
        }
    } // end (bytes_sent < firmware_size)
    something_received = false;
    if(!ok_found) { // try for 15s longer to get the final ok
        unsigned long start=millis();
        while(true) {
            if(millis()-start > 15000) break;
            int available = client.available();
            if(available>0) {
                something_received = true;
                client.read((uint8_t*)buffer, available);
                for(int j=0; j<available; j++) {
                    if(buffer[j] == 'O') {
                        ok_found = true;
                        break;
                    } 
                }
                if(ok_found) break;
            }
        }
    }
    if(!ok_found) {
        Serial.printf("!error no final confirmation: received %d", something_received);
        Serial.flush();
        wifi_server.stop();
        return false;
    } else {
        Serial.println("!success uploaded successfully");
        Serial.flush();
    }
    wifi_server.stop();
    return true; // success
}


void adopt(Cmd* cmd) {
    const int ssid_maxlen = 32;
    char node_network[ssid_maxlen+1];
    char md5sum[33];
    md5sum[32]=0;
    const char *node_ap_password = ULNOIOT_AP_RECONFIG_PASSWORD;
// debug   const char *node_ap_password = "mypassword";

    node_network[ssid_maxlen] = 0 ; // terminate
    cmd->getArg(0)->getValue().toCharArray(node_network, ssid_maxlen);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.printf("Connecting to reconfig node network %s.", node_network);
    WiFi.begin(node_network, node_ap_password);

    display_text(" *Adoption*\n\nTrying to\nconnect.");
    // Try for 15 seconds
    for(int i=0; i<15; i++) {
        delay(1000);
        if(WiFi.status() == WL_CONNECTED) break;
        Serial.print(".");
    }
    if(WiFi.status() != WL_CONNECTED) {
        display_text(" *Adoption*\n\nConnect\nfailed.");
        Serial.println("failure.");
        Serial.println("!error adopt connect");
        return;
    };
    Serial.println("OK.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    // when successfully connected start OTA handshake
    cmd->getArg(2)->getValue().toCharArray(md5sum, 33);
    bool result = ota_serve(cmd->getArg(1)->getValue().toInt(), md5sum);
    if(!result) {
        display_text(" *Adoption*\n\nFailure");
    } else {
        display_text(" *Adoption*\n\nSuccess");
    }
};


void daemon_info(Cmd* cmd) {
    cmd->getArg(0)->getValue().toCharArray(gw_ssid, SMALL_STRING_LEN);
    gw_ssid[SMALL_STRING_LEN] = 0;
    cmd->getArg(1)->getValue().toCharArray(gw_uptime, SMALL_STRING_LEN);
    gw_uptime[SMALL_STRING_LEN] = 0;
    cmd->getArg(2)->getValue().toCharArray(gw_mem_free, SMALL_STRING_LEN);
    gw_mem_free[SMALL_STRING_LEN] = 0;
    cmd->getArg(3)->getValue().toCharArray(gw_load, SMALL_STRING_LEN);
    gw_load[SMALL_STRING_LEN] = 0;
}


void setup_cli() {
    cli = new SimpleCLI();

    // when no valid command could be found for given user input
    cli->onNotFound = [](String str) {
                          Serial.println("\"" + str + "\" command not found, try help.");
                      };


    //// help
    cli->addCmd(new Command("help", [](Cmd* cmd) {
        Serial.println(
            "You can use the follwing commands:\n"
            "- help: shows this help\n"
            "- display TEXT: show TEXT on the display\n"
            "- scan: scan wifi networks\n"
            "- adopt NODE SIZE MD5: adopt NODE and send initial firmware,\n"
            "    SIZE and MD5 are part of following espota protocal\n"
            "- reset: resets the dongle\n"
            "- $info ssid uptime mem_free load: resets the dongle\n");
    }));

    //// help
    cli->addCmd(new Command("reset", [](Cmd* cmd) {
        Serial.println("Reset in 2s.\n");
        delay(2000);
        ESP.restart();
    }));

    //// display
    Command* display_cmd = new Command("display", [](Cmd* cmd) {
        Serial.print("Display got: ");
        Serial.println(cmd->getArg(0)->getValue());
        display_text(cmd->getArg(0)->getValue());
    });
    display_cmd->addArg(new AnonymOptArg()); // text
    cli->addCmd(display_cmd);

    //// scan
    cli->addCmd(new Command("scan", [](Cmd* cmd) {
        // Set WiFi to station mode and disconnect from an AP if it was previously connected
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();

        display_text("WiFi Scan\n\nScanning");

        Serial.println("!network_scan start");
        int count = WiFi.scanNetworks();
        for (int i = 0; i < count; ++i) {
            Serial.print(WiFi.RSSI(i));
            Serial.print(" ");
            Serial.println(WiFi.SSID(i));
        }
        Serial.println("!network_scan end");
        Serial.flush();
        display_text("WiFi Scan\n\nDone.");
    }));

    //// adopt
    Command* adopt_cmd = new Command("adopt", adopt);
    adopt_cmd->addArg(new AnonymOptArg()); // node
    adopt_cmd->addArg(new AnonymOptArg()); // size
    adopt_cmd->addArg(new AnonymOptArg()); // md5
    cli->addCmd(adopt_cmd);

    // dongle daemon responses
    Command* info_cmd = new Command("$info", daemon_info);
    info_cmd->addArg(new AnonymOptArg()); // ap-name/ssid
    info_cmd->addArg(new AnonymOptArg()); // gw uptime
    info_cmd->addArg(new AnonymOptArg()); // mem free
    info_cmd->addArg(new AnonymOptArg()); // load
    cli->addCmd(info_cmd);


    // // TODO: remove =========== Add ping command =========== //
    // // ping                 => pong
    // // ping -s ponk         => ponk
    // // ping -s ponk -n 2    => ponkponk
    // Command* ping = new Command("ping", [](Cmd* cmd) {
    //     int h = cmd->getValue("n").toInt();

    //     for (int i = 0; i < h; i++) {
    //         Serial.print(cmd->getValue("s"));
    //     }
    // });
    // ping->addArg(new OptArg("s", "ping!"));
    // ping->addArg(new OptArg("n", "1"));
    // cli->addCmd(ping);
    // // ======================================== //


    prompt();
}


void setup() {
    Serial.setRxBufferSize(RXBUFFER_SIZE+256); // HW-Buffer seems to be only 256 Bytes (TODO: recheck best value)
    // let's be "fast", should work on a Wemos D1 mini
 //    Serial.begin(115200); 
//    Serial.begin(921600);
    Serial.begin(2000000);  // let's be "fast", should work on a Wemos D1 mini


    // flush some garbage
    for( int i=0; i<20; i++) {
      Serial.print(".");
      delay(50);
    }
    Serial.println();
    Serial.println();
    // print init message
    Serial.println("Starting UlnoIoT Dongle.");
    Serial.println();
    // initially no WiFi switched on, TODO: later be in ESP-Now mode and initialize LORA
    WiFi.softAPdisconnect (true);
    WiFi.disconnect(true);

    i2c_clear_bus();
    // probe image shield
    Wire.begin(); // check default i2c
    Wire.beginTransmission(0x3c);
    if (Wire.endTransmission() == 0) {
        Serial.println("Display shield found.");
        display_present = true;
        u8g2.begin();
        display_init();
        pinMode(D4, INPUT); // only floating as else potential trouble with onboard-led
        pinMode(D3, INPUT_PULLUP);
    } else {
        Serial.println("No display shield found.");
        // TODO: add some onboard-led blink patterns
    }

    setup_cli();
}

int read_buttons() {
    const int debounce = 8;
    static int b1_count = 0;
    static int b2_count = 0;
    int state = 0;

    pinMode(D4, INPUT); // only floating as else potential trouble with onboard-led

    if(!digitalRead(D3)) {
        b1_count ++;
        if(b1_count > debounce*2) b1_count = debounce*2;
    }
    else {
        b1_count--;
        if(b1_count < 0) b1_count = 0;
    }
    if(b1_count >= debounce) state |= 1;

    if(!digitalRead(D4)) {
        b2_count ++;
        if(b2_count > debounce*2) b2_count = debounce*2;
    }
    else {
        b2_count--;
        if(b2_count < 0) b2_count = 0;
    }
    if(b2_count >= debounce) state |= 2;

    return state;
}


void loop() {
    static unsigned long last_action = millis();
    static unsigned long last_button = last_action;
    static unsigned long last_gw_action = last_action;
    static int current_info_screen = 0;
    int next_char;

    if(Serial.available() > 0) {
        next_char= Serial.read();
        if(next_char > 0) {
            Serial.write(next_char);
            Serial.flush();
            buffer[buffer_fill] = (unsigned char)next_char;
            if(next_char == '\n' || buffer_fill == BUFFER_LEN - 1) {
                if(next_char != '\n') {
                    buffer_fill ++;
                    Serial.println();
                }
                buffer[buffer_fill] = 0;
                cli->parse(buffer);
                buffer_fill = 0;
                prompt();
                last_action = millis();
                last_gw_action = millis();
            } else {
                buffer_fill ++;
            }
        }
    }

    static int last_state=0;
    static int menu_pos=0;
    static int current_menu = -1;
    if(display_present && millis() - last_button > 20) { // 50 times per second do button reading
        int state = read_buttons();
        if(state != last_state) {
            last_state = state;
            if(state) { // only trigger with press, not with release
                if(current_menu >= 0) {
                    if(state & 1) { // left pressed
                        menu_pos = display_menu(current_menu, menu_pos, 1);
                        last_action = millis();
                    } else if(state & 2) { // right pressed
                        Serial.print("Selected in dongle menu: ");
                        Serial.print(current_menu);
                        Serial.print(" at pos: ");
                        Serial.println(menu_pos);
                        switch(current_menu) {
                            case 0: // default user menu:
                                switch(menu_pos) {
                                    case 0: // show info
                                        current_menu = -1;
                                        current_info_screen = 0;
                                        display_gw_info();
                                        break;
                                    case 1: // show esp info
                                        current_menu = -1;
                                        display_esp_info();
                                        break;
                                    case 2: // shutdown
                                        current_menu = 1;
                                        menu_pos = 0;
                                        display_menu(current_menu, menu_pos, 0);
                                        break;
                                }
                                last_action = millis();
                                break;
                            case 1: // shutdown menu
                                switch(menu_pos) {
                                    case 0: // back
                                        current_menu = 0;
                                        menu_pos = 2;
                                        display_menu(current_menu, menu_pos, 0);
                                        break;
                                    case 1: // no
                                        current_menu = -1;
                                        current_info_screen = 0;
                                        display_gw_info();
                                        break;
                                    case 2: // shutdown
                                        Serial.println("");
                                        Serial.println("!shutdown");
                                        current_menu = -1;
                                        display_text("Shutting\ndown\ngateway...");
                                        break;
                                }
                                last_action = millis();
                                break;
                        }
                    }
                } else { // not shown, so show menu
                    current_menu = 0;
                    menu_pos = 0;
                    display_menu(current_menu, menu_pos, 0);
                    last_action = millis();
                }
            }
        }
    }

    if(millis() - last_action > 15000) { // every 15 s do status update
        Serial.write("!daemon_check\n");
        current_menu = -1;
        // answer received above
        current_info_screen ++;
        current_info_screen %= 2;
        switch(current_info_screen) {
            case 0: // info menu
                display_gw_info();
                break;
            case 1:
                display_ulnoiot();
                break;
        }
        last_action = millis();
    }

    if(millis() - last_gw_action > 31000) {
        if(gw_ssid[0] != 0) { // only show, if it was set before
            // expire information from pi
            gw_ssid[SMALL_STRING_LEN] = 0;
            gw_uptime[SMALL_STRING_LEN] = 0;
            gw_mem_free[SMALL_STRING_LEN] = 0;
            gw_load[SMALL_STRING_LEN] = 0;
            if(current_menu >= 0)
                display_text("  UlnoIoT\n  Dongle\n\nConnection to\ngateway lost.");
        }
        last_gw_action = millis(); // prevent triggering display all the time
    }
}