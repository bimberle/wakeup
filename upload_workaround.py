#!/usr/bin/env python3
"""
Workaround für CH340-USB-Seriell-Problem auf macOS
Öffnet und schließt den Port mehrmals um den Lock zu brechen
"""
import os
import time
import subprocess
import sys

def force_port_open():
    """Erzwinge Port-Öffnung durch mehrfaches Auf/Zuschließen"""
    port = '/dev/cu.usbserial-110'
    
    for i in range(5):
        try:
            # Öffne und schließe Port sofort
            fd = os.open(port, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
            os.close(fd)
            print(f"Port-Cycle {i+1}/5... OK")
            time.sleep(0.5)
        except Exception as e:
            print(f"Port-Cycle {i+1}/5... FAILED: {e}")
            time.sleep(1)

# Versuche Port mehrfach zu öffnen
print("Versuche Port-Lock zu brechen...")
force_port_open()

time.sleep(3)

print("\nStarte PlatformIO Upload...\n")

# Starte Upload
result = subprocess.run([
    'pio', 'run', '-e', 'esp32-s3-devkitc-1', '-t', 'upload'
], cwd='/Users/michi/Nextcloud/Haus/Touchscreen/WakeUpTool')

sys.exit(result.returncode)
