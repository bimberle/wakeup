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

### 5️⃣ OTA Flash starten ⭐

1. ESPHome Dashboard → **wake_up_tool** Karte
2. Klick auf **drei Punkte (⋮)** → **Install**
3. Wähle: **"Wirelessly"** ← Das ist der OTA-Flash!
4. Gib die IP ein: **`192.168.1.154`**
5. Klick **"CONNECT"**

**Flash startet!** (~1-2 Minuten)

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
