#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// WiFi Credentials - HIER DEINE DATEN EINTRAGEN!
const char* ssid = "K.NET";
const char* password = "GEvHcS04kRH5qoaUC9b7Z";

// Simple Web Server on port 80
WebServer server(80);

// LED Pin für Status
#define LED_PIN 2

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>ESP32-S3 Wake-Up Tool</title></head>";
  html += "<body style='font-family: Arial; text-align: center; padding: 50px;'>";
  html += "<h1>🎉 ESP32-S3 Wake-Up Tool</h1>";
  html += "<h2>WiFi Connected!</h2>";
  html += "<p>IP Address: " + WiFi.localIP().toString() + "</p>";
  html += "<p>RSSI: " + String(WiFi.RSSI()) + " dBm</p>";
  html += "<hr>";
  html += "<h3>Next Steps:</h3>";
  html += "<ol style='text-align: left; max-width: 500px; margin: 0 auto;'>";
  html += "<li>Open Home Assistant</li>";
  html += "<li>Go to ESPHome Dashboard</li>";
  html += "<li>Add new device with this IP: <strong>" + WiFi.localIP().toString() + "</strong></li>";
  html += "<li>Flash ESPHome firmware via WiFi (OTA)</li>";
  html += "</ol>";
  html += "<hr>";
  html += "<p><em>Board ready for ESPHome!</em></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void blinkLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(delayMs);
    digitalWrite(LED_PIN, LOW);
    delay(delayMs);
  }
}

void setup() {
  // LED Setup
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Serial Setup
  delay(2000);
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n========================================");
  Serial.println("[BOOT] ESP32-S3 Wake-Up Tool - WiFi Setup");
  Serial.println("========================================");
  
  // WiFi Connect
  Serial.printf("[WIFI] Connecting to %s...\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    blinkLED(1, 100);
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WIFI] ✓ Connected!");
    Serial.printf("[WIFI] IP Address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("[WIFI] Signal Strength: %d dBm\n", WiFi.RSSI());
    
    // LED dauerhaft an = connected
    digitalWrite(LED_PIN, HIGH);
    
    // Start Web Server
    server.on("/", handleRoot);
    server.begin();
    Serial.println("[HTTP] ✓ Web server started on port 80");
    
    Serial.println("\n========================================");
    Serial.println("[READY] Board ready for ESPHome!");
    Serial.printf("[READY] Open: http://%s/\n", WiFi.localIP().toString().c_str());
    Serial.println("========================================\n");
    
  } else {
    Serial.println("\n[WIFI] ✗ Connection failed!");
    Serial.println("[WIFI] Check SSID and password");
    // LED schnell blinken = error
    while(true) {
      blinkLED(3, 100);
      delay(1000);
    }
  }
}

void loop() {
  server.handleClient();
  delay(10);
}
