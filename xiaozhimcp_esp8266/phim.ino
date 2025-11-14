#include "ButtonDebounced.h"

//ButtonDebounced button(0, but1_click, but1_long_click, but1_double_click);    //DOWN // giu cai wifi
ButtonDebounced button2(BUTTON1, but2_click, but2_long_click, but2_double_click);  // UP // giu nghe lai, 2 lan nghe mang
ButtonDebounced button3(BUTTON2, but3_click, but3_long_click, but3_double_click);  // FUN giu cai wifi,
void check_phim() {
  //button.loop();
  button2.loop();
  button3.loop();
}
void but1_click() {

  Serial.println("da bam nut 1!");
  
}

void but1_long_click() {
  Serial.println("LONG CLICK 1");
  // wifi_mode = 1;
  // setupAP();
}
void but1_double_click() {
  char buff[64] = { 0 };
  sprintf(buff, "{\"RSP\":\"SN_REQUEST\",\"MAC_ID\":\"%s\"}", SN);
  client.publish("SERIAL_NUMBER", buff);  //{ "RSP": "PlayAmount", "RESULT":
  Serial.println("double CLICK 1");
}
void but2_click() {

  Serial.println("CLICK 2");
  Serial.println("da bam nut 2!");
  if (digitalRead(RELAY1) == HIGH) {
    digitalWrite(RELAY1, LOW);
  } else digitalWrite(RELAY1, HIGH);
  client.publish(topic_rec, get_state());
}

// TANG AM
void but2_long_click() {
  Serial.println("LONG CLICK 2");
}
void but2_double_click() {
  // char buff[64] = { 0 };
  // sprintf(buff, "{\"RSP\":\"SN_REQUEST\",\"MAC_ID\":\"%s\"}", SN);
  // client.publish("SERIAL_NUMBER", buff);  //{ "RSP": "PlayAmount", "RESULT":
  Serial.println("double CLICK 2");
}

void but3_click() {

  Serial.println("da bam nut 3!");
  if (digitalRead(RELAY2) == HIGH) {
    digitalWrite(RELAY2, LOW);
  } else digitalWrite(RELAY2, HIGH);
  client.publish(topic_rec, get_state ());
}

void but3_long_click() {
  Serial.println("LONG CLICK 1");
  Serial.println("CAI WIFI");
  //wifi_mode = 0;
  AP_MODE = 1;
  setupAP();
}
void but3_double_click() {
  Serial.println("double CLICK 3");
}