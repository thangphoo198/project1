const char *ntpServer = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 7 * 3600;  // GMT+7 (Việt Nam)
const int daylightOffset_sec = 0;     // Không có DST

void config_time() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, ntpServer2);
}
String setClock() {
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, ntpServer2);

  // Chờ đồng bộ thời gian
  Serial.print(F("Waiting for NTP time sync: "));
  struct tm timeinfo;
  int t=0;
  while (!getLocalTime(&timeinfo)) {
    delay(100);

    Serial.print(F("."));
    t++;
    if (t > 10) break;
  }
  Serial.println(F("\nTime synchronized!"));
  Serial.print(F("Current time (GMT+7): "));
  Serial.println(asctime(&timeinfo));
  // Chuyển sang timestamp (epoch time)
  time_t timestamp = mktime(&timeinfo);  // Lưu ý: timeinfo phải đúng múi giờ nếu bạn cần tính toán thêm

  Serial.print(F("Unix timestamp: "));
  Serial.println(timestamp);
  return String(timestamp);
}
int t_count;
void setClock2() {
  //config_time();
  // Chờ đồng bộ thời gian
  Serial.print(F("Waiting for NTP time sync: "));
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
   // delay(500);
    Serial.print(F("."));
    t_count++;
    if(t_count>4) {setManualTime();break;}

  }
  Serial.println(F("\nTime synchronized!"));
  Serial.print(F("Current time (GMT+7): "));
  Serial.println(asctime(&timeinfo));
}

void setManualTime() {
  struct timeval tv;
  tv.tv_sec = 1739576400;  // Timestamp cho 15/02/2025 00:00:00 UTC
  tv.tv_usec = 0;
  settimeofday(&tv, NULL);
  Serial.println("Time manually set to 15/02/2025");
}
String get_time() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[20];  // Đủ để chứa "YYYY-MM-DD HH:MM:SS" + null
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(timeStr);
  }
  return "1970-01-01 00:00:00";  // Trả về mặc định nếu lỗi
}