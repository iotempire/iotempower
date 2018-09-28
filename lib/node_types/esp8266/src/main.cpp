// main.cpp
// main program for ulnoiot esp8266 firmware

////// Standard libraries
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <WiFiUdp.h>

////// adapted libraries
#include <Ticker.h>
#include <WiFiManager.h>

// PubSubClient.h and ArduinoMqtt unstable
#include <AsyncMqttClient.h>

//// configuration
#include <device-manager.h>
#include <toolbox.h>
#include <ulnoiot-default.h>

// node specific code
#include "config.h"
#include "key.h"
#include "pins-wrapper.h"
#include "wifi-config.h"

// ulnoiot functions for user modification in setup.cpp
void (ulnoiot_init)() __attribute__((weak));
void (ulnoiot_start)() __attribute__((weak));

int long_blinks = 0, short_blinks = 0;

// TODO: find better solution
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
    int pos;
    static unsigned long lasttime;
    unsigned long currenttime;
    int delta;

    if (long_blinks == 0) { // first time, still unitialized
        // randomness for 25 different blink patterns (5*5)
        long_blinks = ESP8266TrueRandom.random(1, 6);
        short_blinks = ESP8266TrueRandom.random(1, 6);
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
    pos = global_pos;
    #ifdef ID_LED
    if (pos < BLINK_OFF_START) { // still in BLINK_OFF_START
        digitalWrite(ID_LED, 1); // off - in off phase
    } else { // already pass BLINK_OFF_START
        pos -= BLINK_OFF_START;
        if(pos < long_blinks * (BLINK_LONG + BLINK_OFF)) { // in long blink phase

            if (pos % (BLINK_LONG + BLINK_OFF) < BLINK_LONG) {
                digitalWrite(ID_LED, 0); // on - in blink phase
            } else {
                digitalWrite(ID_LED, 1); // off - in off phase
            }

        } else { // in short blink phase
            pos -= long_blinks * (BLINK_LONG + BLINK_OFF);
            if(pos < BLINK_OFF_MID) { // still in BLINK_OFF_MID
                digitalWrite(ID_LED, 1); // off - in off phase
            } else {
                pos -= BLINK_OFF_MID;
                if (pos % (BLINK_SHORT + BLINK_OFF) <
                    BLINK_SHORT) {
                    digitalWrite(ID_LED, 0); // on - in blink phase
                } else {
                    digitalWrite(ID_LED, 1); // off - in off phase
                }
            }
        }
    } 
    #endif // ID_LED
    lasttime = currenttime;
}

void reconfigMode() {
    // go to access-point and reconfiguration mode

    char *ap_ssid = (char *)(ULNOIOT_AP_RECONFIG_NAME "-xx-xx");
    const char *ap_password = ULNOIOT_AP_RECONFIG_PASSWORD;

    Serial.println("Reconfiguration requested. Activating AP-mode.");
    WiFi.disconnect(true);
    id_blinker(); //trigger init of random blink pattern
    sprintf(ap_ssid + strlen(ap_ssid) - 5, "%02x-%02d", ESP.getChipId() & 255,
                long_blinks * 10 + short_blinks);
    Serial.printf("Connect to %s with password %s.\n", ap_ssid, ap_password);
    WiFiManager wifiManager;
    wifiManager.setConnectTimeout(3600); // 60 min timeout

    // // parameter test
    // WiFiManagerParameter test_param("tparam", "test parameter", "123",
    // 5); wifiManager.addParameter(&test_param);
    String blink_pattern = "<p>Blink pattern: " + String(long_blinks) +
                           " longs, " + String(short_blinks) + " shorts</p>";
    WiFiManagerParameter custom_text1(blink_pattern.c_str());
    wifiManager.addParameter(&custom_text1);

    // obsolete now as we only use one name
    // String name_display = "<p>Parameter for initialize in ulnoiot: " +
    //                       String(ap_ssid + strlen(ap_ssid) - 6) + "</p>";
    // WiFiManagerParameter custom_text2(name_display.c_str());
    // wifiManager.addParameter(&custom_text2);

    //  const char * menu[] = {"wifi","param","sep","exit"};
    //  wifiManager.setMenu(menu,4);
    //  wifiManager.setShowInfoErase(true); // disable info-field

    // make sure, we head directly to wifi config, when configuration mode
    // activated
    wifiManager.setCaptivePortalWifiRedirect(true);

    wifiManager.setConfigPortalBlocking(false); // allow interrupts
    // wifiManager.autoConnect(ap_ssid, ap_password);
    //wifiManager.startConfigPortal(ap_ssid, ap_password);
    wifiManager.startConfigPortal(ap_ssid, "");

    while (1) {
        if (wifiManager.process())
            break;
        id_blinker();
        yield();
    }
    wifiManager.stopWebPortal();
    WiFi.softAPdisconnect(false); // stop accesspoint mode
    if (wifiManager.getLastConxResult() ==
        WL_CONNECTED) { // there was a connection made
        // trigger reconfigurability (flash with default password) after
        // next reboot
        Serial.println("Requesting password reset on next boot.");
        int magicSize = sizeof(ULNOIOT_RECONFIG_MAGIC);
        char rtcData[magicSize];
        strncpy(rtcData, ULNOIOT_RECONFIG_MAGIC, magicSize);
        ESP.rtcUserMemoryWrite(0, (uint32_t *)rtcData, magicSize);
    } // TODO: consider going back to configuration mode if not successful
    reboot(); // Always reboot after this to free all eventually not freed
              // memory used by WiFiManager
              // TODO: go directly to OTA-mode for a while and then quit
}

bool reconfig_mode_active=false;

void flash_mode_select() {
    // Check if flash with default password is requested
    int magicSize = sizeof(ULNOIOT_RECONFIG_MAGIC);
    char rtcData[magicSize];
    rtcData[magicSize] = 0;
    ESP.rtcUserMemoryRead(0, (uint32_t *)rtcData, magicSize);
    if (memcmp(rtcData, ULNOIOT_RECONFIG_MAGIC, magicSize) == 0) {
        reconfig_mode_active = true;
        Serial.println("Reconfiguration mode requested.");
        // reset request
        rtcData[0] = 0;
        rtcData[1] = 0;
        ESP.rtcUserMemoryWrite(0, (uint32_t *)rtcData, magicSize);

        // // debug read back
        // ESP.rtcUserMemoryRead(0, (uint32_t*) rtcData, magicSize);
        // Serial.print("Another comparison yields: ");
        // Serial.println(memcmp(rtcData,ULNOIOT_RECONFIG_MAGIC,magicSize));

        const char *flash_default_password = ULNOIOT_FLASH_DEFAULT_PASSWORD;
        Serial.printf("Setting flash default password to %s.\n",
                      flash_default_password);
        ArduinoOTA.setPassword(flash_default_password);
    } else { // do not check for special config mode, when just rebooted out
             // of it
        // Check specific GPIO port if pressed and unpressed 2-4 times
        // to enter reconfiguration mode (allow generic reflash in AP
        // mode)
        Serial.println("Allow 5s to check if reconfiguration and AP "
                       "mode is requested.");
        int last = 1, toggles = 0;
        // blink a bit to show that we have booted and waiting for
        // potential input

        pinMode(LED_BUILTIN, OUTPUT);
        pinMode(FLASHBUTTON, INPUT_PULLUP); // check flash button (d3 on wemos and
                                  // nodemcu) 
        for (int i = 0; i < 500; i++) {
            digitalWrite(LED_BUILTIN, (i / 25) % 2);
            int new_state = digitalRead(0);
            if (new_state != last) {
                last = new_state;
                toggles++;
            }
            delay(10);
        }

 
        if (toggles >= 4 && toggles <= 8) { // was pressed 2-4 times
            reconfigMode();
        }

        Serial.println("Continue to boot normally.");
        // register password-hash for uploading
        ArduinoOTA.setPasswordHash(keyhash);

    } // endif default password flash mode
}

////////////// mqtt
AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

Ustring node_topic(mqtt_topic);

static char *my_hostname;

void connectToWifi() {
    // Start WiFi connection and register hostname
    Serial.println("Connecting to Wi-Fi...");
    Serial.print("Registering hostname: ");
    if(reconfig_mode_active) {
        my_hostname = (char *)"ulnoiot_adoptee"; // TODO: define in defaults
        // my_hostname = (char *)"ulnoiot_xxxxxx";
        // sprintf(my_hostname + strlen(my_hostname) - 6, "%06x", ESP.getChipId());
    } else {
        my_hostname = (char *)HOSTNAME;
    }
    WiFi.hostname(my_hostname);
    ArduinoOTA.setHostname(my_hostname);
    Serial.println(my_hostname);
    WiFi.mode(WIFI_STA);

//#ifdef WIFI_SSID
    if(reconfig_mode_active) {
        WiFi.begin(); // use last known configuration (configured in WifiManager)
    } else {
        Serial.println("Setting wifi credentials.");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
// #else
//     // This part is actually kind of obsolete now. TODO: consider removal
//     WiFi.begin();
// #endif
    MDNS.begin(my_hostname);
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event) {
    Serial.println("Disconnected from Wi-Fi.");
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while
                                 // reconnecting to Wi-Fi
    wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
    if (reconfig_mode_active)
        return;
    Serial.println("Connecting to MQTT...");
    mqttClient.connect();
}

void onWifiConnect(const WiFiEventStationModeGotIP &event) {
    Serial.println("Connected to Wi-Fi.");
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
    long seed_helper=ESP8266TrueRandom.random(0,2000000);
    // TODO: fix that calling ESP8266TrueRandom crashes later
    Serial.print("Random generator seeded, testnumber: ");
    Serial.println(seed_helper);
    randomSeed((unsigned long)seed_helper); 

    // // TODO: is this really necessary or is it better just hardcoded?
    // if( ! SPIFFS.begin() ) {
    //   // not formatted?
    //   Serial.println("Spiff not accessible, formatting...");
    //   if( SPIFFS.format() ) {
    //     if( ! SPIFFS.begin() ) {
    //       Serial.println("Can't format SPIFFS.");
    //       reboot();
    //     } else {
    //       Serial.println("Format successful.");
    //     }
    //   }
    // }
    // // TODO: read configuration from SPIFFS -> necessary? or better rtc?

    flash_mode_select();

    WiFi.setSleepMode(WIFI_NONE_SLEEP); // TODO: check if this works

    // Try already to bring up WiFi
    connectToWifi();


    // This reboot and wait might not be necessary
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        reboot();
    }

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
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
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
    });
    ArduinoOTA.begin();
    Serial.println("OTA Ready.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // TODO: check if port is configurable

    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

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
    ArduinoOTA.handle();
    if (!reconfig_mode_active) {
        if (mqttClient.connected()) {
            current_time = millis();
            do_later_check(); // work the scheduler
            if (current_time - last_measured >= _measure_delay) {
                devices_update();
                last_measured = current_time;
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
        // skipped in the ulnoiot-philosophy -> maybe move to driver
        // callbacks
    } // endif !reconfig_mode_active
}
