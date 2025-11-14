#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <LittleFS.h>
#include <SimpleTimer.h>
#include <EEPROM.h>
#include "main.h"
#include <ArduinoJson.h>
//#define SSL_EN
SimpleTimer timer;
ESP8266WebServer server(80);
DNSServer dnsServer;
IPAddress apIP(192, 168, 1, 1);
const byte DNS_PORT = 53;
ESP8266HTTPUpdateServer httpUpdater;
const char default_SSID[] PROGMEM = "Loapay";
const char default_PASS[] PROGMEM = "0904110111a";
const char FW_VERSION[] PROGMEM = "-SMSFW-V01.01";
const char password[] PROGMEM = "0904110111a";
const char TOPIC_NOTIFY[] PROGMEM = "NOTIFY";

char SN[13];            // Serial Number (SN)
char topic_remote[32];  // Topic MQTT cho remote
char topic_rec[32];     // Topic MQTT cho nhận dữ liệu
const char *host = "loa";
const char *update_path = "/update";
#define username_device "admin"  //#define username_device "ch83nth1ng"
#define pass_device "admin"      //#define pass_device "CH83nth1NG@8698"
String st;


unsigned long Time, mqtt_count, trans_count, mqtt_lost;
bool AP_MODE;
unsigned long previousMillis = 0;
unsigned long lastReconnectAttempt = 0;
bool wifi_mode = 0;
volatile int volume = 1;
char esid[62] = { 0 };        // Mảng lưu SSID (31 ký tự + null)
char epass[62] = { 0 };       // Mảng lưu Password (31 ký tự + null)
StaticJsonDocument<512> doc;  // Use a StaticJsonDocument with adequate size
#ifdef SSL_EN
BearSSL::WiFiClientSecure espClient;
#else
WiFiClient espClient;
#endif
PubSubClient client(espClient);

#define TOPIC_OFFLINE "DEVICE_OFFLINE"
#define TOPIC_ONLINE "DEVICE_INFO"
#define MAX_BUFFER_SIZE 512
float voltagePin;
bool check_wifi = false, pin_low = false;


void setup() {
  Serial.begin(115200);
  LittleFS.begin();
  //Serial.setDebugOutput(true);
  // audioLogger = &Serial;
  pinMode(LED, OUTPUT);
  pinMode(BUTTON1,INPUT_PULLUP);
  pinMode(BUTTON2,INPUT_PULLUP);
  for(int i=0;i<2;i++)
  {
    pinMode(RELAY_PINS[i],OUTPUT);
  }
  eeprom_setup();
  if (wifi_mode) reconnect();
  if (client.connected()) {
    mcp_init();
    timer.setInterval(1000l, blink_led);
  } else {
    timer.setInterval(2000L, blink_led);
  }
   digitalWrite(LED,  HIGH);
  //wifi_set_sleep_type(LIGHT_SLEEP_T);
}
void loop() {
  check_phim();
 // if (AP_MODE == 1) {
    dnsServer.processNextRequest();
    MDNS.update();
    server.handleClient();
 // }
  timer.run();
  if (wifi_mode) {
    if (!client.connected()) {
      unsigned long now = millis();
      if (now - lastReconnectAttempt > 10000) {
        lastReconnectAttempt = now;
        // Attempt to reconnect
        if (reconnect()) {
          mqtt_count++;
          lastReconnectAttempt = 0;
        } else {
          mqtt_lost++;
          if (mqtt_lost > 4) {
            Serial.println("mat ket noi MQTT qua 5 lan, restart");
            delay(500);
            ESP.restart();
          }
        }
      }
    } else {
      // Client connected

      client.loop();
      mcp_loop();
    }
  }
}