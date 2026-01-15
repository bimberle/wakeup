#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n[BOOT] Setup started");
  Serial.flush();
}

void loop() {
  Serial.println("[LOOP] Running...");
  Serial.flush();
  delay(2000);
}
