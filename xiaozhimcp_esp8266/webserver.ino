#include "web.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>
String content;

void wifi() {
  String content = String(FPSTR(top_head));
  content += "<h2>Domee Smart Switch-" + String(SN) + String(FW_VERSION) + "</h2>";
  content += "<form method='post' action='/save'>";
  content += "<div>Tên WiFi</div><div><input id='ssid' name='ssid' placeholder='" + String(esid) + "'></div>";
  content += "<div>Mật khẩu</div><div><input name='pass' type='name' placeholder='" + String(epass) + "'></div>";
  content += "<div>API MCP CHATBOT XIAOZHI</div>"
             "<div><input name='mcplink' type='text' placeholder='nhập link MCP xiaozhi vào đây'></div>";

  content += "<div><button type='submit'>Lưu&Khởi động lại</button></div></form>";

  content += "<p><div>Các Mạng WiFi Khả dụng:</div>" + st + "</p>";
  content += "<script>function fillSSID(ssid){ document.getElementById('ssid').value = ssid; }</script>";

  content += "<div><form action='/scanWifi' method='post'><button>Quét mạng WiFi</button></form></div>";
  content += "<div><form action='/update'><button>Nâng cấp phần mềm</button></form></div>";
  content += "<div><form action='/reset1' method='post'><button>Khởi động lại</button></form></div>";
  content += "<div class='status'>WiFi LV: " + String(WiFi.RSSI()) + "dBm   Boot time: " + String(Time) + " MCP STT: " + String(mcp_stt) + "</div>";

  content += FPSTR(bot_head);
  server.send(200, "text/html", content);
}

void reset1() {
  String content = String(FPSTR(top_head));
  content += "<h2>Khởi động lại thiết bị</h2><h3>Thiết bị đang được khởi động lại...</h3>";
  content += FPSTR(bot_head);
  server.send(200, "text/html", content);
  delay(500);
  ESP.restart();
}

void save() {
  EEPROM.begin(512);
  bool changed = false;

  if (server.arg("ssid") != "") {
    String qsid = server.arg("ssid");
    Serial.print("writing eeprom ssid: ");
    Serial.println(qsid);

    for (int i = 0; i < 61; ++i) EEPROM.write(i, 0);  // Xóa dữ esid liệu cũ
    //for (int i = 31; i < 62; ++i) EEPROM.write(i, 0);  // Xóa dữ liệu cũ
    for (int i = 0; i < qsid.length(); ++i) EEPROM.write(i, qsid[i]);
    changed = true;
  }

  if (server.arg("pass") != "") {
    String qpass = server.arg("pass");
    Serial.print("writing eeprom pass: ");
    Serial.println(qpass);
    // for (int i = 0; i < 31; ++i) EEPROM.write(i, 0);   // Xóa dữ liệu cũfor (int i = 0; i < 31; ++i) EEPROM.write(i, 0); // Xóa dữ liệu cũ
    for (int i = 61; i < 92; ++i) EEPROM.write(i, 0);  // Xóa dữ epass liệu cũ
    for (int i = 0; i < qpass.length(); ++i) EEPROM.write(61 + i, qpass[i]);
    changed = true;
  }
  if (server.arg("mcplink") != "") {
    String mcplink = server.arg("mcplink");
    Serial.print("writing eeprom mcplink: ");
    Serial.println(mcplink);
    saveMcpLink(mcplink);
  }

  if (changed) EEPROM.commit();
  EEPROM.end();

  String content = String(FPSTR(top_head));
  content += "<h2>WiFi - Thiết lập thành công!</h2><h3>Thiết bị đang khởi động lại...</h3>";
  content += FPSTR(bot_head);
  server.send(200, "text/html", content);
  delay(500);
  ESP.restart();
}
