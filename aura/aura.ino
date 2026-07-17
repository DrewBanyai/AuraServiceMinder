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
#include "service_monitor_ui.h"
#include "weather_app_ui.h"

void fetch_api_health_checks();

static const ServiceHealthEndpoint HEALTH_ENDPOINTS[SERVICE_TRACKING_COUNT] = {
  { "Anti-Cheat", "https://api-staging.battlecreek.games/api/v1/bcg-anti-cheat-validation-service/health/detailed", "https://api.battlecreek.games/api/v1/bcg-anti-cheat-validation-service/health/detailed"         },
  { "Identity",   "https://api-staging.battlecreek.games/api/v1/identity/health/detailed", "https://api.battlecreek.games/api/v1/identity/health/detailed"                                  },
  { "Wallet",     "https://api-staging.battlecreek.games/api/v1/wallet/health/detailed", "https://api.battlecreek.games/api/v1/wallet/health/detailed"                                    },
};

static void screen_touch_wake_callback(lv_indev_data_t *data)
{
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
}

void setup() {
  Serial.begin(115200);
  delay(100);

  initTFT();
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);

  lv_init();

  initTouchscreen();
  initInputDevice();
  addMouseClickCallback(screen_touch_wake_callback);

  loadWeatherPrefs();

  // Check for Wi-Fi config and request it if not available
  WiFiManager wm;
  wm.setAPCallback(apModeCallback);
  wm.autoConnect(DEFAULT_CAPTIVE_SSID);

  lv_timer_create(update_clock, 1000, NULL);

  lv_obj_clean(lv_scr_act());
  ServiceMonitorUI::CreateUI();
  //create_weather_ui();
  Serial.println("Screen Initialized");
  //fetch_and_update_weather();
  fetch_api_health_checks();
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

static int get_service_health(const char* serviceName, const char* serviceURL) {
  HTTPClient http;
  http.begin(serviceURL);
  int code = http.GET();
  int healthValue = 2;

  Serial.print(serviceName);

  if (code > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(4096);
    MicroserviceHealthResponse health;

    if (deserializeJson(doc, payload) == DeserializationError::Ok && MicroserviceHealthResponse::fromJson(doc, health)) {
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
      if (health.isHealthy()) {
        healthValue = 0;
        Serial.println("  >> All components healthy.");
      }
      else {
        healthValue = 1;
        Serial.println("  >> WARNING: One or more components degraded.");
      }
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
  return healthValue;
}

void fetch_api_health_checks() {
  static ServiceMonitorUI::ServiceMonitorStatus statusResults[SERVICE_TRACKING_COUNT];
  for (int i = 0; i < SERVICE_TRACKING_COUNT; ++i) {
    statusResults[i].ServiceName = HEALTH_ENDPOINTS[i].serviceName;
    statusResults[i].StagingStatus = get_service_health(statusResults[i].ServiceName.c_str(), HEALTH_ENDPOINTS[i].stagingURL);
    statusResults[i].ProdStatus = get_service_health(statusResults[i].ServiceName.c_str(), HEALTH_ENDPOINTS[i].prodURL);
  }
  ServiceMonitorUI::UpdateUI(statusResults);
}