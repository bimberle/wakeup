# GitHub Copilot Instructions für ESP32-S3-CAM Wake-Up Tool

## 🎯 Projektübersicht
- **Ziel**: ESP32-S3-CAM mit VL53L0X ToF-Sensor zur Bewegungserkennung → Windows 11 PC aufwecken via Wake-on-LAN
- **Hardware**: ESP32-S3-CAM WROOM + VL53L0X Breakout Board
- **Firmware**: ESPHome (via OTA Flash von Home Assistant)
- **Integration**: Home Assistant + WiFi
- **Status**: ✅ Board im WiFi (192.168.1.154) → ESPHome OTA Flash ausstehend

## 🔧 Hardware-Setup

### I2C Sensor (VL53L0X) Anschluss
```
VL53L0X Pin    →  ESP32-S3-CAM Pin
─────────────────────────────────
VCC            →  3.3V
GND            →  GND
SDA            →  GPIO 21
SCL            →  GPIO 20
```

### LED für Bewegungs-Feedback
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

## 🚀 ESPHome Setup

### Aktueller Stand
Board ist im WiFi! IP: **192.168.1.154**

### Nächster Schritt: OTA Flash via Home Assistant
1. **ESPHome Dashboard** öffnen: `http://homeassistant.local:6052/`
2. **NEW DEVICE** → `wake_up_tool.yaml` Inhalt einfügen
3. **Secrets** eintragen (WiFi, API Key, OTA Password)
4. **Install** → **Wirelessly** → IP: `192.168.1.154`
5. **Fertig!** Board hat ESPHome Firmware

### Nach Flash erscheinen in HA:
- `binary_sensor.motion_detected`
- `sensor.distance`
- `light.motion_detected_led`

## 📡 Wake-on-LAN Automation

```yaml
automation:
  - alias: "PC Wake-Up on Motion"
    trigger:
      platform: state
      entity_id: binary_sensor.motion_detected
      to: "on"
    action:
      - service: wake_on_lan.send_magic_packet
        data:
          mac: "48:21:0b:73:69:43"
```
## 📊 Sensor-Konfiguration (nach ESPHome Flash)

### Motion Detection - KALIBRIERBAR!
```yaml

**Parameter anpassen:**
- **motion_threshold**: Wie viel mm muss sich die Distanz ändern?
  - `30mm` = sehr empfindlich (schnelle Reaktion, aber mehr False-Positives)
  - `50mm` = ausgewogen (Standard)
  - `100mm` = robust (nur echte Bewegungen, langsamer)

**Debounce (Filter):**
```yaml
filters:
  - delayed_on: 200ms   # 👈 PARAMETER 2: Wie lange bis Motion "aktiv" ist
  - delayed_off: 1500ms # 👈 PARAMETER 3: Wie lange Motion "nachwirkt"
```

- `delayed_on`: Wie lange muss Bewegung erkannt werden bevor Alarm?
  - `100ms` = schnell, aber anfällig für Rauschen
  - `200ms` = Standard (empfohlen)
  - `500ms` = träge, nur starke Bewegungen

- `delayed_off`: Wie lange bleibt Motion "aktiv" nach letzter Erkennung?
  - `500ms` = schnell zurücksetzen
  - `1500ms` = Standard (verhindert Re-Triggering)
  - `3000ms` = lange Nachwirkung

### Kalibrierung - So findest du die besten Werte:

1. **ESPHome Logs aktivieren:**
   ```yaml
   logger:
     level: DEBUG  # Zeigt motion_threshold Logs
   ```

2. **OTA Update flashen** und beobachte die Logs

3. **Teste verschiedene Bewegungen:**
   - Hand schnell nähern
   - Hand langsam nähern
   - Hand weg
   - Vorbeigehend

4. **Parameter anpassen:**
   - Zu viele False-Positives? → `motion_threshold` erhöhen (z.B. 75)
   - Zu langsam? → `delayed_on` senken (z.B. 100ms)
   - Zu oft getriggert? → `delayed_off` erhöhen (z.B. 2000ms)

5. **Nach jeder Änderung OTA flashen** und erneut testen

## 🐛 Troubleshooting

### Problem: "I2C device not found at 0x29"
- ✓ Alle 4 Kabel (VCC, GND, SDA, SCL) fest angeschlossen?
- ✓ SDA an GPIO 21, SCL an GPIO 20?
- ✓ Sensor hat 3.3V?
→ **Fix**: `scan: true` in ESPHome i2c-Sektion → findet alle I2C Devices

### Problem: Gerät verbindet sich nicht mit WiFi
- ✓ SSID & Passwort korrekt in secrets.yaml?
- ✓ 2.4GHz WiFi (kein 5GHz)?
- ✓ Falls Fallback AP: mit "WakeUpTool-Fallback" + "12345678" verbinden
→ **Fix**: ESPHome Logs prüfen (Home Assistant > ESPHome > Device > Logs)

### Problem: Motion wird nicht erkannt
- ✓ Sensor angesteckt & I2C funktioniert?
- ✓ Bewegung ist schnell genug? (muss > `motion_threshold` mm Änderung sein)
- ✓ Threshold zu hoch? (standard: 50mm, senken auf 30-40 für mehr Empfindlichkeit)
- ✓ Delayed-on zu lange? (standard: 200ms, senken für schnellere Reaktion)
→ **Fix**: Siehe **Sensor-Konfiguration** → **Kalibrierung** Sektion
→ **Debug**: `logger: level: DEBUG` in YAML für Distanz-Logs

### Problem: WoL Magic Packet wird nicht gesendet
- ✓ Windows PC Wake-on-LAN in BIOS aktiviert?
- ✓ Netzwerkkarte Wake-on-LAN Support hat?
- ✓ Richtige MAC-Adresse der Windows NIC?
→ **Fix**: MAC mit `ipconfig /all` auf Windows finden

## 🔗 Wichtige Links
- ESPHome Docs: https://esphome.io/
- VL53L0X Component: https://esphome.io/components/sensor/vl53l0x.html
- Wake-on-LAN in HA: https://www.home-assistant.io/integrations/wake_on_lan/
- Home Assistant Automations: https://www.home-assistant.io/docs/automation/

## 📋 Workflow Zusammenfassung

1. ✅ **Hardware verdrahten** (I2C GPIO 21/20, LED GPIO 2)
2. ✅ **WiFi-Bootstrap geflasht** (via PlatformIO)
3. ✅ **Board im WiFi** → IP: 192.168.1.154
4. ⏳ **ESPHome OTA Flash** via Home Assistant
5. ⏳ **HA Entities** erscheinen automatisch
6. ⏳ **Wake-on-LAN Automation** erstellen
7. ⏳ **Testen**: Hand vor Sensor = PC wacht auf! 🎉

## 🔗 Wichtige Dateien

| Datei | Zweck |
|-------|-------|
| `HA_QUICK_FLASH.md` | **START HERE** - OTA Flash via HA |
| `ESPHOME_SETUP.md` | Detaillierte HA Integration |
| `wake_up_tool.yaml` | ESPHome Firmware Konfiguration |
| `secrets.yaml` | Sensitive Daten (nicht in Git!) |

## 🔐 Security Notes
- API Encryption Key: IMMER verwenden (generiert von HA)
- OTA Password: für Firmware Updates nötig
- WiFi auf separatem 2.4GHz SSID empfohlen
- Fallback AP deaktivieren im Production Setup
