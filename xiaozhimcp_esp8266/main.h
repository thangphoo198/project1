#ifndef _MAIN_H
#define _MAIN_H
#define RELAY1 13
#define RELAY2 15
#define BUTTON1 12
#define BUTTON2 5
#define LED 0

#define MODEL "SWSV1"

// Smart switch pin definition
const int SWITCH_PINS[] = {BUTTON1, BUTTON2};
const int RELAY_PINS[] = {RELAY1,RELAY2};  // Relay output pin
bool relayStates[] = {false, false};  // Relay status
unsigned long lastDebounceTime[2] = {0};  // Last anti-shake time
const unsigned long debounceDelay = 50;  // Anti-shake delay (milliseconds)
bool mcp_stt=false;
#define RELAY_COUNT 2

#endif