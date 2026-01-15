#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "config.h"

/**
 * Initialisiert OTA Updates über WiFi
 */
void setupOTA() {
  if (!ENABLE_OTA_UPDATES) {
    Serial.println("OTA Updates disabled in config");
    return;
  }

  Serial.println("Setting up OTA Updates...");

  // WiFi verbinden (falls noch nicht verbunden)
  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20) {
      delay(500);
      Serial.print(".");
      timeout++;
    }
    Serial.println();
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("WiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());

    // ArduinoOTA konfigurieren
    ArduinoOTA.setHostname("ESP32-WakeUpTool");
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.setPort(OTA_PORT);

    ArduinoOTA.onStart([]() {
      Serial.println("OTA Update started!");
    });

    ArduinoOTA.onEnd([]() {
      Serial.println("\nOTA Update complete!");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("OTA Progress: %u/%u bytes (%.1f%%)\r", 
                    progress, total, 
                    (float)progress / total * 100);
    });

    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("OTA Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();
    Serial.println("OTA Updates enabled!");
  } else {
    Serial.println("WARNING: WiFi not connected, OTA Updates disabled");
  }
}

/**
 * Sollte in der main loop aufgerufen werden
 */
void handleOTA() {
  if (ENABLE_OTA_UPDATES && WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
  }
}

#endif // OTA_UPDATE_H
