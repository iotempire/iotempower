// main.cpp
// main program for ulnoiot esp8266 firmware

////// Standard libraries
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <ESP8266TrueRandom.h>
#include <PubSubClient.h>

////// adapted libraries
#include <WiFiManager.h>

//// configuration
#include <toolbox.h>
#include <ulnoiot-default.h>
#include <device-manager.h>

// node specific code
//#include "wifi-config.h" now obsolete and pre-configured
#include "config.h"
#include "key.h"
#include "pins.h"

// ulnoiot functions for user modification in setup.cpp
void ulnoiot_setup();
//void ulnoiot_loop();

int long_blinks=0, short_blinks;

#define ID_LED LED_BUILTIN
#define BLINK_LONG 800
#define BLINK_SHORT 200
#define BLINK_OFF 500

void id_blinker() {
  static int total, pos;
  static unsigned long lasttime;
  unsigned long currenttime;
  int delta;

  if(long_blinks == 0) { //first time, still unitialized
    // randomness for 25 different blink patterns (5*5)
    long_blinks = ESP8266TrueRandom.random(1,6);
    short_blinks = ESP8266TrueRandom.random(1,6);
    Serial.printf("Blink pattern: %d long_blinks, %d short_blinks\n",long_blinks,short_blinks);
    total = long_blinks * (BLINK_LONG + BLINK_OFF) 
            + short_blinks * (BLINK_SHORT + BLINK_OFF);
    lasttime = millis();
    pos = 0;
    pinMode(ID_LED, OUTPUT);
    digitalWrite(ID_LED,0); // on - start with a long blink
  }
  // compute where in blink pattern we currently are
  currenttime = millis();
  delta = currenttime - lasttime;
  pos = (pos + delta) % total;
  if(pos < long_blinks * (BLINK_LONG + BLINK_OFF)) { // in long blink phase
    if(pos%(BLINK_LONG + BLINK_OFF) < BLINK_LONG) {
      digitalWrite(ID_LED,0); // on - in blink phase
    } else {
      digitalWrite(ID_LED,1); // off - in off phase
    }
  } else { // in short blink phase
    if((pos-long_blinks * (BLINK_LONG + BLINK_OFF)) % (BLINK_SHORT + BLINK_OFF) < BLINK_SHORT) {
      digitalWrite(ID_LED,0); // on - in blink phase
    } else {
      digitalWrite(ID_LED,1); // off - in off phase
    }
  }
  lasttime = currenttime;
}

void reconfigMode() {
  // go to access-point and reconfiguration mode
  
  char *ap_ssid = (char *) (ULNOIOT_AP_RECONFIG_NAME "-xxxxxx");
  const char *ap_password = ULNOIOT_AP_RECONFIG_PASSWORD;

  Serial.println("Reconfiguration requested. Activating AP-mode.");
  WiFi.disconnect(true);
  sprintf(ap_ssid+strlen(ap_ssid)-6,"%06x",ESP.getChipId());
  Serial.printf("Connect to %s with password %s.\n", ap_ssid, ap_password);
  WiFiManager wifiManager;
  wifiManager.setConnectTimeout(300); // 5 min timeout
  
  // // parameter test
  // WiFiManagerParameter test_param("tparam", "test parameter", "123", 5);
  // wifiManager.addParameter(&test_param);
  id_blinker(); // TODO -> only call random init part;
  String blink_pattern = "<p>Blink pattern: " + String(long_blinks)
            + " longs, " + String(short_blinks) + " shorts</p>";
  WiFiManagerParameter custom_text(blink_pattern.c_str());
  wifiManager.addParameter(&custom_text);
//  const char * menu[] = {"wifi","param","sep","exit"};
//  wifiManager.setMenu(menu,4);
//  wifiManager.setShowInfoErase(true); // disable info-field

  // make sure, we head directly to wifi config, when configuration mode activated
  wifiManager.setCaptivePortalWifiRedirect(true); 

  wifiManager.setConfigPortalBlocking(false); // allow interrupts
  // wifiManager.autoConnect(ap_ssid, ap_password);
  wifiManager.startConfigPortal(ap_ssid, ap_password);


  while(1) {
    if(wifiManager.process()) break;
    id_blinker();
    yield();
  }
  wifiManager.stopWebPortal();
  WiFi.softAPdisconnect(false); // stop accesspoint mode
  if(wifiManager.getLastConxResult() == WL_CONNECTED) { // there was a connection made
    // trigger reconfigurability (flash with default password) after next reboot
    Serial.println("Requesting password reset on next boot.");
    int magicSize = sizeof(ULNOIOT_RECONFIG_MAGIC);
    char rtcData[magicSize];
    strncpy(rtcData, ULNOIOT_RECONFIG_MAGIC, magicSize);
    ESP.rtcUserMemoryWrite(0, (uint32_t *) rtcData, magicSize);
  } // TODO: consider going back to configuration mode if not successful
  reboot(); // Always reboot after this to free all eventually not freed memory used by WiFiManager
}

void flash_mode_select() {
    // Check if flash with default password is requested
  int magicSize = sizeof(ULNOIOT_RECONFIG_MAGIC);
  char rtcData[magicSize];
  rtcData[magicSize]=0;
  char nothing[1] = {0};
  ESP.rtcUserMemoryRead(0, (uint32_t*) rtcData, magicSize);
  if(memcmp(rtcData,ULNOIOT_RECONFIG_MAGIC,magicSize) == 0) {
    const char* flash_default_password = ULNOIOT_FLASH_DEFAULT_PASSWORD;
    Serial.printf("Setting flash default password to %s.\n", flash_default_password);
    ArduinoOTA.setPassword(flash_default_password);
    ESP.rtcUserMemoryWrite(0, (uint32_t*) nothing, 1 ); // only reset password once
  } else { // do not check for special config mode, when just rebooted out of it
  
    // Check specific GPIO port if pressed and unpressed 2-4 times to enter
    // reconfiguration mode (allow generic reflash in AP mode)
    Serial.println("Allow 5s to check if reconfiguration and AP mode is requested.");
    int last=1, toggles=0;
    // blink a bit to show that we have booted and waiting for potential input
    pinMode(LED_BUILTIN,OUTPUT);
    pinMode(0,INPUT_PULLUP); // check flash button (d3 on wemos and nodemcu)
    for( int i=0; i<500; i++) {
      digitalWrite(LED_BUILTIN, (i/25)%2);
      int new_state=digitalRead(0);
      if( new_state != last) {
        last=new_state;
        toggles ++;
      }
      delay(10);
    }

    if(toggles>=4 && toggles<=8) { // was pressed 2-4 times
      reconfigMode();
    }

    Serial.println("Continue to boot normally.");
    // Password can be set with it's md5 value as well
    ArduinoOTA.setPasswordHash(keyhash);

  } // endif default password flash mode
}

WiFiClient wifiClient;
PubSubClient client;
Ustring node_topic(mqtt_topic);

void mqtt_receive_callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Receiving on topic ");
  Serial.print(topic);
  Serial.print(" >");
  Ustring upayload;
  upayload.from(payload, length);
  Serial.print(upayload.as_cstr());
  Serial.println("<");
}

void setup() {
  // TODO: setup watchdog
  // TODO: consider not using serial at all and freeing it for other connections
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting. Serial port initialized.");

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
  // Try already to bring up WiFi
  WiFi.mode(WIFI_STA);
  //WiFi.begin(wifi_ssid, wifi_password);
  WiFi.begin(); // need to be set in advance once TODO: think about initialization -> probably just upload a program that does the preset

  flash_mode_select();

  if(WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    reboot();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // TODO: only enter OTA when requested

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(hostname);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // TODO: check if port is configurable

  client = PubSubClient(mqtt_server, 1883, mqtt_receive_callback, wifiClient);

  ulnoiot_setup(); // define all devices
}

boolean mqtt_connect() {
  if(!client.connected()) {
    bool connected;
    // reconnect
    if(mqtt_user[0]) { // auth given
      connected = client.connect(hostname,mqtt_user,mqtt_password);
    } else {
      connected = client.connect(hostname);
    }
    if(connected) {
      devices_subscribe(client, node_topic);
    }
    return connected;
  }
  return true;
}

static unsigned long last_transmission = millis();
static unsigned long transmission_delta = 5000;
void transmission_interval(int interval) {
    transmission_delta = ((unsigned long)interval) * 1000;
}

static int _loop_delay=1;
void loop_delay(int delay) {
  // TODO: should 0 be allowed here?
  _loop_delay = delay;
}

void loop() {
  unsigned long current_time;
  // TODO: make sure nothing weird happens -> watchdog
  ArduinoOTA.handle();
  if(mqtt_connect()) {
    client.loop(); // give time to mqtt to execute callback
    current_time = millis();
    if(devices_update() || 
        (transmission_delta > 0 &&
          current_time - last_transmission >= transmission_delta)) {
      // go through devices and send reports if necessary
      devices_publish(client, node_topic); // TODO: check error status
      last_transmission = current_time;
    }
  } else {
    Serial.println("Trouble connecting to mqtt server.");
    // Don't block here with delay as other processes might be running in background
    // TODO: wait a bit before trying to reconnect.
  }
  delay(_loop_delay);
  //ulnoiot_loop(); // TODO: think if this can actually be skipped in the ulnoiot-philosophy -> maybe move to driver callbacks
}
