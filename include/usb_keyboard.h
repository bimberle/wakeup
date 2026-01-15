#ifndef USB_KEYBOARD_H
#define USB_KEYBOARD_H

#include <Arduino.h>
#include <USB.h>
#include <USBHIDKeyboard.h>

// Extern Keyboard-Instanz
extern USBHIDKeyboard keyboard;

// USB-Ready Flag
extern volatile bool usbReady;

/**
 * USB HID Keyboard initialisieren - Minimal version
 */
void initUSBKeyboard() {
  Serial.println("[USB] Configuring USB Stack...");
  Serial.flush();
  
  // USB konfigurieren
  USB.productName("ESP32-Wake-Tool");
  USB.manufacturerName("Custom");
  
  Serial.println("[USB] Starting USB.begin()...");
  Serial.flush();
  USB.begin();
  
  Serial.println("[USB] Starting keyboard.begin()...");
  Serial.flush();
  keyboard.begin();
  
  Serial.println("[USB] Waiting for enumeration...");
  Serial.flush();
  delay(3000);
  
  usbReady = true;
  Serial.println("[USB] ✓ USB Keyboard initialized!");
  Serial.flush();
}

/**
 * Prüft ob USB HID bereit ist
 */
bool isUSBReady() {
  return usbReady;
}

#endif // USB_KEYBOARD_H
