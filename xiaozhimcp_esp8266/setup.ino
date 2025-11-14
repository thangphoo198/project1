void read_eeprom() {
  EEPROM.begin(512);
  Serial.println("Chương trình bắt đầu, đọc EEPROM...");

  // Đọc SSID & Password từ EEPROM
  memset(esid, 0, sizeof(esid));
  memset(epass, 0, sizeof(epass));

  for (int i = 0; i < 61; ++i) esid[i] = EEPROM.read(i);
  for (int i = 61; i < 92; ++i) epass[i - 61] = EEPROM.read(i);

  Serial.printf("SSID: %s\nPASS: %s\n", esid, epass);

  // // Đọc Volume
  // volu = EEPROM.read(volAddr);
  // if (volu == 255 || pin_low) volu = 1;
  // Serial.printf("VOLUME EEPROM: %d \n", volu);

  // Đọc Serial Number (SN)
  memset(SN, 0, sizeof(SN));  // Xóa SN cũ trước khi đọc mới
  readSNFromEEPROM(SN, sizeof(SN));

  if ((uint8_t)SN[0] == 0xFF || (uint8_t)SN[1] == 0xFF) {
    Serial.println("Không tìm thấy SERIAL NUMBER, dùng ESP Chip ID");
    snprintf(SN, sizeof(SN), "%u", ESP.getChipId());  // Lưu thẳng Chip ID vào SN
  } else {
    Serial.printf("SERIAL NUMBER: %s\n", SN);
  }

  // Thiết lập các topic MQTT
  snprintf(topic_remote, sizeof(topic_remote), "SW/%s/cmnd", SN);
  snprintf(topic_rec, sizeof(topic_rec), "SW/%s/result",SN);

  Serial.printf("ID: %s  REC:%s REMOTE:%s\r\n", SN, topic_rec, topic_remote);
}
void eeprom_setup() {
  read_eeprom();  // Đọc dữ liệu từ EEPROM
  WiFi.mode(WIFI_STA);
  for (int i = 0; i < strlen(esid); i++) {
    if ((uint8_t)epass[i] == 0xFF || (uint8_t)esid[i] == 0xFF) {
      // if ((uint8_t)esid[i] == 0xFF) {
      check_wifi = true;
      break;
    }
  }
  const char* ssid = (strlen(esid) > 2 && !check_wifi) ? esid : default_SSID;
  const char* pass = (ssid == esid) ? epass : default_PASS;
  Serial.printf("Đang kết nối: SSID: %s PASS: %s\r\n", ssid, (strlen(pass) > 2) ? pass : "(OPEN)");
  WiFi.setAutoReconnect(true);
  WiFi.onEvent(eventWiFi);    
  if (strlen(pass) > 7) {
    Serial.printf("Đang kết nối: SSID: %s PASS: %s\r\n", ssid, pass);
    WiFi.begin(ssid, pass);
  } else {
    Serial.printf("Đang kết nối: SSID: %s (OPEN)\r\n", ssid);
    WiFi.begin(ssid);
  }

  if (testWifi()) {
    //  WiFi.persistent(0);
    WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
    Serial.println("WiFi ket noi");
    Serial.print("Dia chi IP: ");
    Serial.println(WiFi.localIP());
    launchWeb();  // Có thể gây lỗi stream nếu gọi ngay sau khi kết nối WiFi
    wifi_mode = 1;
    config_time();
   // setClock2();
  } else {
    wifi_mode = 0;
  }
}

// void launchWeb() {
//   Serial.println("");
//   Serial.println("WiFi ket noi");
//   Serial.print("Dia chi IP: ");
//   Serial.println(WiFi.localIP());
//   Serial.print("SoftAP IP: ");
//   Serial.println(WiFi.softAPIP());
//   if (AP_MODE) {
//     server.onNotFound([]() {
//       // server.send(200, "text/html", webSite);
//       // scanWifi();
//       wifi();
//     });
//     createWebServer();  // nguyen nhan gay stream failed

//     // if (!MDNS.begin("loa")) {
//     //   Serial.println("Error setting up MDNS responder!");
//     //   //while (1) { delay(1000); }
//     // }
//     // MDNS.addService("http", "tcp", 80);

//     //httpUpdater.setup(&server, update_path, username_device, pass_device);
//     // Serial.printf("http://%s.local%s username '%s' and password '%s'\n", host, update_path, username_device, pass_device);
//     Serial.println("May chu bat dau");
//     server.begin();
//   }
// }
void setupAP(void) {

  WiFi.mode(WIFI_OFF);  // Tắt WiFi trước khi chuyển chế độ ,nếu k
  AP_MODE = 1;
  Serial.println("Mở AP");
  disconnectMQTT();
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  scanWiFi();  //scanWiFi
  String sid = "Domee Smart Switch-";
  sid += String(SN);
  Serial.println(sid);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(sid);
  launchWeb();
  Serial.println("over");
}


void createWebServer() {
  server.on("/", []() {
    //menu();
    wifi();
  });
  server.on("/reset1", []() {
    reset1();
  });
  // // server.on("/reset2", []() {
  // //   reset2();
  // // });
  // // server.on("/factory2", []() {
  // //   factory2();
  // // });
  server.on("/save", []() {
    save();
  });
  server.on("/scanWifi", []() {
    scanWiFi();
    wifi();
  });
}

void launchWeb() {
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  // if (AP_MODE) {
    dnsServer.start(DNS_PORT, "*", apIP);
    server.onNotFound([]() {
      // server.send(200, "text/html", webSite);
      // scanWifi();
      wifi();
    });
    scanWiFi();
    createWebServer();  // nguyen nhan gay stream failed

    if (!MDNS.begin("loa")) {
      Serial.println("Error setting up MDNS responder!");
      //while (1) { delay(1000); }
    }
    MDNS.addService("http", "tcp", 80);

    httpUpdater.setup(&server, update_path, username_device, pass_device);
    Serial.printf("http://%s.local%s username '%s' and password '%s'\n", host, update_path, username_device, pass_device);
    Serial.println("May chu bat dau");
    server.begin();
  //}
}

void writeSNToEEPROM(const char *sn) {
  Serial.println("Ghi SN vào EEPROM...");
  for (int i = 0; i < strlen(sn); i++) {
    EEPROM.write(EEPROM_ADDR_SN + i, sn[i]);
  }
  EEPROM.write(EEPROM_ADDR_SN + strlen(sn), '\0');  // Kết thúc chuỗi
  EEPROM.commit();
  Serial.println("Ghi thành công!");
}
void readSNFromEEPROM(char *sn, int length) {
  Serial.println("Đọc SN từ EEPROM...");
  for (int i = 0; i < length; i++) {
    sn[i] = EEPROM.read(EEPROM_ADDR_SN + i);
  }
  sn[length - 1] = '\0';  // Đảm bảo chuỗi kết thúc hợp lệ
  Serial.print("SN đã đọc: ");
  Serial.println(sn);
}
