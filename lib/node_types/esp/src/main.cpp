/**
 * @file main.cpp
 * @brief Main program for IoTempower ESP8266/ESP32 firmware
 * @author ulno
 * 
 * OVERVIEW
 * ========
 * This is the main entry point for IoTempower nodes running on ESP8266 and ESP32 microcontrollers.
 * It orchestrates the entire lifecycle of an IoT node from boot to operation.
 * 
 * WHAT IOTEMPOWER ADDS TO STANDARD ARDUINO/MQTT PROJECTS
 * =======================================================
 * IoTempower transforms standard Arduino projects into a comprehensive, production-ready IoT framework:
 * 
 * 1. AUTOMATIC DEVICE MANAGEMENT
 *    - Devices register themselves automatically during construction
 *    - The DeviceManager singleton tracks all devices and manages their lifecycle
 *    - No manual device tracking or initialization code needed
 * 
 * 2. SIMPLIFIED MQTT INTEGRATION
 *    - Automatic MQTT topic generation based on device names
 *    - Built-in reconnection logic with configurable retry intervals
 *    - Automatic publishing when device values change
 *    - Home Assistant MQTT discovery support out of the box
 * 
 * 3. OVER-THE-AIR (OTA) UPDATE SYSTEM
 *    - Password-protected OTA updates for secure remote firmware deployment
 *    - Special "adoption mode" for initial configuration of unconfigured nodes
 *    - Visual feedback via LED blink patterns for node identification
 *    - Display support for adoption progress (if I2C display present)
 * 
 * 4. ROBUST WIFI MANAGEMENT
 *    - Automatic WiFi connection with retry logic
 *    - Hostname registration and mDNS support
 *    - Configurable for both production and adoption modes
 *    - TLS/SSL support for secure MQTT connections
 * 
 * 5. DEVICE POLLING AND MEASUREMENT SYSTEM
 *    - Each device can have its own poll rate in microseconds
 *    - Automatic scheduling ensures devices are read at the right time
 *    - Support for "precision" devices that need exact timing
 *    - Built-in change detection to minimize unnecessary MQTT traffic
 * 
 * 6. CALLBACK AND FILTER SYSTEM
 *    - Devices can have filter callbacks to validate/modify measurements
 *    - Change callbacks trigger when values change
 *    - Flexible callback chaining for complex logic
 * 
 * 7. SCHEDULER (do_later)
 *    - Schedule callbacks to execute after a delay
 *    - Useful for debouncing, delayed actions, deep sleep scheduling
 *    - Automatic management of scheduled tasks
 * 
 * 8. USER-FRIENDLY CONFIGURATION
 *    - Users write simple setup.cpp with high-level device declarations
 *    - Automatic code generation and build process
 *    - No need to understand MQTT, WiFi, or complex Arduino code
 * 
 * MAIN PROGRAM FLOW
 * =================
 * 
 * STARTUP SEQUENCE:
 * 1. Hardware initialization (brownout detection, serial, LED)
 * 2. Flash button check for entering adoption/reconfig mode
 * 3. WiFi connection establishment
 * 4. OTA system initialization
 * 5. User init() function call (from setup.cpp)
 * 6. Device manager starts all registered devices
 * 7. User start() function call (from setup.cpp)
 * 8. MQTT connection establishment
 * 
 * MAIN LOOP:
 * 1. Handle WiFi connection status
 * 2. Maintain MQTT connection
 * 3. Execute do_later scheduled callbacks
 * 4. Update devices in precision interval (exact timing for sensitive devices)
 * 5. Update regular devices
 * 6. Publish changed device values via MQTT
 * 7. Handle OTA updates
 * 8. Run user loop() function (if defined)
 * 
 * ADOPTION MODE
 * =============
 * When a node is unconfigured or the flash button is pressed 2+ times during boot:
 * - Node creates its own WiFi access point with a unique name
 * - LED blinks in a unique pattern for visual identification
 * - OTA server runs for 10 minutes allowing firmware upload
 * - Display (if present) shows adoption status
 * - After timeout or successful adoption, node reboots
 * 
 * See docs for more information on the IoTempower architecture.
 */

// TODO: enable when PJON works 
// // for randomness, we need crypto library first
// #include <hydrogen.h>

// TODO: check when this is actually not harmful
#define BROWNOUT_DETECT_DISABLED

/**
 * LIBRARY INCLUDES
 * ================
 * These are the key libraries that provide the foundation for IoTempower functionality.
 */

////// Standard libraries
#include <ArduinoOTA.h>  // Over-the-air firmware update support
//#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal - obsolete due to dongle


#include "config-wrapper.h"  // Wraps user configuration from node config files

#ifdef MQTT_USE_TLS
    #include <time.h>

#endif

#ifdef ESP32
    // // the flash string helper broke in 6.2 - see https://github.com/espressif/arduino-esp32/issues/8108
    // #define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
    // #define F(string_literal) (FPSTR(PSTR(string_literal)))
    #include <WiFi.h>
    #include <ESPmDNS.h>

    #ifdef MQTT_USE_TLS
        #include <WiFiClientSecure.h>
    #endif


    #ifdef BROWNOUT_DETECT_DISABLED
        #include "soc/soc.h"
        #include "soc/rtc_cntl_reg.h"
    #endif
#else
    #include <ESP8266WiFi.h>
    #include <ESP8266mDNS.h>

    #ifdef MQTT_USE_TLS
        const char mqtt_ca_cert_char[] PROGMEM = mqtt_ca_cert; 
        BearSSL::X509List *serverTrustedCA = new BearSSL::X509List(mqtt_ca_cert_char);
    #endif
#endif
//#include <FS.h> // no filesystem used
#include <WiFiUdp.h>

// MQTT

//// AsyncMqtt disabled in favor of PubSubClient
//#include <Ticker.h> // needed for AsyncMqtt
//#include <AsyncMqttClient.h>

// Timeout Values that can be modified in PubSubClient
//#define MQTT_KEEPALIVE 75
//#define MQTT_SOCKET_TIMEOUT 75
#include <PubSubClient.h>

String mqtt_management_topic;

////// adapted libraries
// none currently


// helpers
#include <device-manager.h>
#include <toolbox.h>

//// default configuration
#include <iotempower-default.h>

// TODO: enable PJON, when UDP works on esp8266 
// // PJON, we use PJON as generic network layer (depends on default)
// #define PJON_MAX_PACKETS 1
// #define PJON_PACKET_MAX_LENGTH IOTEMPOWER_MAX_BUFLEN
// #define PJON_INCLUDE_ASYNC_ACK true
// #define PJON_MAX_RECENT_PACKET_IDS 10
// #include <PJON.h>
// PJON<GlobalUDP> pjon_bus;

#include "key.h"



/**
 * USER CALLBACK FUNCTIONS
 * ========================
 * These weak functions can be defined by users in setup.cpp to customize node behavior.
 * 
 * iotempower_init() - Called during setup before devices are started
 *                     Use this to create device objects and configure them
 * 
 * iotempower_start() - Called after devices have been started
 *                      Use this for any post-initialization setup
 */
void (iotempower_init)() __attribute__((weak));
void (iotempower_start)() __attribute__((weak));

/**
 * GLOBAL STATE VARIABLES
 * ======================
 * These variables track the node's operational state throughout its lifecycle.
 */

// LED blink pattern for node identification during adoption
// Randomly generated pattern (1-5 long blinks, 1-6 short blinks) makes each node visually unique
int long_blinks = 0, short_blinks = 0;

// Network connection status flags
bool wifi_connected = false;  // True when connected to WiFi network
bool ota_failed = false;      // Set when an OTA update attempt fails

#ifndef mqtt_server
    // Space for mqtt_server ip (max 16 chars)
    Ustring mqtt_server_buffer; // can be empty
#endif

// TODO: find better solution (add display support?)
#ifndef ONBOARDLED
    #pragma "No ONBOARDLED is defined for this board, there will be no indicator led" warning
#else
    #define ID_LED ONBOARDLED
#endif
/**
 * LED BLINK PATTERN TIMING (in milliseconds)
 * ===========================================
 * These constants define the visual identification pattern during adoption mode.
 * Each node gets a unique combination of long and short blinks for easy identification.
 */
#define BLINK_OFF_START 2000  // Initial pause before pattern starts
#define BLINK_LONG 800        // Duration of a long blink
#define BLINK_OFF_MID 800     // Pause between long and short blinks
#define BLINK_SHORT 200       // Duration of a short blink
#define BLINK_OFF 500         // Pause between individual blinks

#define IOTEMPOWER_OTA_PORT 8266  // OTA port (works for both ESP8266 and ESP32)

/**
 * @brief Turn on the onboard LED
 * 
 * Handles both regular GPIO LEDs and special onboard LEDs that may be inverted.
 * Used during adoption mode blink patterns and OTA progress indication.
 */
void onboard_led_on() {
    #ifdef ID_LED
        #ifdef ONBOARDLED_FULL_GPIO
            digitalWrite(ID_LED, 1); // on - in blink phase
        #else
            pinMode(ID_LED, OUTPUT);
            digitalWrite(ID_LED, 0); // on - in blink phase
        #endif
    #endif
}

void onboard_led_off() {
    #ifdef ID_LED
        #ifdef ONBOARDLED_FULL_GPIO
            digitalWrite(ID_LED, 0); // off - in off phase
        #else
            pinMode(ID_LED, INPUT); // floating (as onboard led) -> off
        #endif
    #endif
}

/**
 * @brief Generate unique LED blink pattern for node identification
 * 
 * Creates a repeating pattern of long and short blinks unique to this node.
 * Called continuously during adoption mode to help users identify which physical
 * node they are working with.
 * 
 * The pattern is: [2s pause] [N long blinks] [0.8s pause] [M short blinks]
 * Where N = 1-5 and M = 1-6, giving 30 possible unique patterns.
 * 
 * This is crucial in environments with multiple nodes, as it allows visual
 * identification without needing to read serial output or check MAC addresses.
 */
void id_blinker() {
    static bool init_just_done = false;
    static int total, global_pos;
    static unsigned long lasttime;
    unsigned long currenttime;
    int delta;

    if (long_blinks == 0) { // first time, still unitialized
        // randomness for 30 different blink patterns (5*6)
        long_blinks = urandom(1, 6);
        short_blinks = urandom(1, 7);
        Serial.printf("Blink pattern: %d long_blinks, %d short_blinks\n",
                      long_blinks, short_blinks);
        total = BLINK_OFF_START +
                long_blinks * (BLINK_LONG + BLINK_OFF) +
                BLINK_OFF_MID +
                short_blinks * (BLINK_SHORT + BLINK_OFF);
        onboard_led_on(); // on - start with a long blink
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
        // off - in off phase
        onboard_led_off();
    } else { // already pass BLINK_OFF_START
        pos -= BLINK_OFF_START;
        if(pos < long_blinks * (BLINK_LONG + BLINK_OFF)) { // in long blink phase

            if (pos % (BLINK_LONG + BLINK_OFF) < BLINK_LONG) {
                onboard_led_on();
            } else {
                onboard_led_off();
            }

        } else { // in short blink phase
            pos -= long_blinks * (BLINK_LONG + BLINK_OFF);
            if(pos < BLINK_OFF_MID) { // still in BLINK_OFF_MID
                onboard_led_off();
            } else {
                pos -= BLINK_OFF_MID;
                if (pos % (BLINK_SHORT + BLINK_OFF) <
                    BLINK_SHORT) {
                    onboard_led_on();
                } else {
                    onboard_led_off();
                }
            }
        }
    } 
    #endif // ID_LED
    lasttime = currenttime;
}

// TODO: refactor this out so that it can be overwritten for m5 display
#include <dev_display_i2c.h>
Display* ota_display=NULL;
bool ota_display_present = false;

/**
 * @brief Initialize the Over-The-Air (OTA) update system
 * 
 * Sets up the Arduino OTA library with progress callbacks for:
 * - Visual feedback via LED during update
 * - Display updates (if I2C display is present)
 * - Error handling and reporting
 * 
 * This is a key IoTempower feature that allows firmware updates without
 * physical access to the device. The system is:
 * - Password protected for security
 * - Progress-aware with visual feedback
 * - Error-resilient with automatic retry capability
 */
void setup_ota() {
    ArduinoOTA.setPort(IOTEMPOWER_OTA_PORT);

    // TODO: only enter OTA when requested

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = F("sketch");
        else // U_SPIFFS
            type = F("filesystem");

        // NOTE: if updating SPIFFS this would be the place to unmount
        // SPIFFS using SPIFFS.end()
        Serial.println();
        Serial.println(F("Start updating "));
        Serial.println(type);
    });
    ArduinoOTA.onEnd([]() { 
        Serial.println(F("\nOTA End, success."));
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
                onboard_led_off();
            } else {
                onboard_led_on();
            }
            #endif
            if(ota_display_present) {
                ota_display->clear();
                ota_display->print(F("Adoption\nactive\n\nProgress:\n"));
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
            Serial.println(F("Auth Failed"));
        else if (error == OTA_BEGIN_ERROR)
            Serial.println(F("Begin Failed"));
        else if (error == OTA_CONNECT_ERROR)
            Serial.println(F("Connect Failed"));
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println(F("Receive Failed"));
        else if (error == OTA_END_ERROR)
            Serial.println(F("End Failed"));
        if(ota_display_present) {
            ota_display->i2c_start();
            ota_display->clear();
            ota_display->print(F("OTA Error."));
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

/**
 * @brief Enter adoption/reconfiguration mode
 * 
 * ADOPTION MODE - A KEY IOTEMPOWER FEATURE
 * =========================================
 * This mode allows unconfigured nodes to be "adopted" into an IoTempower system:
 * 
 * 1. Node creates its own WiFi access point with a unique name
 *    - SSID format: "uiot-node-XX-NL-MS" where:
 *      XX = last 2 hex digits of chip ID
 *      N = number of long blinks
 *      M = number of short blinks
 * 
 * 2. Node blinks LED in unique pattern for visual identification
 * 
 * 3. OTA server runs for 10 minutes waiting for firmware upload
 * 
 * 4. If display is present, shows adoption status and countdown
 * 
 * 5. After timeout or successful adoption, node reboots
 * 
 * This eliminates the need for serial connection during initial setup,
 * making deployment much easier in production environments.
 */
void reconfigMode() {
    // Go to access-point and reconfiguration mode to allow a new
    // firmware to be uploaded

    Ustring ssid;
    Ustring s;
    const char *ap_password = IOTEMPOWER_AP_RECONFIG_PASSWORD;

    ulog(F("Reconfiguration requested. Activating open AP-mode."));
    WiFi.disconnect(true);
    id_blinker(); //trigger init of random blink pattern
    ssid.printf("%s-%02x-%01dL-%01dS", 
        IOTEMPOWER_AP_RECONFIG_NAME, getChipId32() & 255,
        long_blinks, short_blinks);

    // check if a display is present
    Wire.begin(); // check default i2c // check, if it's the same on esp32 for wemos shield
    Wire.beginTransmission(0x3c);
    if (Wire.endTransmission() == 0) {
        ulog(F("Display shield found."));
        U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0);
        ota_display = new Display("testdisplay", u8g2);
        if(ota_display) {
            ota_display_present = true;
            ota_display->i2c_start();
        }
    }

    ulog(F("Connect to %s with password %s."), ssid.as_cstr(), ap_password);
    
    const char *flash_default_password = IOTEMPOWER_FLASH_DEFAULT_PASSWORD;
    ulog(F("Setting flash default password to %s."),
                    flash_default_password);
    setup_ota();
    ArduinoOTA.setPort(IOTEMPOWER_OTA_PORT);
    ArduinoOTA.setPassword(flash_default_password);
    ArduinoOTA.setHostname(ssid.as_cstr());

   	WiFi.softAP(ssid.as_cstr(), ap_password);
    ArduinoOTA.begin();
    ulog(F("AP and OTA Ready, waiting 10min for new firmware."));

    unsigned long start_time = millis();
    const unsigned long wait_time = 10 * 60 * 1000; // 10 minutes
    int seconds_left;
    int last_seconds_left=-1;
    while (millis()-start_time < wait_time) { // try for 10 minutes
        seconds_left = (int)((wait_time - (millis() - start_time))/1000);
        ArduinoOTA.handle();
        if(ota_failed) {
            WiFi.softAP(ssid.as_cstr(), ap_password);
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
            Serial.print(F("Seconds left for adoption: "));
            Serial.print(seconds_left);
            Serial.print(F("\r"));
            last_seconds_left = seconds_left;
            if(ota_display_present) {
                ota_display->clear();
                ota_display->print(F("ReconfigMode"));
                ota_display->cursor(0,2);
                s.printf("Node: %02x", getChipId32() & 255);
                ota_display->print(s);
                ota_display->cursor(0,3);
                s.printf("Blinks %dL %dS", long_blinks, short_blinks);
                ota_display->print(s);
                ota_display->cursor(0,4);
                s.printf("%ds left", seconds_left);
                ota_display->print(s);
                ota_display->cursor(0,5);
                ota_display->print(F("for adoption"));
                ota_display->measure(); // trigger drawing
            }
        }
        yield(); // let WiFi do what it needs to do (if not present -> crash)
    }

    Serial.println();
    Serial.println(F("No adoption request done, rebooting..."));
    if(ota_display_present) {
        ota_display->i2c_start();
        ota_display->clear();
        ota_display->measure();
        ota_display->clear_bus();
    }
    // Serial.println(F("Requesting password reset on next boot."));
    // int magicSize = sizeof(IOTEMPOWER_RECONFIG_MAGIC);
    // char rtcData[magicSize];
    // strncpy(rtcData, IOTEMPOWER_RECONFIG_MAGIC, magicSize);
    // ESP.rtcUserMemoryWrite(0, (uint32_t *)rtcData, magicSize);
    reboot(); // Always reboot after this to free all eventually not freed
              // memory
}

static bool reconfig_mode_active=false;
static bool adopt_flash_toggle = false;

/**
 * @brief Check if user wants to enter adoption/reconfig mode
 * 
 * During the first 5 seconds after boot, this function:
 * 1. Blinks LED to show the node is ready for mode selection
 * 2. Monitors the flash button (if present)
 * 3. Counts button press/release cycles (toggles)
 * 4. Enters adoption mode if button pressed 2+ times
 * 
 * This provides a simple physical interface for triggering adoption mode
 * without needing to modify configuration files or use serial commands.
 * 
 * The 5-second window is long enough to press the button but short enough
 * not to significantly delay normal operation.
 */
void flash_mode_select() {
    // Check specific GPIO port if pressed and unpressed 2-4 times
    // to enter reconfiguration mode (allow generic reflash in AP
    // mode)
    Serial.println(F("Allow 5s to check if reconfiguration and AP "
                    "mode is requested."));
    int last = 1, toggles = 0;
    // blink a bit to show that we have booted and waiting for
    // potential reconfig/adopt mode selection

    onboard_led_off();
    
    #ifdef FLASHBUTTON
        pinMode(FLASHBUTTON, INPUT_PULLUP); // D3 on wemos and nodemcu
        for (int i = 0; i < 500; i++) {
            #ifdef ID_LED
                if((i / 25) % 2) {
                    onboard_led_off();
                } else {
                    onboard_led_on();
                }
            #endif
            int new_state = digitalRead(FLASHBUTTON);
            if (new_state != last) {
                last = new_state;
                toggles++;
            }
            delay(10);
            if (toggles >= 4) break;
        }
    #endif // FLASHBUTTON

    if (toggles >= 4) { // was pressed (and released) at least 2 times
        reconfigMode();
    }

    Serial.println(F("Continue to boot normally."));
    // register password-hash for uploading
    ArduinoOTA.setPasswordHash(keyhash);
}

/**
 * MQTT CLIENT SETUP
 * =================
 * IoTempower uses PubSubClient for MQTT communication.
 * 
 * Note: AsyncMqttClient was disabled due to conflicts with other interrupt-based
 * libraries. PubSubClient is more reliable but requires manual loop() calls.
 */

////AsyncMqttClient disabled in favor of PubSubClient
//AsyncMqttClient mqttClient;
//Ticker mqttReconnectTimer;
//
//bool mqtt_just_connected = false;
//
//#ifndef ESP32
//WiFiEventHandler wifiConnectHandler;
//WiFiEventHandler wifiDisconnectHandler;
//#endif
//Ticker wifiReconnectTimer;
//
//Ustring node_topic(mqtt_topic);
//
//static char *my_hostname;
//


/// WiFi Network setup

#ifdef MQTT_USE_TLS
    WiFiClientSecure netClient;
#else
    WiFiClient netClient;
#endif

static char *my_hostname;

/**
 * MQTT CLIENT AND STATE
 * =====================
 * The MQTT client handles all communication with the broker.
 * IoTempower adds automatic connection management and message routing.
 */
////////////// mqtt
PubSubClient mqttClient(netClient);  // MQTT client instance
Ustring node_topic(mqtt_topic);      // Base topic for this node (from config)
bool mqtt_connected = false;         // Connection state flag
// char mqtt_id[33]="01234567890123456789012";
#define MQTT_RETRY_INTERVAL 5000     // Retry MQTT connection every 5 seconds
unsigned long mqtt_last_attempt = millis() - MQTT_RETRY_INTERVAL;

/**
 * @brief Handle incoming MQTT messages
 * 
 * AUTOMATIC MESSAGE ROUTING
 * =========================
 * IoTempower automatically routes MQTT messages to the correct device:
 * 
 * 1. Strips the node topic prefix from the full topic
 * 2. Parses remaining topic to identify target device/subdevice
 * 3. Calls device_manager.receive() which dispatches to the correct device
 * 4. Device's receive callback handles the actual command
 * 
 * This means users don't need to write message parsing code - they just
 * define what should happen when a device receives a command.
 * 
 * @param topic Full MQTT topic that received a message
 * @param payload Message payload bytes
 * @param len Length of payload
 */
void onMqttMessage(char *topic, byte *payload, unsigned int len) {
    Ustring log_buffer;
//    log_buffer.printf(F("Publish received.  topic: %s  len:  %u"), // TODO: allow use of flash-string
    log_buffer.printf(("Publish received.  topic: %s  len:  %u"),
        topic, len);

    Ustring utopic(topic);
    utopic.remove(0, node_topic.length() + 1);
    Ustring upayload((char *)payload, (unsigned int)len);

    log_buffer.add(F(" payload: >"));
    log_buffer.add(upayload);
    log_buffer.add(F("<"));

    device_manager.receive(utopic, upayload);
}


/**
 * @brief Initialize MQTT client connection parameters
 * 
 * Sets up the MQTT client with server address and callback.
 * If mqtt_server is not defined in config, uses WiFi gateway address as server.
 * This auto-detection feature means users don't need to configure MQTT broker
 * address if it's running on their gateway.
 */
void init_mqtt() {
    if (reconfig_mode_active)
        return;
    ulog(F("Initializing MQTT..."));
    // mqttClient.connect();
    ////AsyncMqttClient disabled in favor of PubSubClient
    // mqttClient.onConnect(onMqttConnect);
    // mqttClient.onDisconnect(onMqttDisconnect);
    // mqttClient.onSubscribe(onMqttSubscribe);
    // mqttClient.onUnsubscribe(onMqttUnsubscribe);
    // mqttClient.onMessage(onMqttMessage);
    // mqttClient.onPublish(onMqttPublish);
    // mqttClient.setServer(mqtt_server, 1883);
    // mqttClient.setClientId(my_hostname);
    // if (mqtt_user[0]) { // auth given
    //     mqttClient.setCredentials(mqtt_user, mqtt_password);
    // }

    #ifdef MQTT_USE_TLS
        #define mqtt_port 8883
    #else
        #define mqtt_port 1883
    #endif


    #ifdef mqtt_server
        // if defined, just set address
        mqttClient.setServer(mqtt_server, mqtt_port);
        ulog(F("Setting mqtt server to: %s:%d"),  mqtt_server, mqtt_port);
    #else
        // if not defined, take gateway address
        mqtt_server_buffer.from(WiFi.gatewayIP().toString().c_str());
        mqttClient.setServer(mqtt_server_buffer.as_cstr(), mqtt_port);
        ulog(F("Setting mqtt server ip to: %s:%d"),  mqtt_server_buffer.as_cstr(), mqtt_port);
    #endif
    mqttClient.setCallback(onMqttMessage);
}


/**
 * @brief Handle MQTT connection established event
 * 
 * AUTOMATIC MQTT SETUP
 * ====================
 * When MQTT connects, IoTempower automatically:
 * 1. Publishes node's IP address to management topic
 * 2. Publishes Home Assistant discovery messages for all devices
 * 3. Subscribes to command topics for all devices
 * 
 * This automation means users get:
 * - Automatic Home Assistant integration
 * - No manual topic subscription code
 * - Network visibility for debugging
 */
void onMqttConnect() {
    ulog(F("Connected to MQTT."));

    // publish IP on mqtt
    mqttClient.publish((mqtt_management_topic+String("ip")).c_str(),
        WiFi.localIP().toString().c_str(), true); // keep active until next update
    device_manager.publish_discovery_info(mqttClient); // TODO: Check if this is necessary each time or should be somewhere else
    device_manager.subscribe(mqttClient, node_topic);
}


/**
 * @brief Maintain MQTT connection and handle reconnection
 * 
 * ROBUST CONNECTION MANAGEMENT
 * ============================
 * Called every loop iteration to:
 * 1. Process incoming MQTT messages (via mqttClient.loop())
 * 2. Detect disconnections and log the reason
 * 3. Attempt reconnection every 5 seconds if disconnected
 * 4. Re-establish subscriptions after reconnection
 * 
 * This automatic reconnection is crucial for IoT reliability - nodes
 * can recover from temporary network outages without manual intervention.
 */
void maintain_mqtt() {
    if(reconfig_mode_active)
        return;
    if(!wifi_connected)
        return;
    mqttClient.loop();
    if(!mqttClient.connected()) {
        if(mqtt_connected) {
            mqtt_connected = false;
            ulog(F("MQTT: just disconnected. Reason: %d"), mqttClient.state());
        }
        if(millis() - mqtt_last_attempt >= MQTT_RETRY_INTERVAL) { // let's try again
            init_mqtt();
            // for(int i=0; i<32; i++) {
            //     char c = urandom(0,16);
            //     mqtt_id[i] = c>=10?'a'+c:'0'+c;
            // }
            ulog(F("Trying to connect to mqtt server as %s."), my_hostname);
            if(
            #ifdef mqtt_user
                mqttClient.connect(my_hostname, mqtt_user, mqtt_password)
            #else
                mqttClient.connect(my_hostname) // hostname is unique - TODO: can this be done more asynchronously?
            #endif
            ) {
            } else { // mqtt failed
                ulog(F("MQTT connection failed, rc=%d"), mqttClient.state());
            }
            mqtt_last_attempt = millis();
        }
    } else { // mqtt is connected
        if(!mqtt_connected) {
            mqtt_connected = true; // Just became available
            onMqttConnect();
        }
//        mqttClient.loop(); called in beginning
   }
}


/**
 * @brief Initialize WiFi connection
 * 
 * NETWORK SETUP
 * =============
 * Handles complete WiFi setup:
 * 1. Sets hostname for network identification
 * 2. Initializes mDNS for name resolution
 * 3. Starts OTA service
 * 4. Connects to WiFi (credentials from config or adoption mode)
 * 5. Configures TLS/SSL if enabled
 * 
 * The hostname-based identification is important for:
 * - Easy device discovery on the network
 * - OTA updates (connect to hostname.local)
 * - MQTT client identification
 * - Debugging and monitoring
 */
void connectToWifi() {
    // Start WiFi connection and register hostname
    ulog(F("Trying to connect to Wi-Fi with name " WIFI_SSID));
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
    ulog(F("Registering hostname: %s"), my_hostname);
    WiFi.mode(WIFI_STA);

    // Before wifi-start?
    ulog(F("Starting MDNS."));
    MDNS.begin(my_hostname);
    ulog(F("MDNS Ready."));

    // start ota
    ArduinoOTA.begin();
    ulog(F("OTA Ready."));

    if(reconfig_mode_active) {
        ulog(F("Using default wifi credentials in adopt mode."));
        WiFi.begin();
    } else {
        ulog(F("Setting wifi credentials."));
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
    ulog(F("Wifi begin called."));
    
    #ifdef MQTT_USE_TLS

        // load certificate (generated to certificate.h in prepare_build_dir)
        #ifdef ESP32
            // TODO: check if there is a need to modify buffer size on esp32
            // seems not very easy
            netClient.setCACert(mqtt_ca_cert);
        #else
            // decrease the buffer size
            netClient.setBufferSizes(512, 512); // Smallest safe buffers
            netClient.setTrustAnchors(serverTrustedCA);
        #endif
    #endif


    if(WiFi.isConnected()) {
        ulog(F("Already connected to Wi-Fi with IP: %s"), WiFi.localIP());
        wifi_connected = true;
        // init_mqtt(); //AsyncMqttClient disabled in favor of PubSubClient
    }

}

void configureTime() {
    ulog(F("Configuring time..."));
    configTime(0, 0, "pool.ntp.org", "time.nist.gov", "time.windows.com");

    // Wait for NTP synchronization to complete
    ulog("Waiting for NTP synchronization...");
    time_t now = time(nullptr);
    unsigned int seconds = 0;

    while (now < 10000)
    { // Wait for time sync
        delay(1000);
        seconds++;
        ulog(F("Waiting for NTP synchronization... %u seconds"), seconds);
        now = time(nullptr);
    }

    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    ulog(F("Current time: %s"), asctime(&timeinfo));
    ulog(F("Time synchronized after %u seconds."), seconds);
}

void onWifiDisconnect(
////AsyncMqttClient disabled in favor of PubSubClient
//#ifdef ESP32
//    WiFiEvent_t event, WiFiEventInfo_t info
//#else
//    const WiFiEventStationModeDisconnected &event
//#endif
        ) {
    wifi_connected = false;
//    mqtt_just_connected = false;
    Serial.println(F("Disconnected from Wi-Fi."));
//    if(!reconfig_mode_active) {
//        mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while
//                                     // reconnecting to Wi-Fi
//    }
//    wifiReconnectTimer.once(2, connectToWifi);
}


void onWifiConnect(
////AsyncMqttClient disabled in favor of PubSubClient
//#ifdef ESP32
//    WiFiEvent_t event, WiFiEventInfo_t info
//#else
//    const WiFiEventStationModeGotIP &event
//#endif
        ) {
    IPAddress ip_obj = WiFi.localIP();
    ulog(F("Connected to WiFi with IP: %s"), ip_obj.toString().c_str());
    wifi_connected = true;
    //mqtt_connected = false; // trigger reconnect <- should detect automatically
    //init_mqtt();

    // TODO: enable PJON, when UDP works on esp8266 
    // // connect to PJON network
    // pjon_bus.set_shared_network(true);
    // pjon_bus.set_id(ip_obj[3]);
    // bus.strategy.link.set_id(bus.device_id()); // EthernetTCP specific, TODO: remove when UDP works

    // IPAddress mqtt_ip_obj;
    // mqtt_ip_obj.fromString(mqtt_server);
    // uint8_t mqtt_ip[] = {mqtt_ip_obj[0],mqtt_ip_obj[1],mqtt_ip_obj[2],mqtt_ip_obj[3]};
    // //pjon_bus.strategy.add_node(1, mqtt_ip); // UDP version
    // pjon_bus.strategy.link.add_node(1, mqtt_ip); // EthernetTCP version
    // bus.strategy.link.start_listening(); // EthernetTCP specific, TODO: remove when UDP works

    // ulog(F("Connecting to PJON on server %s."), IPAddress(mqtt_ip).toString().c_str()); // TODO: extract this statically in the beginning

    // //bus.set_receiver(receiver_function);
    // pjon_bus.begin();

    // // resend configuration to PJON iot gateway
    // // When coming online we send the general config to the server
    // // hostname - to match to our main topic and find the key we use to communicate
    // // encrypted(nonce+data):
    // //   data=(
    // //     ip4-address, // as 4 bytes
    // //     device1id, // the name of device 1
    // //       subdev1nr, // number of subdevices as byte
    // //       subdevice1id, // subdevice 1 of device 1
    // //       subdevice2id, // subdevice 2 of device 1
    // //       ...
    // //     device2id, // the name of device 2
    // //       subdev2nr, // number of subdevices as byte
    // //       subdevice1id, // subdevice 1 of device 2
    // //       subdevice2id, // subdevice 2 of device 2
    // //       ...
    // //         )

    // // create this config package:
    // Fixed_Buffer b;
    // b.append(strnlen(my_hostname, IOTEMPOWER_MAX_STRLEN), (byte*)my_hostname);

    // b.skip(hydro_secretbox_HEADERBYTES); // space for encryption header

    // for(int i=0; i<4; i++) b.append_byte(ip_obj[i]);

    // device_manager.get_report_list(b);

    // Serial.print("sending via pjon a buffer with length ");
    // Serial.println(b.length());
    // pjon_bus.send_packet(1, b.buffer(), b.length()); // id 1 is gateway

}

////AsyncMqttClient disabled in favor of PubSubClient
// void onMqttConnect(bool sessionPresent) {
//     Serial.println(F("Connected to MQTT."));
//     Serial.print(F("Session present: "));
//     Serial.println(sessionPresent);

//     device_manager.subscribe(mqttClient, node_topic);
//     mqtt_just_connected = true;
// }
//
// void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
//     Serial.println(F("Disconnected from MQTT."));
//
//     mqtt_just_connected = false;
//
//     if (WiFi.isConnected()) {
//         mqttReconnectTimer.once(2, init_mqtt);
//     }
// }
//
// void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
//     Serial.print(F("Subscribe acknowledged."));
//     Serial.print(F("  packetId: "));
//     Serial.print(packetId);
//     Serial.print(F("  qos: "));
//     Serial.println(qos);
// }

// void onMqttUnsubscribe(uint16_t packetId) {
//     Serial.print(F("Unsubscribe acknowledged."));
//     Serial.print(F("  packetId: "));
//     Serial.println(packetId);
// }

// void onMqttMessage(char *topic, char *payload,
//                    AsyncMqttClientMessageProperties properties, size_t len,
//                    size_t index, size_t total) {
//     Serial.print(F("Publish received."));
//     Serial.print(F("  topic: "));
//     Serial.print(topic);
//     Serial.print(F("  qos: "));
//     Serial.print(properties.qos);
//     Serial.print(F("  dup: "));
//     Serial.print(properties.dup);
//     Serial.print(F("  retain: "));
//     Serial.print(properties.retain);
//     Serial.print(F("  len: "));
//     Serial.print(len);
//     Serial.print(F("  index: "));
//     Serial.print(index);
//     Serial.print(F("  total: "));
//     Serial.print(total);

//     Ustring utopic(topic);
//     utopic.remove(0, node_topic.length() + 1);
//     Ustring upayload(payload, (unsigned int)total);

//     Serial.print(F(" payload: >"));
//     Serial.print(upayload.as_cstr());
//     Serial.println(F("<"));
    
//     device_manager.receive(utopic, upayload);
// }

// void onMqttPublish(uint16_t packetId) {
//     Serial.println(F("Publish acknowledged."));
//     Serial.print(F("  packetId: "));
//     Serial.println(packetId);
// }


// __attribute__((constructor)) void early_init() {
//  //   ulog(F("Free memory: %ld"),ESP.getFreeHeap());
//     ulog();
//     ulog();
//     ulog();
// }

/**
 * @brief Arduino setup function - runs once at boot
 * 
 * STARTUP SEQUENCE
 * ================
 * Important: Device objects are constructed BEFORE setup() runs due to C++ 
 * static initialization. This is why devices can register themselves with
 * the DeviceManager during construction.
 * 
 * Setup sequence:
 * 1. Disable brownout detector (ESP32) - prevents spurious resets
 * 2. Initialize serial communication for logging
 * 3. Configure onboard LED
 * 4. Initialize random number generator
 * 5. Check for adoption mode request (flash button)
 * 6. Configure WiFi sleep mode
 * 7. Setup OTA system
 * 8. Connect to WiFi
 * 9. Configure NTP time (if TLS enabled)
 * 10. Call user iotempower_init() function
 * 11. Start all registered devices (calls device.start() on each)
 * 12. Call user iotempower_start() function
 * 
 * This sequence ensures everything is ready before devices start operating.
 */
void setup() {
    // Careful - devices are initialized before this due to class constructors
    // TODO: setup (another, the internal one seems quite ok) watchdog
    // TODO: consider not using serial at all and freeing it for other
    // connections, and offering other debug channels
    #ifdef ESP32
        #ifdef BROWNOUT_DETECT_DISABLED
        WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector TODO: verify that this reduces crashes
        #endif
    #endif

    initialize_serial();

    ulog(F("Booting."));

    #ifdef ID_LED
        pinMode(ID_LED, OUTPUT); // initialize a potential onboardled // TODO: check if it's inverted
    #endif

    // not necessary to initialize randomness, if not seeded
    ulog(F("Random generator check, testnumber: %lu"), random(INT32_MAX));

    flash_mode_select();

    #ifdef ESP32
    // TODO: anything equivalent for ESP32 necessary?
    #else
    WiFi.setSleepMode(WIFI_NONE_SLEEP); // TODO: check if this works -> for better rgb-strip-smoothness - should be obsolete when using neopixelbus
    #endif

    setup_ota();

//    WiFi.disconnect(true);
//    delay(10);

    ////AsyncMqttClient disabled in favor of PubSubClient
    // // Try already to bring up WiFi
    // #ifdef ESP32    
    // WiFi.onEvent(onWifiConnect, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
    // WiFi.onEvent(onWifiDisconnect, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
    // #else
    // wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    // wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
    // #endif

    connectToWifi();

    #ifdef MQTT_USE_TLS 
        // Needed for certificate expiry validation to work
        configureTime();
    #endif

    if (!reconfig_mode_active) {
        mqtt_management_topic = String(F(IOTEMPOWER)) + String(F("/_cfg_/")) 
                                + String(F(HOSTNAME)) + String(F("/"));

        if(iotempower_init) iotempower_init(); // call user init from setup.cpp
        device_manager.start(); // call start of all devices
        if(iotempower_start) iotempower_start(); // call user start from setup.cpp
        // init_mqtt(); //AsyncMqttClient disabled in favor of PubSubClient
    } else {  // do something to show that we are in adopt mode
        #ifdef ID_LED
            // enable flashing that light
            if(adopt_flash_toggle) {
                onboard_led_off();
            } else {
                onboard_led_on();
            }
        #endif
    }

}

// global variables for loop
static unsigned long last_transmission = millis();
static unsigned long last_published = millis();
static unsigned long last_micros = micros();
static unsigned long last_measured = millis();
static unsigned long transmission_delta = 5000;
static unsigned long current_micros = micros();
static unsigned long current_time = millis();
static unsigned long precision_delta = 0;
static unsigned long unprecision_delta = 1000;
bool in_precision_interval = false;

void transmission_interval(int interval) {
    transmission_delta = ((unsigned long)interval) * 1000;
}

void set_precision_interval(long interval_us, long unprecision_interval_us=-1) {
    if(unprecision_interval_us<0) {
        unprecision_interval_us = interval_us*2;
    }
    // if called several times, pick always the biggest
    if(precision_delta < interval_us)
        precision_delta = interval_us;
    if(unprecision_delta < unprecision_interval_us)
        unprecision_delta = unprecision_interval_us;
    ulog(F("precision_delta: %lu, unprecision_delta: %lu"), precision_delta, unprecision_delta);
}

// Variables for performance metrics
unsigned long performance_last_reset_time = 0;
const unsigned long performance_interval = 5000; // Interval of 5 seconds in milliseconds
unsigned long performance_iteration_count = 0;
double performance_total_execution_time = 0; // Total execution time in microseconds

/**
 * @brief Arduino main loop - runs continuously
 * 
 * MAIN LOOP OPERATION
 * ===================
 * This is where IoTempower's cooperative multitasking happens. The loop manages:
 * 
 * 1. PRECISION vs REGULAR INTERVALS
 *    - Precision interval: Very tight timing for sensitive devices (e.g., power meters)
 *    - Regular interval: Normal operation with yields for network/system tasks
 *    - Only precision devices update during precision intervals
 *    - Everything else happens during regular intervals
 * 
 * 2. DEVICE UPDATES
 *    - device_manager.update() polls all devices based on their poll rates
 *    - Each device checks if it's time to measure
 *    - Measurements trigger change detection
 *    - Changes mark devices for publishing
 * 
 * 3. NETWORK MANAGEMENT
 *    - WiFi connection monitoring with automatic recovery
 *    - MQTT connection maintenance and reconnection
 *    - OTA update handling
 * 
 * 4. PUBLISHING
 *    - Changed values published as soon as possible
 *    - Full state reports on configurable interval (transmission_delta)
 *    - Rate limiting to prevent network buffer overflow (MIN_PUBLISH_TIME_MS)
 * 
 * 5. SCHEDULER
 *    - do_later_check() executes scheduled callbacks
 *    - Supports delayed actions, debouncing, deep sleep scheduling
 * 
 * KEY IOTEMPOWER FEATURES IN THE LOOP:
 * - Automatic device polling and measurement
 * - Automatic change detection and publishing
 * - Robust network connection management
 * - No blocking operations (cooperative multitasking)
 * - Minimal user code needed (most logic is automatic)
 * 
 * Compare this to a standard Arduino MQTT project where you would need to:
 * - Manually call every sensor's read function at the right time
 * - Manually check for value changes
 * - Manually build and send MQTT messages
 * - Manually handle reconnections
 * - Write all the timing and scheduling logic yourself
 */
void loop() {
    //yield(); // do necessary things for  maintaining WiFi and other system tasks
    // yield at start of loop doesn't make sense by its definition

    current_time = millis();
    current_micros = micros();

    // Start performace timing this iteration
    unsigned long performance_iteration_start_time = current_micros;


    // TODO: make sure nothing weird happens -> watchdog
    //MDNS.update(); TODO: needed???
    if (!reconfig_mode_active) { // not in reconfig mode -> normal mode after boot
        in_precision_interval = (current_micros - last_micros) < precision_delta;
        // update physical connections as often as possible (they have their own pollrate built in)
        device_manager.update(in_precision_interval);
        if(!in_precision_interval) {
            current_time = millis(); // device update might have taken time, so update (if in precision interval they shoudl barely use time)
            ////AsyncMqttClient disabled in favor of PubSubClient
            // if(mqtt_just_connected) {
            //     mqtt_just_connected = ! // only set to true when publish successful
            //         device_manager.publish_discovery_info(mqttClient); // needs to happen here not to collide with other publishes
            // }
            
            // TODO: enable PJON, when UDP works on esp8266 
            // // handle pjon communication
            // pjon_bus.update();
            // //ulog(F("Packages left: %d"), pjon_bus.update());
            // pjon_bus.receive();

            ArduinoOTA.handle(); // check for firmware update requests

            do_later_check(); // work the scheduler

            // Network connection maintainance
            if(WiFi.status() != WL_CONNECTED) { /// No WiFi connection
                if(wifi_connected) { // It was before
                    onWifiDisconnect();
                }
            } else { // WiFi connection exists
                if(!wifi_connected) { // It was not connected before
                    onWifiConnect();
                }
                maintain_mqtt();
            } // end WiFi exists

            // sent out new values if measured (and time isn't too low)
            if (mqttClient.connected()) {
                // careful, we are not reading any new sensor data during the publishing process
                // That means, if we have sensors that need to monitor data over time
                if (current_time - last_published >= MIN_PUBLISH_TIME_MS) {
                    // go through devices and send reports if necessary
                    if (transmission_delta > 0 &&
                        current_time - last_transmission >=
                            transmission_delta) {
                        if (device_manager.publish(mqttClient, node_topic, true)) {
                            ulog(F("Free memory: %ld"),ESP.getFreeHeap());
                            last_transmission = current_time;
                            last_published = current_time;
                        }
                    } else { // no full transmission necessary
                        if (device_manager.publish(mqttClient, node_topic, false)) {
                            last_published = current_time;
                        }
                    } // endif transmission delta
                } // endif update delay
            } else {
                // ulog(F("Trouble connecting to mqtt server."));
                // Don't block here with delay as other processes might
                // be running in background
                // TODO: wait a bit before trying to reconnect.
            } // endif mqtt connect
            // mqtt_client->yield(_loop_delay);
            // iotempower_loop(); // TODO: think if this can actually be
            // skipped in the iotempower-philosophy -> maybe move to driver
            // callbacks
        } // endif in_precision_interval
        current_micros = micros();
        if(current_micros - last_micros > precision_delta + unprecision_delta){
            last_micros = current_micros;
            device_manager.reset_buffers();
        }
    } else { // reconfig mode is active
        ArduinoOTA.handle(); // check for firmware update requests

        #ifdef ID_LED
        // flashing very rapidly
        if (wifi_connected && current_time - last_measured >= 80) {
            adopt_flash_toggle = !adopt_flash_toggle;
            if(adopt_flash_toggle) {
                onboard_led_off();
            } else {
                onboard_led_on();
            }
            last_measured = current_time;
        }
        #endif
    } // endif !reconfig_mode_active

    // End performance timing this iteration
    unsigned long performance_iteration_end_time = micros();
    double performance_execution_time = performance_iteration_end_time - performance_iteration_start_time;
    performance_total_execution_time += performance_execution_time;
    performance_iteration_count++;

    // Check if 5 seconds for performace have passed
    if (millis() - performance_last_reset_time >= performance_interval) {
        double performance_average_execution_time = performance_total_execution_time / performance_iteration_count;
        double performance_average_calls_per_second = performance_iteration_count / 5.0; // Dividing by 5 seconds directly

        // Display the results
        Serial.print("Average Execution Time (us): ");
        Serial.println(performance_average_execution_time);
        Serial.print("Average Calls Per Second: ");
        Serial.println(performance_average_calls_per_second);

        // Reset for the next interval
        performance_last_reset_time = millis();
        performance_iteration_count = 0;
        performance_total_execution_time = 0;
    }
}
