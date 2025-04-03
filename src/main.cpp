#include "Arduino.h"
#include "M5Stack.h"
#include "esp_camera.h"
#include <WiFi.h>

#define CAMERA_MODEL_M5STACK_PSRAM
#include "camera_pins.h"

#define RUN_IN_AP_MODE false  // Eğer ESP32'nin kendi WiFi ağı açmasını istiyorsan true yap
const char* ssid = "Wohnheim Jahnplatz";  // WiFi ismi
const char* password = "start123";        // WiFi şifresi

void startCameraServer();

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println("\n=== ESP32 Timer Cam Başlatılıyor ===");

    // Kamera yapılandırması
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
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if (psramFound()) {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // Kamerayı başlat
    Serial.println("📸 Kamera başlatılıyor...");
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("❌ Kamera başlatma hatası! Hata kodu: 0x%x\n", err);
        return;
    }
    Serial.println("✅ Kamera başarıyla başlatıldı!");

    // Wi-Fi Bağlantısı
    if (RUN_IN_AP_MODE) {
        WiFi.softAP(ssid, password);
        Serial.print("📡 AP Modu Aktif! Bağlanmak için: http://");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.printf("🔄 '%s' WiFi ağına bağlanılıyor...\n", ssid);
        WiFi.begin(ssid, password);

        int maxWait = 20; // 10 saniye bekler (20 * 500ms)
        while (WiFi.status() != WL_CONNECTED && maxWait > 0) {
            delay(500);
            Serial.print(".");
            maxWait--;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n✅ WiFi bağlantısı başarılı!");
            Serial.print("📸 Kamera görüntüsünü izlemek için tarayıcıdan şu adrese git: http://");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("\n❌ WiFi bağlantısı başarısız! Lütfen SSID ve şifreyi kontrol et.");
        }
    }

    startCameraServer();
}

void loop() {
    delay(10000);
}
