

#define PIN_VOL_MIN 3100
#define PIN_VOL_MAX 4100
#define R1 20000  // Điện trở R1 (đơn vị: Ω)
#define R2 4700   // Điện trở R2 (đơn vị: Ω)
#define VOLTAGE_DIVIDER_RATIO ((R1 + R2) / (float)R2)
void blink_led() {
  Time++;
  // if (digitalRead(LED) == HIGH) {
  //   digitalWrite(LED, LOW);
  // } else digitalWrite(LED, HIGH);
  // Đọc giá trị ADC (trong phạm vi 0 - 1023)
  Serial.printf_P(PSTR("Free mem=%d, Time=%d\n"), ESP.getFreeHeap(), Time);
}
const char* get_info() {
  static char jsonBuffer[512];
  doc.clear();

  // Thông tin bên ngoài
  doc["device_id"] = SN;

  // message wrapper
  JsonObject message = doc.createNestedObject("message");
  message["cmd"] = "get_device_info";

  // data object
  JsonObject data = message.createNestedObject("data");
  data["WF_LV"] = WiFi.RSSI();
  data["IP"] = WiFi.localIP().toString();  // hoặc dùng MAC nếu bạn muốn đúng format
  data["TIME"] = get_time();               // Định dạng ISO "2025-03-18T14:03:54.172Z"
  data["MODEL"] = MODEL;                   // Hoặc dùng biến MODEL nếu có
  data["FW_VER"] = FW_VERSION;
  unsigned long bootSeconds = millis() / 1000;
  int days = bootSeconds / 86400;
  int hours = (bootSeconds % 86400) / 3600;
  int minutes = (bootSeconds % 3600) / 60;
  int seconds = bootSeconds % 60;
  char bootTimeFormatted[64];
  sprintf(bootTimeFormatted, "%d day %02d:%02d:%02d", days, hours, minutes, seconds);
  // Serial.println(bootTimeFormatted);
  data["BOOT_TIME"] = bootTimeFormatted;
  //data["BOOT_TIME"] = millis() / 1000;  // Hoặc Time nếu bạn đang đếm từ lúc boot
  data["MQTT_COUNT"] = mqtt_count;
  // data["RSP"] = "GET_INFO";
  data["MAC"] = WiFi.macAddress();
  data["MQTT_COUNT"] = mqtt_count;
  data["MQTT_LOST"] = mqtt_lost;
  data["FREE_MEM"] = ESP.getFreeHeap();

  // Nested object: SETTING
  JsonObject setting = data.createNestedObject("SETTING");
  // setting["HANDLE"] = "AUTO";
  if (check_wifi) {
    setting["SSID"] = "NULL";
  } else {
    setting["SSID"] = esid;
  }


  // Ghi JSON ra buffer
  memset(jsonBuffer, 0, sizeof(jsonBuffer));
  serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));

  Serial.println("JSON reply:");
  Serial.println(jsonBuffer);

  return jsonBuffer;
}
const char* get_state() {
  static char jsonBuffer[256];
  doc.clear();
  doc["cmd"] = "get_state";
  doc["device_id"] = SN;

  JsonArray data = doc.createNestedArray("data");
  for (int i = 0; i < RELAY_COUNT; i++) {
    JsonObject item = data.createNestedObject();
    item["channel"] = i + 1;
    item["state"] = digitalRead(RELAY_PINS[i]) ? 1 : 0;
  }

  serializeJson(doc, jsonBuffer);
  return (jsonBuffer);
  Serial.println("Reported relay states.");
}
