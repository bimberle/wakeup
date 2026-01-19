# ESP32-S3-CAM Wake-Up Tool

Bewegungserkennung mit VL53L0X ToF-Sensor → Windows PC aufwecken via Wake-on-LAN.

## 🎯 Projekt-Status

| Feature | Status | Notizen |
|---------|--------|---------|
| WiFi Verbindung | ✅ Funktioniert | 192.168.1.154 |
| ToF-Sensor (VL53L0X) | ⏳ Bereit | I2C an GPIO 20/21 |
| ESPHome Firmware | ⏳ Nächster Schritt | Via OTA Flash |
| Home Assistant Integration | ⏳ Nach ESPHome | Automatische Erkennung |
| Wake-on-LAN | ⏳ Nach HA | Automation erstellen |

## 🔧 Hardware-Setup

### Board
- **ESP32-S3-CAM WROOM** (esp32-s3-devkitc-1)
- 8MB PSRAM

### VL53L0X ToF-Sensor (I2C)
```
VL53L0X Pin    →  ESP32-S3-CAM Pin
─────────────────────────────────
VCC            →  3.3V
GND            →  GND
SDA            →  GPIO 21
SCL            →  GPIO 20
```

### LED-Feedback
```
LED            →  ESP32-S3-CAM Pin
──────────────────────────────────
Anode (+)      →  GPIO 2 (über 470Ω)
Kathode (-)    →  GND
```

### TTL-Adapter (nur für Debugging)
```
TTL Adapter    →  ESP32-S3-CAM Pin
──────────────────────────────────
TX             →  GPIO 43 (RX)  ⚠️ Nicht GPIO 1!
RX             →  GPIO 44 (TX)  ⚠️ Nicht GPIO 3!
GND            →  GND
```

## 🚀 Schnellstart

### Aktueller Stand
Das Board ist bereits im WiFi und bereit für ESPHome!

**Board-IP: `192.168.1.154`**

### Nächster Schritt: ESPHome via Home Assistant flashen

👉 **Siehe [HA_QUICK_FLASH.md](HA_QUICK_FLASH.md)** für die komplette Anleitung!

Kurzfassung:
1. Home Assistant → ESPHome Dashboard öffnen
2. `wake_up_tool.yaml` importieren
3. **Install** → **Wirelessly** → IP: `192.168.1.154`
4. Fertig! 🎉

## 📋 Dateistruktur

```
WakeUpTool/
├── wake_up_tool.yaml     # ESPHome Firmware-Konfiguration
├── secrets.yaml          # WiFi/API Credentials (nicht in Git!)
├── HA_QUICK_FLASH.md     # ⭐ Anleitung: Flash via Home Assistant
├── ESPHOME_SETUP.md      # Detaillierte ESPHome Dokumentation
├── src/
│   └── main.cpp          # WiFi-Bootstrap Firmware (bereits geflasht)
├── include/              # PlatformIO Header (für Bootstrap)
├── platformio.ini        # PlatformIO Config
└── .github/
    └── copilot-instructions.md  # AI-Assistenten Instruktionen
```

## 🎯 Workflow

```
┌─────────────────────────────────────────────────────────────┐
│  1. WiFi-Bootstrap (PlatformIO)           ✅ ERLEDIGT       │
│     Board verbindet sich mit WiFi                           │
│     → IP: 192.168.1.154                                     │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│  2. ESPHome Flash (via Home Assistant)    ⏳ NÄCHSTER SCHRITT│
│     - wake_up_tool.yaml in HA ESPHome importieren           │
│     - OTA Flash an 192.168.1.154                            │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│  3. Home Assistant Integration                              │
│     - binary_sensor.motion_detected                         │
│     - sensor.distance                                       │
│     - light.motion_detected_led                             │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│  4. Wake-on-LAN Automation                                  │
│     - Motion detected → Magic Packet an PC                  │
│     - PC wacht auf! 🎉                                       │
└─────────────────────────────────────────────────────────────┘
```

## 🐛 Troubleshooting

### Board nicht im Netzwerk gefunden?
- Router DHCP-Liste prüfen
- Board neu starten (Reset-Taste)
- Falls nötig: PlatformIO neu flashen

### ESPHome OTA schlägt fehl?
- Prüfe ob Board erreichbar: `curl http://192.168.1.154/`
- Firewall-Regeln prüfen (Port 3232 für OTA)

### Motion wird nicht erkannt?
- Sensor-Verkabelung prüfen (I2C GPIO 20/21)
- ESPHome Logs in HA prüfen
- Threshold anpassen (siehe `wake_up_tool.yaml`)

## 📚 Dokumentation

| Datei | Beschreibung |
|-------|--------------|
| [HA_QUICK_FLASH.md](HA_QUICK_FLASH.md) | **START HERE** - ESPHome Flash via HA |
| [ESPHOME_SETUP.md](ESPHOME_SETUP.md) | Detaillierte HA + WoL Setup |
| [LESSONS_LEARNED.md](LESSONS_LEARNED.md) | Debugging-Erkenntnisse |

## 📄 Lizenz

Proprietär - Privates Projekt

---

**Aktueller Status**: Board im WiFi ✅ → ESPHome Flash ausstehend ⏳
