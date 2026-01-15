# ESP32-S3-CAM Wake-Up Tool

Automatisches Wecken eines Windows PCs durch Bewegungserkennung via ESP32-S3-CAM Board mit USB-Tastatur-Emulation.

## 🎯 Projekt-Status

| Feature | Status | Notizen |
|---------|--------|---------|
| Hardware-Boot | ✅ Funktioniert | Via TTL-Adapter |
| Serial-Debugging | ✅ Funktioniert | 115200 Baud über TTL |
| USB-OTG Erkennung | ⚠️ Erkannt | Aber keine CDC auf macOS |
| USB-HID Tastatur | 🔄 In Arbeit | Braucht separate Testfirmware |
| Kamera-Init | ❌ Crash | Pin-Konflikte (Debugging ausstehend) |
| Bewegungserkennung | 🔄 In Arbeit | Nach Kamera-Fix |
| PC Wake-Up | 🔄 In Arbeit | Nach USB-HID & Motion-Detection |

## 🔧 Hardware-Setup

### Board
- **ESP32-S3-CAM** (wichtig: NOT esp32s3box!)
- 8MB PSRAM
- OV2640 Kamera-Sensor

### Anschlüsse
- **TTL-Adapter** (für Debugging):
  - GND → GND
  - TX (Adapter) → RX0 (ESP32, GPIO44)
  - RX (Adapter) → TX0 (ESP32, GPIO43)
  - `/dev/cu.usbserial-110` (macOS)

- **USB-OTG** (für Production):
  - USB-C Port (via OTG-Kabel zu PC)
  - Noch nicht vollständig getestet

### Buttons
- **Boot**: GPIO0 (zum Bootloader-Modus)
- **Reset**: RST (zum Neustarten)

## 🚀 Schnellstart

### Requirements
```bash
pip install platformio
# oder via Homebrew: brew install platformio
```

### Build
```bash
cd WakeUpTool
pio run -e esp32-s3-cam
```

### Upload (über TTL-Adapter)
```bash
pio run -t upload -e esp32-s3-cam --upload-port /dev/cu.usbserial-110
```

### Serial Monitor
```bash
# Option 1: PlatformIO
pio device monitor -b 115200 --port /dev/cu.usbserial-110

# Option 2: Raw (macOS)
cat /dev/cu.usbserial-110
```

## 📋 Dateistruktur

```
WakeUpTool/
├── src/
│   └── main.cpp          # Hauptprogramm (Setup & Loop)
├── include/
│   ├── config.h          # Pin-Definitionen & Konstanten
│   ├── motion_detection.h # Bewegungserkennung (noch mit Kamera-Init)
│   ├── usb_keyboard.h    # USB-HID Wrapper (noch disabled)
│   └── ota_update.h      # OTA-Update (WIP)
├── platformio.ini        # PlatformIO Konfiguration
├── .github/
│   └── copilot-instructions.md  # Für Copilot/LLM-Assistenten
├── LESSONS_LEARNED.md    # ⭐ Detaillierte Erkenntnisse & Fehler
└── README.md             # Dieses Dokument
```

## 🐛 Bekannte Probleme & Lösungen

### Problem: "Guru Meditation Error" beim Boot
**Ursache**: Falsches Board-Type  
**Lösung**: Stelle sicher dass `platformio.ini` hat:
```ini
board = esp32-s3-devkitc-1  # NICHT esp32s3box!
```

### Problem: Serial Output mit Garbage
**Ursache**: USB-Stack Initialisierung stört UART-Timing  
**Lösung**: Nutze `ARDUINO_USB_MODE=0` zum Debuggen:
```ini
build_flags = -DARDUINO_USB_MODE=0
```

### Problem: Kamera Init crasht
**Ursache**: Pin-Konflikte (noch nicht vollständig analysiert)  
**Lösung**: Kamera aktuell auskommentiert. Siehe [LESSONS_LEARNED.md](LESSONS_LEARNED.md#3-kamera-initialisierung-crash-)

### Problem: USB-OTG wird nicht als seriell erkannt (macOS)
**Ursache**: macOS braucht CDC-Treiber  
**Lösung**: Nutze TTL-Adapter für Entwicklung

Für detailliertere Lösungsansätze siehe **[LESSONS_LEARNED.md](LESSONS_LEARNED.md)** ⭐

## 📚 Für Copilot / AI-Assistenten

Dieses Projekt enthält detaillierte Instruktionen für KI-Assistenten:
- [.github/copilot-instructions.md](.github/copilot-instructions.md) - Kurze technische Checklist
- [LESSONS_LEARNED.md](LESSONS_LEARNED.md) - Ausführliches Fehler-Journal (wichtig!)

**Bitte lesen vor Code-Änderungen!** Das spart Debugging-Zeit erheblich.

## 🎯 Nächste Entwicklungs-Schritte

1. **[HIGH PRIORITY]** Kamera-Debugging
   - Pin-Konflikt isolieren
   - Oder alternative Pins testen
   
2. **[HIGH PRIORITY]** USB-HID Tastatur Test
   - Separate Testfirmware ohne Kamera
   - Windows Geräte-Erkennung prüfen

3. **[MEDIUM PRIORITY]** Bewegungserkennung
   - Nach Kamera-Fix implementieren
   - Oder mit Dummy-Motion debuggen

4. **[LOW PRIORITY]** PC Wake-Up Integration
   - Timing testen (Verzögerung akzeptabel?)
   - Sleep-Verhalten testen

## 📞 Support / Debugging

Falls du auf neue Probleme stößt:

1. **Check [LESSONS_LEARNED.md](LESSONS_LEARNED.md)** - Viele bekannte Fehler sind dort dokumentiert
2. **Lies [.github/copilot-instructions.md](.github/copilot-instructions.md)** - Critical Learnings
3. **Führe `pio run -v`** aus um detaillierte Build-Ausgabe zu sehen
4. **Teste Serial-Output**: `timeout 5 cat /dev/cu.usbserial-110 | od -c`

## 📄 Lizenz

Proprietär - Privates Projekt

## 👨‍💻 Credits

Entwicklung: Michi + GitHub Copilot  
Debugging-Insights dokumentiert: 15. Januar 2026

---

**Tip**: Vor größeren Änderungen `LESSONS_LEARNED.md` lesen - spart viele Stunden Debugging! 🚀
