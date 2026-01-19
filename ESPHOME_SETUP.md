# ESPHome Setup für ESP32-S3-CAM Wake-Up Tool

## 🎯 Aktueller Status

**✅ Board ist im WiFi!** IP: `192.168.1.154`

Das Board wurde bereits mit einer WiFi-Bootstrap-Firmware geflasht und ist bereit für ESPHome OTA-Flash.

---

## 📋 Hardware-Setup

### VL53L0X ToF-Sensor (Bewegungserkennung)
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

---

## 🚀 ESPHome Flash via Home Assistant

### 1️⃣ ESPHome Dashboard öffnen

```
http://homeassistant.local:6052/
```

### 2️⃣ Konfiguration erstellen

1. **"+ NEW DEVICE"** klicken
2. Name: `wake_up_tool`
3. **"SKIP"** bei Board-Auswahl
4. **EDIT** → Inhalt von `wake_up_tool.yaml` einfügen
5. **SAVE**

### 3️⃣ Secrets eintragen

ESPHome Dashboard → **"SECRETS"**:

```yaml
wifi_ssid: "K.NET"
wifi_password: "GEvHcS04kRH5qoaUC9b7Z"
api_encryption_key: "v3gGRdPvvpYx5gvUn/e6DvJxM4YnLBT3XrB6sHeHYNQ="
ota_password: "wakeup2026"
pc_mac_address: "48:21:0b:73:69:43"
```

### 4️⃣ OTA Flash

1. **wake_up_tool** → **⋮** → **Install**
2. Wähle: **"Wirelessly"**
3. IP: **`192.168.1.154`**
4. **CONNECT**

Flash dauert ~1-2 Minuten.

### 5️⃣ Verifikation

Nach Flash erscheinen in Home Assistant:
- `binary_sensor.motion_detected`
- `sensor.distance`
- `light.motion_detected_led`

---

## 🔧 Wake-on-LAN Automation erstellen

Damit der Windows PC tatsächlich aufwacht, musst du eine HA-Automation erstellen:

### 1️⃣ Wake-on-LAN Service aktivieren
1. Home Assistant → **Settings > Devices & Services > Create Automation**
2. Oder in `configuration.yaml`:

```yaml
wake_on_lan:
```

### 2️⃣ Automation erstellen
1. Home Assistant → **Automations > Create Automation**
2. Trigger:
   - **Trigger Type**: State
   - **Entity**: `binary_sensor.motion_detected`
   - **To**: `on`

3. Action:
   - **Action Type**: Call a service
   - **Service**: `wake_on_lan.send_magic_packet`
   - **Data**:
     ```yaml
     mac_address: "AA:BB:CC:DD:EE:FF"  # Oder !secret pc_mac_address
     ```

4. Speichern!

### YAML-Beispiel (falls du es direkt schreiben möchtest):

```yaml
automation:
  - alias: "PC Wake-Up on Motion"
    description: "Wecke Windows PC wenn Motion erkannt wird"
    trigger:
      platform: state
      entity_id: binary_sensor.motion_detected
      to: "on"
    action:
      - service: wake_on_lan.send_magic_packet
        data:
          mac_address: !secret pc_mac_address
    mode: single
```

---

## 🧪 Testen

### Motion Detection testen
1. Hand vor den Sensor halten
2. **Erwartung**: 
   - `binary_sensor.motion_detected` wird `on`
   - LED blinkt
   - HA zeigt Motion Sensor als aktiv

### Wake-on-LAN testen (vor Windows PC)
1. Windows PC muss in BIOS/UEFI Wake-on-LAN aktiviert haben:
   - **BIOS Setting**: `Wake on LAN` oder `Network Boot` aktivieren
   - **Windows Netzwerk**: Netzwerkkarte-Eigenschaften → "Allow computer to wake this computer"

2. Windows PC herunterfahren (nicht Schlafmodus!)

3. Motion vor Sensor erkennen lassen

4. Windows PC sollte aufwachen! 🎉

---

## 🐛 Troubleshooting

### Problem: Board verbindet sich nicht mit WiFi
- ✓ WiFi SSID & Passwort korrekt in `secrets.yaml`?
- ✓ 2.4GHz WiFi? (5GHz wird nicht unterstützt)
- ✓ Board in Reichweite des WiFi?
→ **ESPHome Logs prüfen**: ESPHome Dashboard → Device → Logs

### Problem: Sensor wird nicht erkannt (I2C Error)
- ✓ Alle 4 Kabel (VCC, GND, SDA, SCL) fest angeschlossen?
- ✓ Korrekte Pins? (SDA=GPIO21, SCL=GPIO20)
- ✓ VL53L0X hat Stromversorgung (3.3V)?
→ **I2C Scan**: `i2c.scan: true` in YAML → zeigt alle erkannten Geräte

### Problem: Motion wird nicht erkannt
- ✓ Bewegung ist schnell genug?
- ✓ `sensor.distance` zeigt Messwerte in HA?
- ✓ Sensor zeigt richtige Werte (< 1300mm)?
→ **Kalibrierung**: Siehe `.github/copilot-instructions.md` → Sensor-Konfiguration

### Problem: Wake-on-LAN funktioniert nicht
- ✓ Windows PC hat Wake-on-LAN in BIOS aktiviert?
- ✓ Netzwerkkarte Wake-on-LAN Support?
- ✓ Richtige MAC-Adresse? (prüfe mit `ipconfig /all` auf Windows)
→ **MAC-Adresse finden**:
  ```powershell
  # Auf Windows PowerShell:
  Get-NetAdapter | Format-Table Name, MacAddress
  ```

---

## 📝 Wichtige Dateien

| Datei | Zweck |
|-------|-------|
| `wake_up_tool.yaml` | ESPHome Konfiguration (Sensor, WiFi, Automation) |
| `secrets.yaml` | Sensitive Daten (WiFi, Passwörter) - nicht in Git! |
| `.github/copilot-instructions.md` | Detaillierte Dokumentation & Kalibrierung |

---

## 🔐 Sicherheit

- **secrets.yaml** in `.gitignore` hinzufügen (nicht pushen!)
- **API Encryption Key** ist wichtig für HA-Kommunikation
- **OTA Password** schützt vor unbefugten Updates
- WiFi-Passwort ist sensitiv → nicht in GitHub

---

## 🔗 Weitere Ressourcen

- [ESPHome Dokumentation](https://esphome.io/)
- [VL53L0X Component](https://esphome.io/components/sensor/vl53l0x.html)
- [Wake-on-LAN in Home Assistant](https://www.home-assistant.io/integrations/wake_on_lan/)
- [ESPHome Home Assistant Integration](https://esphome.io/guides/getting_started_command_line.html)

---

## 🎯 Nächste Schritte

1. ✅ Hardware verdrahten
2. ✅ `secrets.yaml` mit echten Daten füllen
3. ✅ `wake_up_tool.yaml` ins ESPHome Add-on hochladen
4. ✅ Board über USB flashen
5. ✅ Motion Detection testen
6. ✅ Wake-on-LAN Automation in HA erstellen
7. ✅ Windows PC Wake-on-LAN aktivieren
8. ✅ Endtest: Motion → PC wacht auf! 🎉
