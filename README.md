# ESP32-S3 Wake-Up Tool

Bewegungserkennung mit VL53L0X ToF-Sensor → Windows 11 PC aufwecken via USB HID.

## 🎯 Projekt-Status

| Feature | Status | Notizen |
|---------|--------|---------|
| VL53L0X ToF-Sensor | ✅ Funktioniert | Pololu-Port mit Kalibrierung |
| USB HID Keyboard | ✅ Funktioniert | Remote Wakeup |
| Motion Detection | ✅ Funktioniert | Delta > 500mm |
| Windows 11 Wake | ✅ Funktioniert | Aus Standby |

## 🔧 Hardware-Setup

### Board
- **ESP32-S3 WROOM** (z.B. Freenove ESP32-S3-WROOM)
- Dual USB: OTG (für Windows) + UART (für Flashen)

### VL53L0X ToF-Sensor (I2C)
```
VL53L0X Pin    →  ESP32-S3 Pin
─────────────────────────────────
VCC            →  3.3V
GND            →  GND
SDA            →  GPIO 14
SCL            →  GPIO 21
XSHUT          →  GPIO 47
```

### USB-Anschlüsse
```
USB Port       →  Verwendung
──────────────────────────────────
USB OTG        →  Windows PC (für Wake-Signal)
USB UART       →  Mac/PC zum Flashen & Debuggen
```

**⚠️ Wichtig:** Der USB OTG Port ist der native USB-C/Micro - NICHT der UART-Adapter!

## 🚀 Schnellstart

Das funktionierende Projekt liegt in `esp-idf-wakeup/`:

```bash
cd esp-idf-wakeup

# 1. ESP-IDF installieren (einmalig)
./00-init.sh

# 2. Target setzen (einmalig)
./00-set-target.sh esp32s3

# 3. Bauen und Flashen
source esp-idf/export.sh
idf.py build flash -p /dev/cu.usbserial-110
```

**Hinweis:** Port kann variieren - prüfe mit `ls /dev/cu.*`

## ⚙️ Parameter anpassen

Die wichtigsten Parameter findest du in `esp-idf-wakeup/main/tof_sensor.h`:

```c
#define MOTION_THRESHOLD_MM     500     // Distanzänderung für Trigger (mm)
#define MOTION_COOLDOWN_MS      240000  // Pause nach Wake (4 Minuten)
#define TOF_SAMPLE_INTERVAL_MS  200     // Abtastrate
```

Nach Änderung neu flashen:
```bash
cd esp-idf-wakeup
source esp-idf/export.sh
idf.py build flash -p /dev/cu.usbserial-110
```

## 🖥️ Windows Setup

1. ESP32 mit **USB OTG Port** an Windows PC anschließen
2. Windows erkennt "Wakeup Keyboard Device"
3. **Geräte-Manager** → Tastaturen → "Wakeup Keyboard Device"
4. **Eigenschaften** → **Energieverwaltung**
5. ✅ **"Gerät kann den Computer aus dem Ruhezustand aktivieren"** aktivieren

## 📋 Dateistruktur

```
WakeUpTool/
├── esp-idf-wakeup/           # ⭐ AKTIVES PROJEKT (ESP-IDF)
│   ├── main/
│   │   ├── tof_sensor.c      # VL53L0X Treiber (Pololu-Port)
│   │   ├── tof_sensor.h      # ⚙️ Parameter hier anpassen!
│   │   ├── usb.c             # USB HID Keyboard
│   │   └── main.c            # Hauptprogramm
│   └── README.md             # Detaillierte Doku
├── wake_up_tool.yaml         # (Alt: ESPHome-Versuch)
└── README.md                 # Diese Datei
```

## 🐛 Troubleshooting

### Sensor zeigt nur 65535 / 2000mm
- Kabel prüfen (SDA→GPIO14, SCL→GPIO21, XSHUT→GPIO47)
- Nach Kabel-Änderung: Reset-Knopf drücken

### PC wacht nicht auf
- "Gerät kann Computer aktivieren" in Windows aktiviert?
- USB OTG Port verwendet (nicht UART)?
- Im Log prüfen: `remote_wakeup_en=1`?

### Zu viele False-Positives
- `MOTION_THRESHOLD_MM` erhöhen (z.B. 600-800)
- Sensor-Ausrichtung prüfen

## 📚 Links

- **GitHub Repo:** https://github.com/bimberle/esp32-wakeup-tool
- **ESP-IDF Docs:** https://docs.espressif.com/projects/esp-idf/
- **VL53L0X Datasheet:** https://www.st.com/resource/en/datasheet/vl53l0x.pdf

## 📄 Lizenz

MIT - Siehe LICENSE

---

**Status**: ✅ Funktioniert! ESP32 erkennt Bewegung und weckt Windows 11 PC auf.

