#ifndef CONFIG_H
#define CONFIG_H

// ============ Camera Einstellungen ============
#define CAMERA_MODEL_ESP32S3_CAM // ESP32-S3-CAM Board

// Camera Pin-Konfiguration (für ESP32-S3-CAM) - exakt gemäß Pinout-Diagramm
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    15      // Kamera Takt
#define SIOD_GPIO_NUM    4       // SDA für I2C
#define SIOC_GPIO_NUM    5       // SCL für I2C
#define Y9_GPIO_NUM      16      // D0
#define Y8_GPIO_NUM      17      // D1
#define Y7_GPIO_NUM      18      // D2
#define Y6_GPIO_NUM      12      // D3
#define Y5_GPIO_NUM      10      // D4
#define Y4_GPIO_NUM      8       // D5
#define Y3_GPIO_NUM      9       // D6
#define Y2_GPIO_NUM      11      // D7
#define VSYNC_GPIO_NUM   6       // Vertical Sync
#define HREF_GPIO_NUM    7       // Horizontal Ref
#define PCLK_GPIO_NUM    13      // Pixel Clock

// ============ LED Einstellungen ============
#define LED_PIN          43      // GPIO43 für LED TX (ESP32-S3-CAM)

// ============ Motion Detection Einstellungen ============
#define MOTION_THRESHOLD 60        // Noch höhere Schwelle - nur große Bewegungen
#define FRAME_WIDTH 160            // Bessere Auflösung für zuverlässige Erkennung
#define FRAME_HEIGHT 120
#define FRAME_SIZE FRAMESIZE_QQVGA  // 160x120

// ============ USB HID Einstellungen ============
#define USB_VENDOR_ID 0x303A       // Espressif USB Vendor ID
#define USB_PRODUCT_ID 0x8258      // ESP32-S3 PID
#define USB_DEVICE_CLASS USB_CLASS_HID

// ============ OTA Update Einstellungen ============
#define OTA_PORT 8266
#define OTA_PASSWORD "esp32wakeup"  // WICHTIG: Ändere diesen in Production!

// ============ WiFi Einstellungen ============
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define ENABLE_OTA_UPDATES true

#endif // CONFIG_H
