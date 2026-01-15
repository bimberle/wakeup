# ESP32 Wake-Up Tool

Ein Projekt zur Steuerung eines Windows 11 PCs mit Bewegungserkennung über eine ESP32-Kamera, über USB-OTG als HID-Tastatur emuliert.

## 🎯 Features

- **USB-HID Tastatur-Emulation**: Registriert sich als USB-Tastatur am Windows 11 PC über OTG-Port
- **Bewegungserkennung**: Erfasst Bewegungen über die angeschlossene Kamera
- **Wake-Up Funktionalität**: Sendet Tasteneingaben (z.B. Pfeiltaste) zur PC-Aktivierung
- **OTA Updates**: Firmware-Updates über WiFi ohne TTL-Verbindung
- **MacBook M4 kompatibel**: Entwicklung und Debugging über Terminal/PlatformIO

## 📋 Hardware-Anforderungen

- **ESP32-S3** (z.B. ESP32-S3-DevKitC-1 oder ESP32-S3-EYE)
- **OV2640 oder ähnliche Kamera**
- **USB-Typ-C Kabel** (für OTG am Windows PC)
- **MacBook mit M4** (für Entwicklung)

### Pinning ESP32-S3-EYE
Die Konfiguration ist bereits in [include/config.h](include/config.h) vordefiniert.

## 🔧 Einrichtung auf MacBook M4

### Voraussetzungen installieren
```bash
# Homebrew installieren (falls noch nicht vorhanden)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Python 3 installieren (für PlatformIO)
brew install python3

# PlatformIO installieren
pip3 install platformio

# VSCode Extension installieren (optional aber empfohlen)
# - PlatformIO IDE
# - ESP-IDF Tools
```

### Projekt öffnen
```bash
cd /Users/michi/Nextcloud/Haus/Touchscreen/WakeUpTool
code .
```

## 🚀 Erste Schritte

### 1. Konfiguration anpassen

Editiere [include/config.h](include/config.h):
- **WiFi SSID/Password**: Gib deine WiFi-Daten ein
- **Kamera-Pins**: Falls eine andere Camera/Board verwendet wird, anpassen
- **OTA Password**: Ändere das Standard-Passwort!

### 2. Projekt kompilieren

```bash
# Via Terminal
pio run -e esp32-s3-devkitc-1

# Oder via VSCode: PlatformIO Home > Build
```

### 3. Auf den ESP32 uploaden

**Via TTL (initial setup):**
```bash
pio run -e esp32-s3-devkitc-1 -t upload --upload-port /dev/tty.usbserial-*
```

**Via OTA (später, nach WiFi-Setup):**
```bash
pio run -e esp32-s3-devkitc-1 -t upload --upload-port <ESP32-IP-ADRESSE>
```

### 4. Seriellen Monitor öffnen
```bash
pio device monitor -b 115200 --port /dev/tty.usbserial-*
```

## 💻 Verwendung mit Windows 11 PC

### USB-OTG Verbindung

1. **Verbinde** den ESP32 über USB-Typ-C OTG-Adapter mit dem Windows 11 PC
2. Der ESP32 sollte als **USB HID Keyboard** erkannt werden
3. Bewegungen vor der Kamera triggern Tasteneingaben zum Wake-Up

### Wake-Up Konfiguration
Im Windows Device Manager sollte das Gerät als "USB Human Interface Device" aufgelistet sein.

## 🔄 OTA Updates (WiFi)

Nach erfolgreichem initial TTL-Upload können zukünftige Updates über WiFi erfolgen:

### Setup
1. Stelle sicher, dass der ESP32 mit deinem WiFi verbunden ist
2. Starte den OTA-Server: `pio run -e esp32-s3-devkitc-1 -t upload --upload-port <IP>`

### Optional: Web-Interface für OTA
Das Projekt kann um ein Web-Interface erweitert werden (mit ArduinoOTA + AsyncWebServer).

## 📊 Debugging

### Seriellen Output ansehen
```bash
pio device monitor -b 115200
```

### Spezifische Fehler debuggen
- **"Camera initialization failed"**: Pin-Konfiguration in config.h überprüfen
- **"USB Keyboard not connected"**: USB-Verbindung oder Treiber auf PC überprüfen
- **"WiFi not connected"**: SSID/Password in config.h überprüfen

## 📁 Projektstruktur

```
WakeUpTool/
├── src/
│   └── main.cpp              # Hauptprogramm
├── include/
│   ├── config.h              # Konfiguration
│   ├── motion_detection.h    # Bewegungserkennung & Camera-Init
│   ├── usb_keyboard.h        # USB-HID Keyboard
│   └── ota_update.h          # OTA Update-Funktionalität
├── platformio.ini            # PlatformIO Konfiguration
└── README.md                 # Diese Datei
```

## 🔐 Sicherheit

- **OTA Password**: Ändere `OTA_PASSWORD` in config.h auf ein sicheres Passwort
- **WiFi**: Verwende ein sicheres WiFi-Passwort
- **USB-HID**: Der ESP32 wird als Tastatur erkannt - keine Authentifizierung möglich

## 📚 Weitere Ressourcen

- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [esp32-camera Library](https://github.com/espressif/esp32-camera)
- [USBHIDKeyboard Library](https://github.com/espressif/arduino-esp32/tree/master/libraries/USB)
- [PlatformIO Dokumentation](https://docs.platformio.org/)

## 🐛 Known Limitations

- Motion Detection ist vereinfacht (Pixel-Vergleich). Für bessere Genauigkeit: OpenCV oder TensorFlow Lite verwenden
- USB-HID benötigt direkte Verbindung (kein WiFi möglich)
- OTA erfordert WiFi-Verbindung

## 📝 Lizenzen

Dieses Projekt verwendet:
- Arduino Framework
- Espressif ESP-IDF
- Diverse open-source Libraries (siehe platformio.ini)

---

**Hinweis**: Dies ist ein privates Hobby-Projekt. Verwende auf eigenes Risiko!
