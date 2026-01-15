# GitHub Copilot Instructions für ESP32 Wake-Up Tool

## Projektübersicht
- **Ziel**: ESP32-S3-CAM als USB-Tastatur zur PC-Aktivierung via Bewegungserkennung
- **Aktuelles Board**: ESP32-S3-CAM (NICHT esp32s3box - das verursacht Bootfails!)
- **Host-System**: MacBook M4 (M-Serie)
- **Target**: Windows 11 PC (USB-OTG nach stabiler TTL-Baseline)
- **Build-System**: PlatformIO
- **Status**: ✅ Serial-TTL Upload & Boot funktioniert | ⏳ USB-HID & Camera in Entwicklung

## KRITISCHE LEARNINGS (Fehler vermeiden!)

### 🔴 Board-Definition
- **FALSCH**: `board = esp32s3box` → führt zu Guru Meditation Errors beim Boot
- **RICHTIG**: `board = esp32-s3-devkitc-1` → funktioniert auch mit ESP32-S3-CAM Hardware
- Grund: Die DevKit-Definition ist generischer und kompatibel mit ESP32-S3-CAM

### 🔴 USB-Konfiguration
**Problem**: USB-HID Initialisierung verursacht Serial-Output Corruption
- `USB.begin()` muss mit Vorsicht verwendet werden
- `ARDUINO_USB_CDC_ON_BOOT=0` reduziert Init-Probleme, behebt aber nicht alles
- **Lösung**: Für Entwicklung Serial-TTL nutzen (`ARDUINO_USB_MODE=0`), USB-HID später separat testen

**Serial-Garbage-Debugging**:
- Wenn nach Setup() Garbage kommt: Liegt meist an USB/Library-Init
- Nicht an Code-Logic oder Baudrate!
- Lösungsansatz: Libraries auskommentieren → feststellen welche das verursacht

### 🔴 Kamera-Initialisierung  
- `esp_camera_init()` mit Standard-Pins crasht (Guru Meditation)
- **Grund**: Pin-Konflikte zwischen Kamera-Daten und anderen Funktionen
- **Status**: Noch nicht behoben - Camera aus der initialen Version entfernt
- **TODO**: Mit OV sensor spezifischen Pins debuggen (möglicherweise I2C/XCLK Konflikt)

### 🔴 TTL-Adapter vs USB-OTG
- **TTL-Adapter** (über /dev/cu.usbserial-*): Zuverlässig, funktioniert sofort
- **USB-OTG**: Board wird erkannt aber kein /dev/tty* Device auf macOS (braucht CDC)
- **Empfehlung**: TTL für Entwicklung/Debugging, USB-OTG später nur für HID-Funktionalität

## Wichtige Technologien
- **USB-HID**: Tastatur-Emulation über USB (noch nicht aktiv)
- **esp32-camera**: Kameramodul (noch nicht stabil - Pin-Konflikte)
- **Serial/TTL**: Debug-Output (funktioniert stabil)
- **ArduinoOTA**: Firmware-Updates über WiFi (noch nicht getestet)

## Häufige Aufgaben
1. **Build**: `pio run -e esp32-s3-cam`
2. **Upload via TTL** (aktuell): `pio run -t upload -e esp32-s3-cam --upload-port /dev/cu.usbserial-110`
3. **Serial Monitor**: `cat /dev/cu.usbserial-110` (oder `pio device monitor -b 115200`)
4. **Bewegungserkennung**: Siehe [include/motion_detection.h](../include/motion_detection.h)
5. **Wake-Up Taste ändern**: `sendWakeupKey()` in [src/main.cpp](../src/main.cpp)

## Build-Flags Erklärung
```ini
# Aktuell in Verwendung (TTL-only):
build_flags =
    -DARDUINO_USB_MODE=0    # Kein USB (Serial-TTL statt CDC)

# NICHT verwenden (verursacht Probleme):
# -DARDUINO_USB_MODE=1 -DARDUINO_USB_CDC_ON_BOOT=1  # Serial-Corruption!
```

## Debugging-Tipps
- Gibberish/Garbage nach Boot? → USB-Init Problem, nicht Code
- Guru Meditation Error? → Pin-Konflikt, meist bei Kamera
- Board nicht erkannt? → TTL-Adapter prüfen, oder Bootloader-Mode (Boot+Reset)
- Serial funktioniert aber USB-HID nicht? → Separate Test-Firmware für USB schreiben

---

**Letzter Update**: 13. Januar 2026
