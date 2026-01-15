# GitHub Copilot Instructions für ESP32 Wake-Up Tool

## Projektübersicht
- **Ziel**: ESP32 mit Kamera als USB-Tastatur zur PC-Aktivierung via Bewegungserkennung
- **Zielboard**: ESP32-S3-DevKitC-1 oder ESP32-S3-EYE
- **Host-System**: MacBook M4 (M-Serie)
- **Target**: Windows 11 PC (USB-OTG)
- **Build-System**: PlatformIO

## Wichtige Technologien
- **USB-HID**: Tastatur-Emulation über USB
- **esp32-camera**: Kameramodul-Steuerung
- **ArduinoOTA**: Firmware-Updates über WiFi
- **ESP-IDF**: Espressif IoT Development Framework

## Häufige Aufgaben
1. **Bewegungserkennung verbessern**: Siehe [include/motion_detection.h](include/motion_detection.h)
2. **Wake-Up Taste ändern**: Editiere `sendWakeupKey()` in [src/main.cpp](src/main.cpp)
3. **WiFi/OTA aktivieren**: Konfiguriere WiFi-Daten in [include/config.h](include/config.h)
4. **Kamera-Pins anpassen**: Board-spezifische Pins in [include/config.h](include/config.h)

## Build & Deploy
- **Kompilieren**: `pio run -e esp32-s3-devkitc-1`
- **Upload via TTL**: `pio run -t upload --upload-port /dev/tty.usbserial-*`
- **Upload via OTA**: `pio run -t upload --upload-port <IP>`
- **Serial Monitor**: `pio device monitor -b 115200`

## Debugging-Tipps
- Aktiviere Debug-Output in [src/main.cpp](src/main.cpp) mit `Serial.println()`
- Für komplexeres Debugging nutze [env:esp32-s3-devkitc-1-debug] in [platformio.ini](platformio.ini)

---

**Letzter Update**: 13. Januar 2026
