// main.cpp
// main program for iotempower esp8266 firmware

////// Standard libraries
#include <ArduinoOTA.h>
//#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#ifdef ESP32
    #include <WiFi.h>
    #include <ESPmDNS.h>
#else
    #include <ESP8266WiFi.h>
    #include <ESP8266mDNS.h>
#endif
#include <FS.h>
#include <WiFiUdp.h>

////// adapted libraries
#include <Ticker.h>
//#include <WiFiManager.h>

// PubSubClient.h and ArduinoMqtt unstable
// TODO: recheck PubSubClient
#include <AsyncMqttClient.h>

// helpers
#include <device-manager.h>
#include <toolbox.h>

//// default configuration
#include <iotempower-default.h>

// TODO: move this in own file to shorten compilation time
// node specific code
#include "config.h"
#include "key.h"//// configuration

#include "pins-wrapper.h"
#include "wifi-config.h"

// iotempower functions for user modification in setup.cpp
void (ulnoiot_init)() __attribute__((weak));
void (ulnoiot_start)() __attribute__((weak));

int long_blinks = 0, short_blinks = 0;

bool wifi_connected = false;
bool ota_failed = false;

// TODO: find better solution (add display support?)
#ifndef ONBOARDLED
    #pragma "No ONBOARDLED is defined for this board, there will be no indicator led" warning
#else
    #define ID_LED ONBOARDLED
#endif
#define BLINK_OFF_START 2000
#define BLINK_LONG 800
#define BLINK_OFF_MID 800
#define BLINK_SHORT 200
#define BLINK_OFF 500

void id_blinker() {
    static bool init_just_done = false;
    static int total, global_pos;
    static unsigned long lasttime;
    unsigned long currenttime;
    int delta;

    if (long_blinks == 0) { // first time, still unitialized
        // randomness for 30 different blink patterns (5*6)
        long_blinks = ESPTrueRandom.random(1, 6);
        short_blinks = ESPTrueRandom.random(1, 7);
        Serial.printf("Blink pattern: %d long_blinks, %d short_blinks\n",
                      long_blinks, short_blinks);
        total = BLINK_OFF_START +
                long_blinks * (BLINK_LONG + BLINK_OFF) +
                BLINK_OFF_MID +
                short_blinks * (BLINK_SHORT + BLINK_OFF);
        #ifdef ID_LED
        pinMode(ID_LED, OUTPUT);
        digitalWrite(ID_LED, 0); // on - start with a long blink
        #endif // ID_LED
        init_just_done = true;
        return; // finish here first time
    }
    if(init_just_done) { //second time go here
        lasttime = millis();
        global_pos = 0;
        init_just_done = false;
    }
    // compute where in blink pattern we currently are
    currenttime = millis();
    delta = currenttime - lasttime;
    global_pos = (global_pos + delta) % total;
    #ifdef ID_LED
    int pos = global_pos;
    if (pos < BLINK_OFF_START) { // still in BLINK_OFF_START
        pinMode(ID_LED, INPUT); // floating (as onboard led)
        //digitalWrite(ID_LED, 1); // off - in off phase
    } else { // already pass BLINK_OFF_START
        pos -= BLINK_OFF_START;
        if(pos < long_blinks * (BLINK_LONG + BLINK_OFF)) { // in long blink phase

            if (pos % (BLINK_LONG + BLINK_OFF) < BLINK_LONG) {
                pinMode(ID_LED, OUTPUT);
                digitalWrite(ID_LED, 0); // on - in blink phase
            } else {
                pinMode(ID_LED, INPUT); // floating (as onboard led)
                // digitalWrite(ID_LED, 1); // off - in off phase
            }

        } else { // in short blink phase
            pos -= long_blinks * (BLINK_LONG + BLINK_OFF);
            if(pos < BLINK_OFF_MID) { // still in BLINK_OFF_MID
                pinMode(ID_LED, INPUT); // floating (as onboard led)
                // digitalWrite(ID_LED, 1); // off - in off phase
            } else {
                pos -= BLINK_OFF_MID;
                if (pos % (BLINK_SHORT + BLINK_OFF) <
                    BLINK_SHORT) {
                    pinMode(ID_LED, OUTPUT);
                    digitalWrite(ID_LED, 0); // on - in blink phase
                } else {
                    pinMode(ID_LED, INPUT); // floating (as onboard led)
                    // digitalWrite(ID_LED, 1); // off - in off phase
                }
            }
        }
    } 
    #endif // ID_LED
    lasttime = currenttime;
}


#include <display.h>
Display* ota_display=NULL;
bool ota_display_present = false;

void setup_ota() {
        // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // TODO: only enter OTA when requested

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount
        // SPIFFS using SPIFFS.end()
        Serial.println();
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() { 
        Serial.println("\nOTA End, success.");
        if(ota_display_present) {
            ota_display->i2c_start();
            ota_display->clear();
            ota_display->measure();
            delay(100);
            ota_display->clear_bus();            
        }
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        static int last_percent = -1;
        Ustring s;
        int percent = (int)(progress * 100 / total);
        if(percent != last_percent) {
            Serial.printf("Progress: %d%%\r", percent);
            // blink led, toggle every percent
            #ifdef ID_LED
            if(percent % 2) {
                pinMode(ID_LED, INPUT); // floating -> off
            } else {
                pinMode(ID_LED, OUTPUT);
                digitalWrite(ID_LED, 0);
            }
            #endif
            if(ota_display_present) {
                ota_display->clear();
                ota_display->print("Adoption\nactive\n\nProgress:\n");
                s.printf("%d%%", percent);
                ota_display->print(s);
                ota_display->measure();
            }
            last_percent = percent;
        }
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
            Serial.println("End Failed");
        if(ota_display_present) {
            ota_display->i2c_start();
            ota_display->clear();
            ota_display->print("OTA Error.");
            ota_display->measure();
        }
        ota_failed = true;
    });
}

uint32_t getChipId32() {
    #ifdef ESP32
        return ESP.getEfuseMac();
    #else
        return ESP.getChipId();
    #endif
}

void reconfigMode() {
    // go to access-point and reconfiguration mode to allow a new
    // firmware to be uploaded

    Ustring s;
    char *ap_ssid = (char *)(IOTEMPOWER_AP_RECONFIG_NAME "-xx-xL-xS");
    const char *ap_password = IOTEMPOWER_AP_RECONFIG_PASSWORD;

    Serial.println("Reconfiguration requested. Activating open AP-mode.");
    WiFi.disconnect(true);
    id_blinker(); //trigger init of random blink pattern
    sprintf(ap_ssid + strlen(ap_ssid) - 8, "%02x-%01dL-%01dS", getChipId32() & 255,
                long_blinks, short_blinks);

    // check if a display is present
    Wire.begin(); // check default i2c
    Wire.beginTransmission(0x3c);
    if (Wire.endTransmission() == 0) {
        Serial.println("Display shield found.");
        U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0);
        ota_display = new Display("testdisplay", u8g2);
        if(ota_display) {
            ota_display_present = true;
            ota_display->clear_bus(); // debug test TODO: remove
            Wire.begin(); // check default i2c
            ota_display->measure_init(); // debug test TODO: remove
            ota_display->i2c_start();
        }
    }

    Serial.printf("Connect to %s with password %s.\n", ap_ssid, ap_password);
    
    const char *flash_default_password = IOTEMPOWER_FLASH_DEFAULT_PASSWORD;
    Serial.printf("Setting flash default password to %s.\n",
                    flash_default_password);
    setup_ota();
    ArduinoOTA.setPassword(flash_default_password);
    ArduinoOTA.setHostname(ap_ssid);

   	WiFi.softAP(ap_ssid, ap_password);
    ArduinoOTA.begin();
    Serial.println("AP and OTA Ready, waiting 10min for new firmware.");

    unsigned long start_time = millis();
    const unsigned long wait_time = 10 * 60 * 1000; // 10 minutes
    int seconds_left;
    int last_seconds_left=-1;
    while (millis()-start_time < wait_time) { // try for 10 minutes
        seconds_left = (int)((wait_time - (millis() - start_time))/1000);
        ArduinoOTA.handle();
        if(ota_failed) {
            WiFi.softAP(ap_ssid, ap_password);
            ota_failed = false;
            start_time = millis(); // restart time
            ArduinoOTA.begin();
            // done in error
            // if(ota_display_present) {
            //     ota_display->i2c_start();
            // }
        }
        id_blinker(); // TODO: check why not blinking after crash
        yield();
        if(last_seconds_left != seconds_left) {
            Serial.print("Seconds left for adoption: ");
            Serial.print(seconds_left);
            Serial.print("\r");
            last_seconds_left = seconds_left;
            if(ota_display_present) {
                ota_display->clear();
                ota_display->print("ReconfigMode");
                ota_display->cursor(0,2);
                s.printf("node %02x", getChipId32() & 255);
                ota_display->print(s);
                s.printf("blinks %dL-%dS", long_blinks, short_blinks);
                ota_display->print(s);
                ota_display->cursor(0,4);
                s.printf("%ds left", seconds_left);
                ota_display->print(s);
                ota_display->cursor(0,5);
                ota_display->print("for adoption");
                ota_display->measure(); // trigger drawing
            }
        }
        yield(); // let WiFi do what it needs to do (if not present -> crash)
    }

    Serial.println();
    Serial.println("No adoption request done, rebooting...");
    if(ota_display_present) {
        ota_display->i2c_start();
        ota_display->clear();
        ota_display->measure();
        ota_display->clear_bus();
    }
    // Serial.println("Requesting password reset on next boot.");
    // int magicSize = sizeof(IOTEMPOWER_RECONFIG_MAGIC);
    // char rtcData[magicSize];
    // strncpy(rtcData, IOTEMPOWER_RECONFIG_MAGIC, magicSize);
    // ESP.rtcUserMemoryWrite(0, (uint32_t *)rtcData, magicSize);
    reboot(); // Always reboot after this to free all eventually not freed
              // memory
}

static bool reconfig_mode_active=false;
static bool adopt_flash_toggle = false;

void flash_mode_select() {
    // Check specific GPIO port if pressed and unpressed 2-4 times
    // to enter reconfiguration mode (allow generic reflash in AP
    // mode)
    Serial.println("Allow 5s to check if reconfiguration and AP "
                    "mode is requested.");
    int last = 1, toggles = 0;
    // blink a bit to show that we have booted and waiting for
    // potential input

    #ifdef ID_LED
    pinMode(ID_LED, INPUT); // floating -> off
    #endif
    pinMode(FLASHBUTTON, INPUT_PULLUP); // check flash button (d3 on wemos and
                                // nodemcu) 
    for (int i = 0; i < 500; i++) {
        #ifdef ID_LED
        if((i / 25) % 2) {
            pinMode(ID_LED, INPUT); // floating -> off
        } else {
            pinMode(ID_LED, OUTPUT);
            digitalWrite(ID_LED, 0);
        }
        #endif
        int new_state = digitalRead(0);
        if (new_state != last) {
            last = new_state;
            toggles++;
        }
        delay(10);
        if (toggles >= 4) break;
    }


    if (toggles >= 4) { // was pressed (and released) at least 2 times
        reconfigMode();
    }

    Serial.println("Continue to boot normally.");
    // register password-hash for uploading
    ArduinoOTA.setPasswordHash(keyhash);
}

////////////// mqtt
AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

#ifndef ESP32
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
#endif
Ticker wifiReconnectTimer;

Ustring node_topic(mqtt_topic);

static char *my_hostname;

void connectToMqtt() {
    if (reconfig_mode_active)
        return;
    Serial.println("Connecting to MQTT...");
    mqttClient.connect();
}


void connectToWifi() {
    // Start WiFi connection and register hostname
    Serial.print("Trying to connect to Wi-Fi with name ");
    Serial.println(WIFI_SSID);
    Serial.print("Registering hostname: ");
    if(reconfig_mode_active) {
        my_hostname = (char *)"iotempower-adoptee"; // TODO: define in defaults
        // my_hostname = (char *)"iotempower-xxxxxx";
        // sprintf(my_hostname + strlen(my_hostname) - 6, "%06x", getChipId32());
    } else {
        my_hostname = (char *)HOSTNAME;
    }
    #ifdef ESP32
    WiFi.setHostname(my_hostname);
    #else
    WiFi.hostname(my_hostname);
    #endif
    ArduinoOTA.setHostname(my_hostname);
    Serial.println(my_hostname);
    WiFi.mode(WIFI_STA);

    // Before wifi-start?
    Serial.println("Starting MDNS.");
    MDNS.begin(my_hostname);
    Serial.println("MDNS Ready.");

    // start ota
    ArduinoOTA.begin();
    Serial.println("OTA Ready.");

    if(reconfig_mode_active) {
        // use last known configuration (configured in WifiManager)
        Serial.println("Using last wifi credentials in adopt mode.");
        WiFi.begin();
    } else {
        Serial.println("Setting wifi credentials.");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
    Serial.println("Wifi begin called.");
    if(WiFi.isConnected()) {
        Serial.print("Already connected to Wi-Fi with IP: ");
        Serial.println(WiFi.localIP());
        wifi_connected = true;
        connectToMqtt();
    }
}

void onWifiDisconnect(
#ifdef ESP32
    WiFiEvent_t event, WiFiEventInfo_t info
#else
    const WiFiEventStationModeDisconnected &event
#endif
        ) {
    wifi_connected = false;
    Serial.println("Disconnected from Wi-Fi.");
    if(!reconfig_mode_active) {
        mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while
                                     // reconnecting to Wi-Fi
    }
    wifiReconnectTimer.once(2, connectToWifi);
}


void onWifiConnect(
#ifdef ESP32
    WiFiEvent_t event, WiFiEventInfo_t info
#else
    const WiFiEventStationModeDisconnected &event
#endif
        ) {
    Serial.print("Connected to Wi-Fi with IP: ");
    Serial.println(WiFi.localIP());
    wifi_connected = true;
    connectToMqtt();
}

void onMqttConnect(bool sessionPresent) {
    Serial.println("Connected to MQTT.");
    Serial.print("Session present: ");
    Serial.println(sessionPresent);

    devices_subscribe(mqttClient, node_topic);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.println("Disconnected from MQTT.");

    if (WiFi.isConnected()) {
        mqttReconnectTimer.once(2, connectToMqtt);
    }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
    Serial.print("Subscribe acknowledged.");
    Serial.print("  packetId: ");
    Serial.print(packetId);
    Serial.print("  qos: ");
    Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
    Serial.print("Unsubscribe acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
}

void onMqttMessage(char *topic, char *payload,
                   AsyncMqttClientMessageProperties properties, size_t len,
                   size_t index, size_t total) {
    Serial.print("Publish received.");
    Serial.print("  topic: ");
    Serial.print(topic);
    Serial.print("  qos: ");
    Serial.print(properties.qos);
    Serial.print("  dup: ");
    Serial.print(properties.dup);
    Serial.print("  retain: ");
    Serial.print(properties.retain);
    Serial.print("  len: ");
    Serial.print(len);
    Serial.print("  index: ");
    Serial.print(index);
    Serial.print("  total: ");
    Serial.print(total);

    Ustring utopic(topic);
    utopic.remove(0, node_topic.length() + 1);
    Ustring upayload(payload, (unsigned int)total);

    Serial.print(" payload: >");
    Serial.print(upayload.as_cstr());
    Serial.println("<");
    
    devices_receive(utopic, upayload);
}

void onMqttPublish(uint16_t packetId) {
    Serial.println("Publish acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
}


void setup() {
    // TODO: setup watchdog
    // TODO: consider not using serial at all and freeing it for other
    // connections
    // Serial.begin(115200);
    // Serial.println();
    ulog("Booting.");

    // intialize randomness
    long seed_helper=ESPTrueRandom.random(0,2000000);
    // TODO: fix that calling ESPTrueRandom crashes later
    Serial.print("Random generator seeded, testnumber: ");
    Serial.println(seed_helper);
    randomSeed((unsigned long)seed_helper); 

    flash_mode_select();

    #ifdef ESP32
    // TODO: anything equivalent for ESP32
    #else
    WiFi.setSleepMode(WIFI_NONE_SLEEP); // TODO: check if this works -> for better rgb-strip-smoothness
    #endif

    setup_ota();

//    WiFi.disconnect(true);
//    delay(10);
    
    // Try already to bring up WiFi
    #ifdef ESP32    
    WiFi.onEvent(onWifiConnect, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(onWifiDisconnect, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
    #else
    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
    #endif

    connectToWifi();

    // TODO: check if port is configurable

    if (!reconfig_mode_active) {
        mqttClient.onConnect(onMqttConnect);
        mqttClient.onDisconnect(onMqttDisconnect);
        mqttClient.onSubscribe(onMqttSubscribe);
        mqttClient.onUnsubscribe(onMqttUnsubscribe);
        mqttClient.onMessage(onMqttMessage);
        mqttClient.onPublish(onMqttPublish);
        mqttClient.setServer(mqtt_server, 1883);
        mqttClient.setClientId(my_hostname);
        if (mqtt_user[0]) { // auth given
            mqttClient.setCredentials(mqtt_user, mqtt_password);
        }
        if(ulnoiot_init) ulnoiot_init(); // call user init from setup.cpp
        devices_start(); // call start of all devices
        if(ulnoiot_start) ulnoiot_start(); // call user start from setup.cpp
        connectToMqtt(); // only subscribe after setup
    } else {  // do something to show that we are in adopt mode
        // enable flashing that light
        #ifdef ID_LED
        pinMode(ID_LED, OUTPUT);
        digitalWrite(ID_LED, adopt_flash_toggle);
        #endif
    }

}

static unsigned long last_transmission = millis();
static unsigned long last_published = millis();
static unsigned long last_measured = millis();
static unsigned long transmission_delta = 5000;
void transmission_interval(int interval) {
    transmission_delta = ((unsigned long)interval) * 1000;
}

// TODO: is this actually necessary if sending is limited
// too small delay prevents makes analog reads interrupt wifi
static unsigned long _measure_delay = 1;
void measure_delay(int delay) {
    // TODO: should 0 be allowed here?
    if (delay <= 0)
        delay = 1;
    _measure_delay = delay;
}

void loop() {
    unsigned long current_time;
    // TODO: make sure nothing weird happens -> watchdog
    //MDNS.update();
    ArduinoOTA.handle();
    current_time = millis();
    if (!reconfig_mode_active) {
        do_later_check(); // work the scheduler
        if (current_time - last_measured >= _measure_delay) {
            devices_update();
            last_measured = current_time;
            if (mqttClient.connected()) {
                if (current_time - last_published >= MIN_PUBLISH_TIME_MS) {
                    // go through devices and send reports if necessary
                    if (transmission_delta > 0 &&
                        current_time - last_transmission >=
                            transmission_delta) {
                        if (devices_publish(mqttClient, node_topic, true)) {
                            ulog("Free memory: %ld",ESP.getFreeHeap());
                            last_transmission = current_time;
                            last_published = current_time;
                        }
                    } else { // no full transmission necessary
                        if (devices_publish(mqttClient, node_topic, false)) {
                            last_published = current_time;
                        }
                    } // endif transmission delta
                } // endif update delay
            } // endif measure delay
        } else {
            // ulog("Trouble connecting to mqtt server.");
            // Don't block here with delay as other processes might
            // be running in background
            // TODO: wait a bit before trying to reconnect.
        } // endif mqtt connect
        // mqtt_client->yield(_loop_delay);
        // ulnoiot_loop(); // TODO: think if this can actually be
        // skipped in the iotempower-philosophy -> maybe move to driver
        // callbacks
    } else { // reconfig mode is active
        #ifdef ID_LED
        // flashing very rapidly
        if (wifi_connected && current_time - last_measured >= 80) {
            adopt_flash_toggle = !adopt_flash_toggle;
            digitalWrite(ID_LED, adopt_flash_toggle);
            last_measured = current_time;
        }
        #endif
    } // endif !reconfig_mode_active
}