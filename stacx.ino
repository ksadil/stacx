#if defined(ESP8266)
#include <FS.h> // must be first
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#define USE_NTP
#ifdef USE_NTP
#include <TimeLib.h>
#include <NtpClientLib.h>
#endif
#define helloPin D4
#else // ESP32
#include <SPIFFS.h>
#include <WiFi.h>          //https://github.com/esp8266/Arduino
#include <WebServer.h>
#include <ESPmDNS.h>
#endif

#include <Arduino.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>
#include <time.h>

#define SYSLOG_flag wifiConnected
#define SYSLOG_host mqtt_server
#define DEBUG_LEVEL L_DEBUG

#include "config.h"
#include "accelerando_trace.h"

//@******************************* constants *********************************
// you can override these by defining them in config.h

#ifndef HEARTBEAT_INTERVAL_SECONDS
#define HEARTBEAT_INTERVAL_SECONDS (5*60)
#endif

#ifndef NETWORK_RECONNECT_SECONDS 
#define NETWORK_RECONNECT_SECONDS 5
#endif

#ifndef MQTT_RECONNECT_SECONDS
#define MQTT_RECONNECT_SECONDS 10
#endif

//@******************************* variables *********************************


//@********************************* leaves **********************************

#include "wifi.h"
#include "leaf.h"
#include "mqtt.h"
#include "leaves.h"

//
//@********************************* setup ***********************************

void setup(void)
{
  // 
  // Set up the serial port for diagnostic trace
  // 
  Serial.begin(115200);
  Serial.println("\n\n\n");
  Serial.println("Accelerando.io Multipurpose IoT Backplane");

  NOTICE("ESP Wakeup reason: %s", ESP.getResetReason().c_str());

  // 
  // Set up the WiFi connection and MQTT client
  // 
  wifi_setup();
  mqtt_setup();

  // 
  // Set up the IO leaves
  //
#ifdef helloPin
  pinMode(helloPin, OUTPUT);
#endif  

  for (int i=0; leaves[i]; i++) {
    NOTICE("Leaf %d: %s", i+1, leaves[i]->describe().c_str());
    leaves[i]->setup();
  }
  
  ALERT("Setup complete");
}

//
//@********************************** loop ***********************************

void loop(void)
{  
  unsigned long now = millis();

#ifdef helloPin
  static int hello = 0;

  int pos = now % blink_rate;
  int flip = blink_rate * blink_duty / 100;
  int led = (pos > flip);
  if (led != hello) {
    hello = led;
    digitalWrite(helloPin, hello);
  }
#endif
  
  // 
  // Handle network Events
  // 
  wifi_loop();
  mqtt_loop();

  // 
  // Handle Leaf events
  // 
  for (int i=0; leaves[i]; i++) {
    leaves[i]->loop();
  }
}

// Local Variables:
// mode: C++
// c-basic-offset: 2
// End: