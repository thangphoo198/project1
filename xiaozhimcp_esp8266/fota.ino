

#include <Arduino.h>

#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}


void update_finished() {
  Serial.println("FOTA finished");
  //  uint16_t msg_len = 50;
  //  char *buff = (char *)malloc(msg_len);
  //  sprintf(buff, "{\"ID\":\"%s\",\"STT\":\"OFFLINE\"}", chip_ID.c_str());
  //  client.publish("DEVICE_OFFLINE", buff);
  //  free(buff);
  //  delay(200);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  //delay(1000);  // Wait a second and restart
  ESP.restart();
  //esp_deep_sleep_start();
}

void update_progress(int cur, int total) {
  float proc = (cur * 100.00) / total;
  int proc_int = (int)proc;
  Serial.printf("update FOTA  %d\n", proc_int);
  //  if(int(proc)%10==0)
  //  {

  //  char buf[50]="";
  //  sprintf(buf,"{\"EVENT\":\"UPDATE_FW\",\"PERCENT\":%d}",proc_int);
  //  Serial.println(buf);
  //  client.publish(topic_rec.c_str(), buf);
  //}
}

void update_error(int err) {
  Serial.printf("update FOTA fatal error code %d\n", err);
}
void disconnectMQTT() {
  if (client.connected()) {
    Serial.println(F("Disconnecting MQTT..."));
    client.disconnect();  // Ngắt kết nối MQTT
  }

  Serial.println(F("Freeing MQTT memory..."));
  client.setCallback(nullptr);  // Loại bỏ callback

#ifdef SSL_EN
  Serial.println(F("Clearing SSL credentials..."));
  espClient.setTrustAnchors(nullptr);            // Giải phóng CA Root Certificate
  espClient.setClientRSACert(nullptr, nullptr);  // Giải phóng Client Cert + Private Key
#endif
  //client.~PubSubClient();  // Gọi destructor để giải phóng bộ nhớ
  // client = nullptr;  // Thu hồi bộ nhớ của PubSubClient
}

void FOTA(String link) {
  //disconnectMQTT();
  // Serial.printf_P(PSTR("Free mem=%d, Time=%d BAT_STT %d\n"), ESP.getFreeHeap(), Time, digitalRead(BAT_FULL));
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {
    //setClock();
    // BearSSL::WiFiClientSecure e;
    // WiFiClientSecure e;
    WiFiClient e;
    //BearSSL::WiFiClientSecure e;
    // WiFiClientSecure e;
    //e.setInsecure();  //bỏ qua xác thực

    // Add optional callback notifiers
    ESPhttpUpdate.setLedPin(LED, LOW);
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    ESPhttpUpdate.rebootOnUpdate(false);  // remove automatic update

    Serial.println(F("Update start now!"));

    t_httpUpdate_return ret = ESPhttpUpdate.update(e, link);
    // Or:
    //t_httpUpdate_return ret = ESPhttpUpdate.update(e, "150.95.105.144", 443, "/api/firmwares/download?ver=FINAL-392025&key=af256f4b-6f64-44cd-829a-122bf47af12d");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        Serial.println(F("Retry in 10secs!"));
        delay(1000);  // Wait 10secs
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        //client.publish("DEVICE_OFFLINE", "HTTP_UPDATE_OK");
        break;
    }
  }
}
void FOTA_FS(String link) {
  //disconnectMQTT();
  WiFiClient e;
  // BearSSL::WiFiClientSecure e;
  // WiFiClientSecure e;
  //WiFiClient e;
  //BearSSL::WiFiClientSecure e;
  // WiFiClientSecure e;
  // e.setInsecure();  //bỏ qua xác thực
  // WiFiClientSecure e;
  // e.setInsecure();
  ESPhttpUpdate.setLedPin(LED, LOW);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);
  t_httpUpdate_return ret = ESPhttpUpdate.updateFS(e, link);
  if (ret == HTTP_UPDATE_OK) {
    Serial.println("Update sketch...");
    // ret = ESPhttpUpdate.update(espClient, link);
    switch (ret) {
      case HTTP_UPDATE_FAILED: Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str()); break;

      case HTTP_UPDATE_NO_UPDATES: Serial.println("HTTP_UPDATE_NO_UPDATES"); break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
}
