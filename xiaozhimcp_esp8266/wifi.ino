
bool testWifi(void) {
  int c = 0;
  while (c < 500) {
   // timer.run();
    check_phim();
    if (WiFi.status() == WL_CONNECTED) {
     // WiFi.setSleepMode(WIFI_NONE_SLEEP);
      return true;  //THOAT CT
    }
    delay(50);
    Serial.print(".");
    bool ledState = digitalRead(LED);
    ((ledState == HIGH) ? digitalWrite(LED, LOW) : digitalWrite(LED, HIGH));
    c++;
  }
  Serial.println("");
  Serial.println("Thoi gian ket noi cham, Mo AP");
  return false;
}


void scanWiFi() {
  uint8_t n = WiFi.scanNetworks();
  Serial.println("Tìm kiếm hoàn tất");
  if (n == 0) {
    Serial.println("Không tìm thấy mạng WiFi nào");
    st += "<div>Không tìm thấy mạng WiFi nào</div>";
  } else {
    Serial.print(n);
    Serial.println(" mạng WiFi được tìm thấy");

    st = "<ol class='wifi-list'>";
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm)");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");

      st += "<li><a href='#' onclick=\"fillSSID('";
      st += WiFi.SSID(i);
      st += "')\">";
      st += WiFi.SSID(i);
      st += " (" + String(WiFi.RSSI(i)) + " dBm)";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : " *";
      st += "</a></li>";
    }
    st += "</ol>";
  }
  WiFi.scanDelete();
}
void xoa_wifi() {
  EEPROM.begin(512);
  for (int i = 0; i < 62; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
  delay(1000);
}
/*
void check_wf() {
  if (wifi_mode == 0) {
    // WiFi.hostname("LOALPB");
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    dbg_printf("[WiFi] KET NOI TOI WIFI...\n", ssid, pass);
    WiFi.begin(ssid, pass);
    // int connRes = WiFi.waitForConnectResult();
    // if (connRes != WL_CONNECTED) {
    //   dbg_printf("[WiFi] Connection to %s failed - Status %d, %s, Retrying in 10s...\n", WiFi.SSID().c_str(), WiFi.status(), connectionStatus(WiFi.status()).c_str());
    //   WiFi.disconnect();
    //   //ESP.restart();
    // } else {
    //   dbg_printf("[WiFi] Connected to %s, IP address: " IPSTR "\n", WiFi.SSID().c_str(), IP2STR(WiFi.localIP()));
    // }
    // WiFi.mode(WIFI_STA);
    if (testWifi()) {
      //WiFi.setAutoConnect(true);
      // WiFi.setAutoReconnect(true);
      //WiFi.persistent(0);
      Serial.println("WiFi ket noi");
      Serial.print("Dia chi IP: ");
      Serial.println(WiFi.localIP());
      wifi_mode = 1;
      PlayMP3File(wifi_ok);
      config_time();
      setClock();
    } else {
      wifi_mode = 0;
    }
  } else {
    Serial.println("WiFi OK");
    wifi_mode = 1;
  }
}
*/

void dbg_printf(const char *format, ...) {

  static char sbuf[1400];  // For debug lines
  va_list varArgs;         // For variable number of params

  va_start(varArgs, format);                       // Prepare parameters
  vsnprintf(sbuf, sizeof(sbuf), format, varArgs);  // Format the message
  va_end(varArgs);                                 // End of using parameters

  Serial.print(sbuf);
}

String connectionStatus(int which) {
  switch (which) {
    case WL_CONNECTED:
      return "Connected";
      break;

    case WL_NO_SSID_AVAIL:
      return "Network not availible";
      break;

    case WL_CONNECT_FAILED:
      return "Wrong password";
      break;

    case WL_IDLE_STATUS:
      return "Idle status";
      break;

    case WL_DISCONNECTED:
      return "Disconnected";
      Serial.println("mat ket noi WIFI");
      break;

    default:
      return "Unknown";
      break;
  }
}
void eventWiFi(WiFiEvent_t event) {

  switch (event) {
    case WIFI_EVENT_STAMODE_CONNECTED:
      dbg_printf("[WiFi] %d, Connected\n", event);
     // wifi_mode = 1;
      break;

    case WIFI_EVENT_STAMODE_DISCONNECTED:
      dbg_printf("[WiFi] %d, Disconnected - Status %d, %s\n", event, WiFi.status(), connectionStatus(WiFi.status()).c_str());
      wifi_mode = 0;

      break;

    case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
      dbg_printf("[WiFi] %d, AuthMode Change\n", event);
      break;

    case WIFI_EVENT_STAMODE_GOT_IP:
      dbg_printf("[WiFi] %d, Got IP\n", event);
      wifi_mode = 1;
      //setupOTA();
      break;

    case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
      dbg_printf("[WiFi] %d, DHCP Timeout\n", event);
      break;

    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
      dbg_printf("[AP] %d, Client Connected\n", event);
      break;

    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
      dbg_printf("[AP] %d, Client Disconnected\n", event);
      break;

    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
      //      dbg_printf("[AP] %d, Probe Request Recieved\n", event);
      break;
  }
}
