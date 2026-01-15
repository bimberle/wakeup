# Lessons Learned - ESP32 Wake-Up Tool

## Übersicht Debugging-Journey

Dieses Dokument dokumentiert alle Probleme und deren Lösungen während der ESP32-S3-CAM Entwicklung, damit diese nicht wiederholt werden.

---

## 1. Board-Definition Fehler ⚠️

### Problem
```
Guru Meditation Error: Core / Watchdog Reset
```

### Root Cause
Board wurde als `esp32s3box` definiert, aber Hardware ist `ESP32-S3-CAM` (DevKit).

### Lösung
```ini
[env:esp32-s3-cam]
board = esp32-s3-devkitc-1  # ← KORREKT
# Nicht: board = esp32s3box  (führt zu Bootfails)
```

**Warum**: Die DevKit-Definition ist generischer und kompatibel mit mehreren ESP32-S3 Varianten (CAM, EYE, etc). Die `esp32s3box` Variante erwartet spezifische Hardware-Konfigurationen.

**Lernpunkt**: Immer die offizielle DevKit-Variante verwenden, nicht spezielle Boardtypen, wenn nicht exakt sicher.

---

## 2. USB-Serial Initialization Corruption 🔴

### Problem
```
ESP-ROM: esp32s3-20210327
Setup...
[USB] Initializing...
(H¤J¬(H¤(H¤J¬(H¤H¤(H¤H¤H¤H«J¬§...  ← GARBAGE
```

Sauberes Boot-Output wird von Garbage überschrieben nach USB-Init Attempt.

### Root Cause
1. USB-Stack Initialisierung (`USB.begin()`) interferiert mit Serial/UART Timing
2. Zu kurze Delays zwischen Serial.begin() und USB.begin() 
3. CDC (Communication Device Class) benötigt zusätzliche Init-Zeit

### Lösungsversuche

❌ **Nicht wirksam**:
- Erhöhen von Delays auf 2000ms
- `Serial.flush()` vor USB-Init
- `-DARDUINO_USB_CDC_ON_BOOT=0` (reduziert Garbage, behebt es nicht)

✅ **Wirksam**:
- Komplett auf `ARDUINO_USB_MODE=0` setzen (Serial-only, keine USB-Init)
- USB-HID Features in separater Testfirmware implementieren

### Implementierung
```cpp
// platformio.ini
build_flags =
    -DARDUINO_USB_MODE=0    # Deaktiviert USB-Stack komplett
    
// src/main.cpp  
void setup() {
    Serial.begin(115200);
    delay(100);  // Minimal delay reicht mit USB=0
    Serial.println("Boot OK");  // Kein Garbage mehr
}
```

**Lernpunkt**: USB-Initialisierung auf ESP32 ist komplex. Für Debugging Serial-only verwenden, USB-HID später separat testen.

---

## 3. Kamera-Initialisierung Crash 💥

### Problem
```
[CAMERA] Initializing...
Guru Meditation Error: Core  / ...
```

Kamera-Init verursacht CPU Exception/Reboot.

### Root Cause
**Noch nicht vollständig geklärt**, aber wahrscheinlich:
- Pin-Konflikt zwischen Kamera-GPIO und anderen Subsystemen (I2C, UART, USB?)
- XCLK (Camera External Clock) auf GPIO15 könnte mit anderen Funktionen kollidieren
- PSRAM-Init bei Kamera-Init führt zu Memory-Konflikt

### Lösungsansätze

❌ **Versucht, nicht wirksam**:
- Standardpin-Konfiguration wie in Tutorials
- Erhöhen von Frame-Size auf QVGA
- CDC-Mode Kombinationen
- Verschiedene PIXFORMAT (JPEG vs GRAYSCALE)

✅ **Workaround (aktuell)**:
```cpp
// src/main.cpp - Kamera auskommentiert
// initCamera();  // TODO: Fix Pin Conflicts
```

### Nächste Debug-Schritte
1. Alle Kamera-Pins einzeln testen (GPIO conflict detection)
2. I2C/SIOC/SIOD Pins überprüfen (OV Sensor Config)
3. XCLK-Frequenz reduzieren (20MHz → 10MHz?)
4. Mit minimaem Sensor-Setup starten (nur Takt, keine Daten)

**Lernpunkt**: Kamera-Module benötigen präzise Pin-Konfiguration. Nicht einfach Copy-Paste von Tutorials - jedes Board ist unterschiedlich.

---

## 4. TTL-Adapter vs USB-OTG 🔌

### Findings

| Aspekt | TTL-Adapter | USB-OTG |
|--------|------------|---------|
| **Erkannt** | ✅ /dev/cu.usbserial-110 | ⚠️ ioreg sieht Device |
| **Device-File** | ✅ vorhanden | ❌ kein /dev/tty.* auf macOS |
| **esptool.py** | ✅ Upload funktioniert | ❌ "No serial data received" |
| **Zuverlässigkeit** | ✅ 100% | ⚠️ Unbekannt |
| **CDC/Serial** | Natürlich | Braucht ARDUINO_USB_CDC=1 |

### Entscheidung
- **Für Entwicklung/Debugging**: TTL-Adapter (zuverlässig)
- **Für Production**: USB-OTG nur für USB-HID, separate TTL für Debugging

### macOS Spezifikum
`ARDUINO_USB_CDC_ON_BOOT=1` erstellt kein Device-File auf macOS, obwohl Device enumerated wird. Das ist ein macOS-Treiber-Thema, nicht ein Board-Problem.

**Lernpunkt**: USB-OTG auf macOS ist komplizierter als erwartet. TTL ist der pragmatische Weg für schnelle Entwicklung.

---

## 5. Build-Umgebung Cleanup 🧹

### Problem
Mehrere fehlgeschlagene Builds hinterlassen beschädigte `.pio/` Build-Artefakte.

### Lösung
```bash
# Sauberer Rebuild
rm -rf .pio/build/esp32-s3-cam/
pio run -e esp32-s3-cam -t clean
pio run -e esp32-s3-cam
```

**Lernpunkt**: Nach großen Änderungen (Board, Flags) einen Clean Build machen.

---

## 6. Serial Output Encoding 📝

### Finding
Garbage-Output mit Bytes wie `(H¤J¬` deutet oft auf:
1. Baudrate-Mismatch (NICHT in diesem Fall - war richtig)
2. Hardware-Debug-Output vom Bootloader (NICHT ausschaltbar)
3. **USB-Init Interferenz mit UART** (das war es hier!)

### Debugging Tipps
```bash
# Hex-Dump um echte Bytes zu sehen
timeout 5 cat /dev/cu.usbserial-110 | od -c | head

# Nur printable chars
timeout 5 cat /dev/cu.usbserial-110 | tr -cd '[:print:]\n'
```

**Lernpunkt**: Nicht zu schnell Baudrate-Probleme annehmen - oft sind es Hardware-Timing-Issues.

---

## 7. Debugging Workflow 🔧

### Erfolgreicher Ansatz (für Future)
1. **Minimal funktionierende Baseline aufbauen**
   - Serial.begin() + einfache Ausgabe
   - Ohne Libs, ohne USB, ohne Peripherie
   - ✅ Ziel: Clean Boot-Output

2. **Dann Features inkrementell hinzufügen**
   - Eine Lib pro Iteration
   - Nach jedem Step: Upload + Serial-Test
   - ✅ Ziel: Feststellen welche Lib/Feature Probleme verursacht

3. **Probleme isolieren**
   - Betroffene Lib auskommentieren
   - Ohne diese bootet es? → Lib-Problem
   - Mit dieser ist es kaputt? → Pin-/Init-Konflikt

### Beispiel Iteration (für dieses Projekt)
```
Iteration 1: Serial-only Boot ✅
Iteration 2: + USB-HID (problematisch) → auskommentiert
Iteration 3: + Kamera (Crash) → auskommentiert  
Iteration 4: + Motion-Detection (ausstehend)
Iteration 5: Integration & Test
```

**Lernpunkt**: Nicht alles auf einmal implementieren - Feature by Feature.

---

## Nächste Schritte für nächsten Entwickler

1. **Kamera-Debugging** (höchste Priorität)
   - Pin-Konflikt-Analyse durchführen
   - Möglicherweise andere Pins verwenden
   - Oder PSRAM-Konflikt lösen

2. **USB-HID Test-Firmware**
   - Separate Firmware ohne Kamera
   - Nur USB-HID Initialisierung
   - Test: Tastatur von Windows erkannt?

3. **Motion Detection**
   - Ohne Kamera: Dummy-Motion erzeugen
   - Mit Kamera: Nach Kamera-Fix

4. **Integration**
   - Alle Features kombinieren
   - PC Wake-Up Test durchführen
   - Sleep-Handling testen

---

## Zusammenfassung der wichtigsten Erkenntnisse

| Fehler | Ursache | Lösung |
|--------|--------|--------|
| Guru Meditation | Falsches Board-Type | `esp32-s3-devkitc-1` verwenden |
| Serial Garbage | USB-Init Timing | USB komplett ausschalten zum Debuggen |
| Kamera Crash | Pin-Konflikt | Noch zu debuggen - aktuell auskommentiert |
| USB nicht erkannt | macOS Treiber | TTL verwenden, USB separat testen |

---

**Letzte Aktualisierung**: 15. Januar 2026
**Status**: Erste stabile Version mit Serial-TTL baseline
