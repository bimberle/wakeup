#!/usr/bin/env python3
import subprocess
import os
import time
import sys

def reset_usb_port():
    """Reset USB-Port auf macOS"""
    port = '/dev/cu.usbserial-110'
    
    # Versuche non-blocking open
    try:
        fd = os.open(port, os.O_RDWR | os.O_NONBLOCK)
        os.close(fd)
        print("Port-Reset erfolgreich")
    except Exception as e:
        print(f"Port-Reset fehlgeschlagen: {e}")

reset_usb_port()
time.sleep(2)

# Starte Upload
print("\nStarte Upload...\n")
result = subprocess.run([
    'pio', 'run', '-e', 'esp32-s3-devkitc-1', '-t', 'upload'
], cwd='/Users/michi/Nextcloud/Haus/Touchscreen/WakeUpTool')

sys.exit(result.returncode)
