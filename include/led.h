#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include "config.h"

/**
 * LED Kontroller für Motion Detection Feedback
 */
class LEDController {
private:
  bool blinking;
  uint32_t blink_duration;
  uint32_t blink_start;

public:
  LEDController() : blinking(false), blink_duration(0), blink_start(0) {}

  /**
   * Initialisiert die LED
   */
  void begin() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    Serial.printf("LED initialized on GPIO %d\n", LED_PIN);
  }

  /**
   * LED blinken lassen für angegebene Millisekunden
   * @param duration_ms Wie lange soll die LED blinken
   */
  void blink(uint32_t duration_ms = 100) {
    digitalWrite(LED_PIN, HIGH);
    blinking = true;
    blink_duration = duration_ms;
    blink_start = millis();
    Serial.println("LED blink triggered!");
  }

  /**
   * Muss regelmäßig aufgerufen werden um Blink-Timing zu verwalten
   */
  void update() {
    if (blinking && (millis() - blink_start) > blink_duration) {
      digitalWrite(LED_PIN, LOW);
      blinking = false;
    }
  }

  /**
   * LED an
   */
  void on() {
    digitalWrite(LED_PIN, HIGH);
    blinking = false;
  }

  /**
   * LED aus
   */
  void off() {
    digitalWrite(LED_PIN, LOW);
    blinking = false;
  }

  /**
   * LED-Status
   */
  bool isOn() {
    return digitalRead(LED_PIN) == HIGH;
  }
};

#endif // LED_H
