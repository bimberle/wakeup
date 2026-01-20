/**
 * USB HID Keyboard für ESP32-S3
 * Verwendet ESP-IDF USB HID API
 * Sendet Tastendruck um Bildschirm aufzuwecken
 */

#pragma once

#include "esphome.h"

// ESP-IDF USB Headers
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_private/usb_phy.h"
#include "tusb.h"

static bool usb_keyboard_initialized = false;
static usb_phy_handle_t phy_hdl = NULL;

// HID Keyboard Report Descriptor (minimal)
#define TUD_HID_REPORT_DESC_KEYBOARD_MINIMAL(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                    ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD )                    ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                    ,\
    /* Modifiers */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
    HID_USAGE_MIN  ( 224 )                                         ,\
    HID_USAGE_MAX  ( 231 )                                         ,\
    HID_LOGICAL_MIN( 0 )                                           ,\
    HID_LOGICAL_MAX( 1 )                                           ,\
    HID_REPORT_COUNT( 8 )                                          ,\
    HID_REPORT_SIZE ( 1 )                                          ,\
    HID_INPUT      ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )      ,\
    /* Reserved byte */ \
    HID_REPORT_COUNT( 1 )                                          ,\
    HID_REPORT_SIZE ( 8 )                                          ,\
    HID_INPUT      ( HID_CONSTANT )                                ,\
    /* Keycodes */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
    HID_USAGE_MIN  ( 0   )                                         ,\
    HID_USAGE_MAX  ( 255 )                                         ,\
    HID_LOGICAL_MIN( 0   )                                         ,\
    HID_LOGICAL_MAX_N( 255, 2 )                                    ,\
    HID_REPORT_COUNT( 6 )                                          ,\
    HID_REPORT_SIZE ( 8 )                                          ,\
    HID_INPUT      ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )         ,\
  HID_COLLECTION_END

static const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD_MINIMAL()
};

// TinyUSB Callbacks (werden von tusb benötigt)
extern "C" {

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
    (void) instance;
    return hid_report_descriptor;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                                hid_report_type_t report_type,
                                uint8_t *buffer, uint16_t reqlen) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                            hid_report_type_t report_type,
                            uint8_t const *buffer, uint16_t bufsize) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;
}

}  // extern "C"

/**
 * USB Keyboard initialisieren
 */
void usb_keyboard_init() {
    if (usb_keyboard_initialized) return;
    
    ESP_LOGI("usb_keyboard", "Initialisiere USB HID Keyboard...");
    
    // USB PHY konfigurieren
    usb_phy_config_t phy_config = {
        .controller = USB_PHY_CTRL_OTG,
        .target = USB_PHY_TARGET_INT,
        .otg_mode = USB_OTG_MODE_DEVICE,
        .otg_speed = USB_PHY_SPEED_FULL,
        .ext_io_conf = NULL,
        .otg_io_conf = NULL,
    };
    
    esp_err_t ret = usb_new_phy(&phy_config, &phy_hdl);
    if (ret != ESP_OK) {
        ESP_LOGE("usb_keyboard", "USB PHY init failed: %s", esp_err_to_name(ret));
        return;
    }
    
    // TinyUSB initialisieren
    if (!tusb_init()) {
        ESP_LOGE("usb_keyboard", "TinyUSB init failed");
        return;
    }
    
    usb_keyboard_initialized = true;
    ESP_LOGI("usb_keyboard", "USB HID Keyboard initialisiert");
}

/**
 * Taste drücken um Bildschirm aufzuwecken
 */
void usb_keyboard_wake_screen() {
    if (!usb_keyboard_initialized) {
        ESP_LOGW("usb_keyboard", "USB nicht initialisiert!");
        return;
    }
    
    // TinyUSB task ausführen
    tud_task();
    
    if (!tud_mounted()) {
        ESP_LOGW("usb_keyboard", "USB nicht verbunden mit Host!");
        return;
    }
    
    if (!tud_hid_ready()) {
        ESP_LOGW("usb_keyboard", "HID nicht bereit!");
        return;
    }
    
    ESP_LOGI("usb_keyboard", "Sende Tastendruck (LEFT_SHIFT)...");
    
    // Keyboard Report: [modifier, reserved, key1-6]
    uint8_t keycode[6] = {0};
    
    // Shift drücken
    tud_hid_keyboard_report(0, KEYBOARD_MODIFIER_LEFTSHIFT, keycode);
    vTaskDelay(pdMS_TO_TICKS(50));
    
    // Loslassen
    tud_hid_keyboard_report(0, 0, keycode);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Nochmal
    tud_hid_keyboard_report(0, KEYBOARD_MODIFIER_LEFTSHIFT, keycode);
    vTaskDelay(pdMS_TO_TICKS(50));
    tud_hid_keyboard_report(0, 0, keycode);
    
    ESP_LOGI("usb_keyboard", "Tastendruck gesendet");
}

/**
 * Custom ESPHome Component Wrapper
 */
class USBKeyboardComponent : public Component {
 public:
  void setup() override {
    usb_keyboard_init();
  }
  
  void loop() override {
    // TinyUSB task regelmäßig ausführen
    if (usb_keyboard_initialized) {
      tud_task();
    }
  }
  
  void wake_screen() {
    usb_keyboard_wake_screen();
  }
  
  float get_setup_priority() const override {
    return setup_priority::AFTER_WIFI;
  }
};
