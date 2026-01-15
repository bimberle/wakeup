#ifndef MOTION_DETECTION_H
#define MOTION_DETECTION_H

#include <Arduino.h>
#include "esp_camera.h"
#include "config.h"

class MotionDetector {
private:
  uint8_t *prevFrame;
  uint8_t *currFrame;
  bool initialized;
  uint32_t frameCount;

public:
  MotionDetector() : initialized(false), frameCount(0) {
    prevFrame = nullptr;
    currFrame = nullptr;
  }

  /**
   * Initialisiert den Motion Detector mit notwendigen Puffern
   */
  void begin() {
    Serial.println("Initializing Motion Detector...");
    
    size_t frameSize = FRAME_WIDTH * FRAME_HEIGHT;
    prevFrame = (uint8_t *)malloc(frameSize);
    currFrame = (uint8_t *)malloc(frameSize);
    
    if (!prevFrame || !currFrame) {
      Serial.println("ERROR: Failed to allocate frame buffers!");
      return;
    }
    
    initialized = true;
    Serial.printf("Motion Detector ready. Frame buffer size: %d bytes\n", frameSize);
  }

  /**
   * Erfasst aktuellen Frame und vergleicht mit vorherigem
   * @return true wenn Bewegung erkannt wird
   */
  bool detectMotion() {
    if (!initialized) return false;

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("ERROR: Failed to get camera frame!");
      return false;
    }

    // Konvertiere JPEG zu Grayscale für schnellere Verarbeitung
    // (Simplified: hier nur Bewegungserkennung auf Basis der Helligkeit)
    uint32_t changes = 0;
    uint32_t totalPixels = FRAME_WIDTH * FRAME_HEIGHT;
    
    if (frameCount > 0) {
      // Vereinfachte Bewegungserkennung durch Pixelvergleich
      // In Produktion sollte ein sophistizierteres Verfahren verwendet werden
      for (uint32_t i = 0; i < totalPixels; i++) {
        uint32_t idx = (i * fb->len) / totalPixels;
        if (idx < fb->len) {
          uint8_t diff = abs(currFrame[i] - fb->buf[idx]);
          if (diff > 30) { // Threshold für Pixeländerung
            changes++;
          }
        }
      }
    }

    // Speichere aktuellen Frame für nächsten Vergleich
    memcpy(prevFrame, currFrame, FRAME_WIDTH * FRAME_HEIGHT);
    if (fb->len <= FRAME_WIDTH * FRAME_HEIGHT) {
      memcpy(currFrame, fb->buf, fb->len);
    }

    esp_camera_fb_return(fb);
    frameCount++;

    // Bewegung erkannt wenn >30% Pixelveränderungen
    bool motionDetected = (changes > (totalPixels * MOTION_THRESHOLD / 100));
    
    if (motionDetected && frameCount % 10 == 0) { // Log alle 10 Frames
      Serial.printf("Motion detected! Changes: %u/%u (%.1f%%)\n", 
                    changes, totalPixels, 
                    (float)changes * 100 / totalPixels);
    }

    return motionDetected;
  }

  uint32_t getFrameCount() const {
    return frameCount;
  }

  void reset() {
    frameCount = 0;
  }

  ~MotionDetector() {
    if (prevFrame) free(prevFrame);
    if (currFrame) free(currFrame);
  }
};

// ============ Camera Initialisierung ============

/**
 * Initialisiert die ESP32 Kamera
 * @return true wenn erfolgreich
 */
bool initCamera() {
  Serial.println("Initializing camera...");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAME_SIZE;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.fb_count = 1;

  // Camera initialisieren
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return false;
  }

  // Sensor-Einstellungen optimieren
  sensor_t *s = esp_camera_sensor_get();
  if (s != nullptr) {
    s->set_framesize(s, FRAME_SIZE);
    s->set_quality(s, 10);
    s->set_brightness(s, 0);
    s->set_contrast(s, 0);
    s->set_saturation(s, 0);
    s->set_special_effect(s, 0);
    s->set_whitebal(s, 1);
    s->set_awb_gain(s, 1);
    s->set_wb_mode(s, 0);
    s->set_exposure_ctrl(s, 1);
    s->set_aec_value(s, 300);
    s->set_aec2(s, 1);
    s->set_ae_level(s, 0);
    s->set_agc_gain(s, 0);
    s->set_gainceiling(s, GAINCEILING_2X);
    s->set_bpc(s, 1);
    s->set_wpc(s, 1);
    s->set_raw_gma(s, 1);
    s->set_lenc(s, 1);
    s->set_hmirror(s, 0);
    s->set_vflip(s, 0);
    s->set_dcw(s, 1);
  }

  Serial.println("Camera initialized successfully!");
  return true;
}

#endif // MOTION_DETECTION_H
