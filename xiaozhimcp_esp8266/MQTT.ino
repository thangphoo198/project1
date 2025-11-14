#include <map>
#define EEPROM_ADDR_SN 100  // Địa chỉ bắt đầu lưu SN
const char default_MQTTSERVER[] PROGMEM = "mqtt.jvm.io.vn";
const char default_MQTTUSER[] PROGMEM = "ivoice";
const char default_MQTTPASS[] PROGMEM = "Hoang1Qaz@123";
// const char default_MQTTSERVER[] PROGMEM = "mqtt-uat.tingee.vn";
// const char default_MQTTUSER[] PROGMEM = "loapay";
// const char default_MQTTPASS[] PROGMEM = "yFQ16EBUAeqrytm";
// const int default_MQTTPORT = 1883;

// const char default_MQTTSERVER[] PROGMEM = "mqtt.tingee.vn";
// const char default_MQTTUSER[] PROGMEM = "loapay";
// const char default_MQTTPASS[] PROGMEM = "0ail4xJRAfXa312w";
const int default_MQTTPORT = 1883;

bool MqttSubscribeLib(const char *topic) {

  bool t = client.subscribe(topic);  // USE_MQTT_AZURE_IOT
  client.loop();                     // Solve LmacRxBlk:1 messages
  return t;
}

#define TOPIC_UPDATE "DEVICE_UPDATE"
void set_mqtt() {
  client.setServer(default_MQTTSERVER, default_MQTTPORT);
  client.setCallback(callback);
  client.setBufferSize(MAX_BUFFER_SIZE);
}

boolean reconnect() {
  set_mqtt();
  // if (wifi_mode = 1) {
#ifdef SSL_EN
  setManualTime();
  espClient.setInsecure();

#endif
  if (!client.connected()) {
    char client_id[32];
    snprintf(client_id, sizeof(client_id), "LOAPAY-CLIENT-ID%s%x", SN, random(0, analogRead(A0)));
    Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id);
    static char x[50] = "";
    sprintf(x, "{\"ID\":\"%s\",\"RSP\":\"OFFLINE\"}", SN);
    // if (client.connect(client_id, default_MQTTUSER, default_MQTTPASS)) {
    if (client.connect(client_id, default_MQTTUSER, default_MQTTPASS, TOPIC_OFFLINE, 0, false, x)) {
      // Serial.printf("Connected to MQTT broker, topic %s, %s\r\n", topic_remote.c_str(), topic_rec.c_str());
      // client.setBufferSize(MAX_BUFFER_SIZE);
      client.setKeepAlive(60);
      MqttSubscribeLib(topic_remote);
      MqttSubscribeLib(TOPIC_UPDATE);
      client.publish(topic_rec, get_info());
      //PlayMP3File(maychu_ok);
    } else {
#ifdef SSL_EN
      char err_buf[64];
      espClient.getLastSSLError(err_buf, sizeof(err_buf));
      Serial.print("SSL error: ");
      Serial.println(err_buf);
#endif
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.println(client.state());
      //delay(5000);
    }
    // }
  }
  return client.connected();
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Có tin nhắn mới từ topic: ");
  Serial.println(topic);

  const size_t buffSize = 256;
  if (length >= buffSize) {
    Serial.println("Error: Payload quá lớn");
    return;
  }

  // Sử dụng memcpy để copy payload, đảm bảo null termination
  char buff[buffSize] = { 0 };
  memcpy(buff, payload, length);
  buff[length] = '\0';

  if (strlen(buff) > 2) {
    Serial.println(buff);
    xuly(String(buff));  // Chuyển xử lý JSON sang hàm xuly()
  }
}

void xuly(String js) {
  doc.clear();
  char buff[256] = { 0 };

  DeserializationError error = deserializeJson(doc, js);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  String cmd = doc["cmd"].as<String>();
  String device_id = doc["device_id"] | SN;

  // Xử lý lệnh bật/tắt relay
  if (cmd == "set_state") {
    int channel = doc["channel"].as<int>();
    int state = doc["state"].as<int>();

    if (channel >= 1 && channel <= RELAY_COUNT) {
      digitalWrite(RELAY_PINS[channel - 1], state ? HIGH : LOW);
      Serial.printf("Relay %d -> %s\n", channel, state ? "ON" : "OFF");

      // Gửi phản hồi trạng thái về app
      snprintf(buff, sizeof(buff),
               "{\"cmd\":\"report\",\"device_id\":\"%s\",\"data\":[{\"channel\":%d,\"state\":%d}]}",
               SN, channel, state);
      client.publish(topic_rec, buff);
    } else {
      Serial.println("Invalid relay channel!");
    }
  }

  // Xử lý yêu cầu lấy trạng thái
  else if (cmd == "get_state") {
    client.publish(topic_rec, get_state());
  } else if (cmd == "get_device_info") {
    client.publish(topic_rec, get_info());

  } else if (cmd == "get_mcp_link") {
    client.publish(topic_rec, loadMcpLink().c_str());

  }

  else {
    Serial.println("Unknown command!");
  }
}
