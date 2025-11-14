void set_state(int lightIndex, bool state) {
  // Chuyển về index 0-based
  int idx = lightIndex - 1;

  if (idx < 0 || idx >= 2) {
    Serial.printf("[set_state] Sai chỉ số đèn: %d\n", lightIndex);
    return;
  }
  digitalWrite(RELAY_PINS[idx], state);

  // In ra log
  Serial.printf("[set_state] Điều khiển đèn %d: %s\n", lightIndex, state ? "BẬT" : "TẮT");
}
bool getLightState(int index) {
  return digitalRead(RELAY_PINS[index]);
}