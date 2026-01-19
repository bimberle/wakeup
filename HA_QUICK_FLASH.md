# 🚀 ESPHome Flash via Home Assistant (OTA)

Das Board ist bereits im WiFi! Jetzt nur noch ESPHome flashen.

**✅ Board-IP: `192.168.1.154`**

---

## 📋 Voraussetzungen

- ✅ Board im WiFi (bereits erledigt!)
- ✅ Home Assistant läuft
- ✅ ESPHome Add-on installiert

---

## 🎯 Schritt-für-Schritt

### 1️⃣ ESPHome Dashboard öffnen

```
Home Assistant → Settings → Add-ons → ESPHome → Open Web UI
```

Oder direkt: `http://homeassistant.local:6052/`

### 2️⃣ Neue Konfiguration erstellen

1. Klick **"+ NEW DEVICE"**
2. Name eingeben: `wake_up_tool`
3. Klick **"SKIP THIS STEP"** bei Board-Auswahl (wir haben eigene Config)

### 3️⃣ YAML-Konfiguration einfügen

1. ESPHome Dashboard → `wake_up_tool` Karte → **EDIT**
2. **Lösche** den generierten Inhalt
3. **Kopiere** den kompletten Inhalt von `wake_up_tool.yaml` aus diesem Projekt rein
4. **SAVE**

### 4️⃣ Secrets eintragen

1. ESPHome Dashboard → **"SECRETS"** (oben rechts)
2. Füge ein:

```yaml
# WiFi
wifi_ssid: "K.NET"
wifi_password: "GEvHcS04kRH5qoaUC9b7Z"

# API (kannst du neu generieren oder diesen verwenden)
api_encryption_key: "v3gGRdPvvpYx5gvUn/e6DvJxM4YnLBT3XrB6sHeHYNQ="

# OTA
ota_password: "wakeup2026"

# Windows PC MAC (für Wake-on-LAN)
pc_mac_address: "48:21:0b:73:69:43"
```

3. **SAVE**

### 5️⃣ Erstes Flash via TTL-Adapter (vom Mac) ⭐

Das Board läuft noch die Bootstrap-Firmware. Wir flashen ESPHome über den TTL-Adapter vom Mac.

**Schritt A: Binary in ESPHome Dashboard herunterladen**

1. ESPHome Dashboard → `wake_up_tool` Karte
2. Klick auf **drei Punkte (⋮)** → **Install**
3. Wähle: **"Manual download"**
4. Wähle: **"Modern format"** (oder "Legacy format" falls Modern nicht geht)
5. **Warte** bis ESPHome kompiliert (~1-3 Minuten)
6. Browser lädt automatisch die `.bin` Datei herunter
7. Speichere in `~/Downloads/` (z.B. `wake-up-tool.bin`)

**Schritt B: Binary via TTL-Adapter flashen**

1. Board ist mit TTL-Adapter am Mac angeschlossen (`/dev/cu.usbserial-110`)
2. Terminal öffnen und ausführen:

```bash
cd /Users/michi/Nextcloud/Haus/Touchscreen/WakeUpTool
source .venv/bin/activate
esptool.py --chip esp32s3 --port /dev/cu.usbserial-110 --baud 460800 write_flash 0x0 ~/Downloads/wake-up-tool.bin
```

3. **Falls "Connecting..." hängt**: 
   - **BOOT-Taste** gedrückt halten
   - **RESET-Taste** kurz drücken
   - BOOT-Taste loslassen
   - Flash startet automatisch

4. **Warte ~1-2 Minuten** bis Flash fertig ist

**Nach erfolgreichem Flash:**
- Board startet mit ESPHome Firmware neu
- Verbindet sich automatisch mit WiFi
- Port 3232 (API) ist jetzt verfügbar ✅
- ESPHome Dashboard zeigt grünen Status
- **Ab jetzt funktioniert OTA Updates über WiFi!**

### 6️⃣ Verifikation

Nach erfolgreichem Flash:

1. **ESPHome Dashboard**: Device zeigt grünen Status ✅
2. **Home Assistant** → **Settings** → **Devices & Services**
3. ESPHome Integration sollte neues Device zeigen
4. **Entities**:
   - `binary_sensor.motion_detected`
   - `sensor.distance`
   - `light.motion_detected_led`

---

## 🔧 Wake-on-LAN Automation erstellen

### Option A: Über UI

1. **Settings** → **Automations & Scenes** → **Create Automation**
2. **Trigger**: 
   - Type: State
   - Entity: `binary_sensor.motion_detected`
   - To: `on`
3. **Action**:
   - Service: `wake_on_lan.send_magic_packet`
   - MAC: `48:21:0b:73:69:43`

### Option B: YAML

Füge in `automations.yaml` ein:

```yaml
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

---

## ✅ Test

1. **Hand vor Sensor** bewegen
2. **LED** sollte angehen
3. In HA: `binary_sensor.motion_detected` → `on`
4. **PC wacht auf!** 🎉

---

## 🐛 Troubleshooting

### "Es kann keine Verbindung zum ESPHome-Gerät hergestellt werden"
**Ursache:** Das Board läuft noch die Bootstrap-Firmware (nur Webserver auf Port 80), nicht ESPHome mit API (Port 3232).

**Lösung:**
- Flashe das Board **einmalig über TTL-Adapter** mit ESPHome (siehe Schritt 5️⃣)
- Danach ist der API Port 3232 verfügbar ✅
- Dann funktionieren OTA Updates

### esptool.py "Connecting..." hängt
**Lösung:** Board in Bootloader-Modus bringen:
1. **BOOT-Taste** gedrückt halten
2. **RESET-Taste** kurz drücken
3. **BOOT-Taste** loslassen
4. Jetzt `esptool.py` Befehl ausführen

### "Device not reachable"
```bash
# Prüfe ob Board antwortet
curl http://192.168.1.154/
```
→ Falls keine Antwort: Board neu starten

### OTA schlägt fehl
- Firewall Port 3232 freigeben
- Gleiches Netzwerk-Subnet?
- Board näher an Router

### Device erscheint nicht in HA
- ESPHome Dashboard → Logs prüfen
- API Key stimmt überein?
- Neustart HA

---

**Viel Erfolg!** 🚀
