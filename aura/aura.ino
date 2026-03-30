#include <Arduino.h>
#include <time.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <Preferences.h>
#include "esp_system.h"

#include "declarations.h"
#include "screen_control.h"
#include "web.h"
#include "microservice_health.h"
#include "weather_app_ui.h"

void fetch_api_health_checks();

static const ServiceHealthEndpoint HEALTH_ENDPOINTS[] = {
  { "Anti-Cheat", "Staging",    "https://api-staging.battlecreek.games/api/v1/bcg-anti-cheat-validation-service/health/detailed" },
  { "Anti-Cheat", "Production", "https://api.battlecreek.games/api/v1/bcg-anti-cheat-validation-service/health/detailed"         },
  { "Identity",   "Staging",    "https://api-staging.battlecreek.games/api/v1/identity/health/detailed"                          },
  { "Identity",   "Production", "https://api.battlecreek.games/api/v1/identity/health/detailed"                                  },
  { "Wallet",     "Staging",    "https://api-staging.battlecreek.games/api/v1/wallet/health/detailed"                            },
  { "Wallet",     "Production", "https://api.battlecreek.games/api/v1/wallet/health/detailed"                                    },
};

static void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data) {
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();

    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;

    // Handle touch during dimmed screen
    if (night_mode_active) {
      // Temporarily wake the screen for 15 seconds
      analogWrite(LCD_BACKLIGHT_PIN, weather_app_prefs.getUInt("brightness", 128));
    
      if (temp_screen_wakeup_timer) {
        lv_timer_del(temp_screen_wakeup_timer);
      }
      temp_screen_wakeup_timer = lv_timer_create(handle_temp_screen_wakeup_timeout, 15000, NULL);
      lv_timer_set_repeat_count(temp_screen_wakeup_timer, 1); // Run only once
      Serial.println("Woke up screen. Setting timer to turn of screen after 15 seconds of inactivity.");

      if (!temp_screen_wakeup_active) {
          // If this is the wake-up tap, don't pass this touch to the UI - just undim the screen
          temp_screen_wakeup_active = true;
          data->state = LV_INDEV_STATE_RELEASED;
          return;
      }

      temp_screen_wakeup_active = true;
    }

    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = x;
    data->point.y = y;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

static void initInputDevice()
{
  lv_indev_t *inputDevice = lv_indev_create();
  lv_indev_set_type(inputDevice, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(inputDevice, touchscreen_read);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  initTFT();
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);

  lv_init();

  initTouchscreen();
  initInputDevice();

  loadWeatherPrefs();

  // Check for Wi-Fi config and request it if not available
  WiFiManager wm;
  wm.setAPCallback(apModeCallback);
  wm.autoConnect(DEFAULT_CAPTIVE_SSID);

  lv_timer_create(update_clock, 1000, NULL);

  lv_obj_clean(lv_scr_act());
  create_weather_ui();
  Serial.println("Screen Initialized");
  fetch_and_update_weather();
  fetch_api_health_checks();
}

void apModeCallback(WiFiManager *mgr) {
  wifi_splash_screen();
  flush_wifi_splashscreen();
}

void loop() {
  lv_timer_handler();
  static uint32_t last = millis();

  if (millis() - last >= UPDATE_INTERVAL) {
    fetch_and_update_weather();
    last = millis();
  }

  lv_tick_inc(5);
  delay(5);
}

static void print_service_health(const char* service, const char* label, const char* url) {
  HTTPClient http;
  http.begin(url);
  int code = http.GET();

  Serial.print(service);
  Serial.print(" ");
  Serial.println(label);

  if (code > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(4096);
    MicroserviceHealthResponse health;

    if (deserializeJson(doc, payload) == DeserializationError::Ok
        && MicroserviceHealthResponse::fromJson(doc, health)) {
      Serial.print("  Overall:   "); Serial.println(health.status);
      Serial.print("  Version:   "); Serial.println(health.version);
      Serial.print("  Timestamp: "); Serial.println(health.timestamp);
      for (const HealthComponent& c : health.components) {
        Serial.print("  ");
        Serial.print(c.name);
        Serial.print(": ");
        Serial.print(c.status);
        Serial.print("  (");
        Serial.print(c.response_time_ms);
        Serial.print("ms)  ");
        Serial.println(c.message);
      }
      Serial.println(health.isHealthy() ? "  >> All components healthy." : "  >> WARNING: One or more components degraded.");
    } else {
      Serial.println("  ERROR: Failed to deserialize response.");
      Serial.println(payload);
    }
  } else {
    Serial.print("  ERROR: HTTP GET failed — ");
    Serial.println(http.errorToString(code));
  }

  http.end();
  Serial.println();
}

void fetch_api_health_checks() {
  for (const ServiceHealthEndpoint& e : HEALTH_ENDPOINTS) {
    print_service_health(e.service, e.label, e.url);
  }
}