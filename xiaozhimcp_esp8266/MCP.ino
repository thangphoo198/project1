#include "main.h"
#ifdef ESP32
#include <WiFi.h>  // Thư viện WiFi cho ESP32
// Kiểm tra nếu đang sử dụng ESP8266
#elif defined(ESP8266)
#include <ESP8266WiFi.h>  // Thư viện WiFi cho ESP8266
#endif
#include <WebSocketMCP.h>
// MCP server configuration
const char* mcpEndpoint = "wss://api.xiaozhi.me/mcp/?token=eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VySWQiOjUxODEzMCwiYWdlbnRJZCI6NzUzMzA4LCJlbmRwb2ludElkIjoiYWdlbnRfNzUzMzA4IiwicHVycG9zZSI6Im1jcC1lbmRwb2ludCIsImlhdCI6MTc2Mjc3MDEzOCwiZXhwIjoxNzk0MzI3NzM4fQ.cRmJAYA9heF9erPYoDYV1DF-Rh1D-82L_7ssGt3_r676ts82ItOUFxUv9oDlUo4KtJPh3JXZaaqvlojZcKKnWw";

// Create a WebSocketMCP instance
WebSocketMCP mcpClient;
// Connection status callback function
void onConnectionStatus(bool connected) {
  if (connected) {
    Serial.println("[MCP] Connected to the server");
    mcp_stt = 1;
    // Register tool after successful connection
    registerMcpTools();
  } else {
    Serial.println("[MCP] Disconnect from the server");
    mcp_stt = 0;
  }
}
// Register MCP Tools
void registerMcpTools() {
  // Register Relay Control Tool
  /*
  mcpClient.registerTool(
    "relay_control",
    "Control two-channel relay",
    "{\"type\":\"object\",\"properties\":{\"relayIndex\":{\"type\":\"integer\",\"minimum\":1,\"maximum\":3},\"state\":{\"type\":\"boolean\"}},\"required\":[\"relayIndex\",\"state\"]}",
    [](const String& args) {
      DynamicJsonDocument doc(256);
      deserializeJson(doc, args);
      
      int relayIndex = doc["relayIndex"].as<int>() - 1;  // Convert to 0-based index
      bool state = doc["state"].as<bool>();
      
      if (relayIndex >= 0 && relayIndex < 2) {
        Serial.println("da nhan lenh dk relay");
       // controlRelay(relayIndex, state);
        return WebSocketMCP::ToolResponse("{\"success\":true,\"relayIndex\":" + String(relayIndex + 1) + ",\"state\":" + (state ? "true" : "false") + "}");
      } else {
        return WebSocketMCP::ToolResponse("{\"success\":false,\"error\":\"Invalid relay index\"}", true);
      }
    }
  );
  Serial.println("[MCP] Relay Control Tool Registeredl Tool Registered");

  */
  // -----------------------------
  // Tool: LIGHT CONTROL
  // -----------------------------
  mcpClient.registerTool(
    "light_control",
    "Control two lights (den 1, den 2) or all lights (index 0)",
    "{\"type\":\"object\",\"properties\":{\"lightIndex\":{\"type\":\"integer\",\"minimum\":0,\"maximum\":2},\"state\":{\"type\":\"boolean\"}},\"required\":[\"lightIndex\",\"state\"]}",
    [](const String& args) {
      DynamicJsonDocument doc(256);
      deserializeJson(doc, args);

      int lightIndex = doc["lightIndex"].as<int>();  // Convert to 0-based index
      Serial.println(lightIndex);
      bool state = doc["state"].as<bool>();

      if (lightIndex >= 0 && lightIndex < 3) {

        if (lightIndex == 0) {
          Serial.printf("Đã nhận lệnh điều khiển tất cả đèn: %s\n", state ? "BẬT" : "TẮT");
          // Bật/tắt tất cả đèn
          for (int i = 0; i < 2; i++) {
            set_state(i+1, state);
          }

        } else {
          Serial.printf("Đã nhận lệnh điều khiển đèn %d: %s\n", lightIndex, state ? "BẬT" : "TẮT");
          set_state(lightIndex, state);
        }
        return WebSocketMCP::ToolResponse("{\"success\":true,\"lightIndex\":" + String(lightIndex + 1) + ",\"state\":" + (state ? "true" : "false") + "}");
      } else {
        return WebSocketMCP::ToolResponse("{\"success\":false,\"error\":\"Invalid light index\"}", true);
      }
    });
  mcpClient.registerTool(
    "light_status",
    "Query the status of two lights",
    "{\"type\":\"object\",\"properties\":{}}",
    [](const String& args) {
      String result = "{\"success\":true,\"lightIndex\":[";
      for (int i = 0; i < 2; i++) {
        if (i > 0) result += ",";
        // result += "{\"index\":" + String(i + 1) + ",\"state\":" + (relayStates[i] ? "true" : "false") + "}";
        result += "{\"index\":" + String(i + 1) + ",\"state\":" + (getLightState(i) ? "true" : "false") + "}";
      }
      result += "]}";
      Serial.print("da gui trang thai relay:");
      Serial.println(result);
      return WebSocketMCP::ToolResponse(result);
    });
  Serial.println("[MCP] Relay status query tool registered");
  // Register relay status query tool
  // mcpClient.registerTool(
  //   "relay_status",
  //   "Query the status of two relays",
  //   "{\"type\":\"object\",\"properties\":{}}",
  //   [](const String& args) {
  //     String result = "{\"success\":true,\"relays\":[";
  //     for (int i = 0; i < 2; i++) {
  //       if (i > 0) result += ",";
  //       result += "{\"index\":" + String(i + 1) + ",\"state\":" + (relayStates[i] ? "true" : "false") + "}";
  //     }
  //     result += "]}";
  //     Serial.print("da gui trang thai relay:");
  //     Serial.println(result);
  //     return WebSocketMCP::ToolResponse(result);
  //   }
  // );
  // Serial.println("[MCP] Relay status query tool registered");
  // === 2. Air Conditioner Control Tool ===
  mcpClient.registerTool(
    "ac_control",
    "Control air conditioner via IR",
    R"({
      "type":"object",
      "properties":{
        "power":{"type":"boolean"},
        "temperature":{"type":"integer","minimum":16,"maximum":30},
        "mode":{"type":"string","enum":["cool","heat","dry","fan","auto"]},
        "fan":{"type":"string","enum":["low","medium","high","auto"]}
      },
      "required":["power"]
    })",
    [](const String& args) {
      DynamicJsonDocument doc(512);
      DeserializationError err = deserializeJson(doc, args);
      if (err) {
        return WebSocketMCP::ToolResponse("{\"success\":false,\"error\":\"JSON parse error\"}", true);
      }

      bool power = doc["power"] | false;
      int temperature = doc["temperature"] | 24;
      String mode = doc["mode"] | "auto";
      String fan = doc["fan"] | "auto";

      Serial.println("Da nhan lenh dk dieu hoa:");
      Serial.printf("Power: %s, Temp: %d, Mode: %s, Fan: %s\n", power ? "ON" : "OFF", temperature, mode.c_str(), fan.c_str());

      // === Gọi hàm gửi IR thật ===
      // sendACCommand(power, temperature, mode, fan);

      String response = "{\"success\":true,\"power\":" + String(power ? "true" : "false") + ",\"temperature\":" + String(temperature) + ",\"mode\":\"" + mode + "\"" + ",\"fan\":\"" + fan + "\"}";
      return WebSocketMCP::ToolResponse(response);
    });

  mcpClient.registerTool(
    "fan_control",
    "Control fan (on/off, speed, direction)",
    R"({
    "type":"object",
    "properties":{
      "state":{"type":"boolean"},
      "speed":{"type":"integer","minimum":1,"maximum":3},
      "reverse":{"type":"boolean"}
    },
    "required":["state"]
  })",
    [](const String& args) {
      DynamicJsonDocument doc(256);
      DeserializationError err = deserializeJson(doc, args);
      if (err) {
        return WebSocketMCP::ToolResponse("{\"success\":false,\"error\":\"JSON parse error\"}", true);
      }

      bool state = doc["state"].as<bool>();
      int speed = doc.containsKey("speed") ? doc["speed"].as<int>() : 1;
      bool reverse = doc.containsKey("reverse") ? doc["reverse"].as<bool>() : false;

      if (!state) {
        Serial.println("Tắt quạt");
        // controlFan(false, 0, false);
      } else {
        Serial.printf("Bật quạt tốc độ %d, đảo chiều: %s\n", speed, reverse ? "ON" : "OFF");
        // controlFan(true, speed, reverse);
      }

      String response = "{\"success\":true,\"state\":" + String(state ? "true" : "false") + ",\"speed\":" + String(speed) + ",\"reverse\":" + String(reverse ? "true" : "false") + "}";
      return WebSocketMCP::ToolResponse(response);
    });
}
void mcp_init() {
  mcpClient.begin(mcpEndpoint, onConnectionStatus);
}

void mcp_loop() {
  mcpClient.loop();
}