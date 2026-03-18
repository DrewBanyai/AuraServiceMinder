#ifndef WEB_H
#define WEB_H

#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DEFAULT_CAPTIVE_SSID "Aura"

static DynamicJsonDocument geoDoc(8 * 1024);
static JsonArray geoResults;

String urlencode(const String &str) {
  String encoded = "";
  char buf[5];
  for (size_t i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    // Unreserved characters according to RFC 3986
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += c;
    } else {
      // Percent-encode others
      sprintf(buf, "%%%02X", (unsigned char)c);
      encoded += buf;
    }
  }
  return encoded;
}

void do_geocode_query(const char *q, bool *success) {
    geoDoc.clear();
    String url = String("https://geocoding-api.open-meteo.com/v1/search?name=") + urlencode(q) + "&count=15";

    HTTPClient http;
    http.begin(url);
    if (http.GET() == HTTP_CODE_OK) {
        Serial.println("Completed location search at open-meteo: " + url);
        auto err = deserializeJson(geoDoc, http.getString());
        if (!err) {
            geoResults = geoDoc["results"].as<JsonArray>();
            (*success) = true;
        } else {
            Serial.println("Failed to parse search response from open-meteo: " + url);
        }
    } else {
        Serial.println("Failed location search at open-meteo: " + url);
    }
    http.end();
}

void reset_wifi() {
    WiFiManager wm;
    wm.resetSettings();
}

String get_forecast_url(char* latitude, char* longitude)
{
    return String("http://api.open-meteo.com/v1/forecast?latitude=")
        + latitude + "&longitude=" + longitude
        + "&current=temperature_2m,apparent_temperature,is_day,weather_code"
        + "&daily=temperature_2m_min,temperature_2m_max,weather_code"
        + "&hourly=temperature_2m,precipitation_probability,is_day,weather_code"
        + "&forecast_hours=7"
        + "&timezone=auto";
}

bool fetch_weather(String url, DynamicJsonDocument *jsonDoc) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi no longer connected. Attempting to reconnect...");
        WiFi.disconnect();
        WiFiManager wm;  
        wm.autoConnect(DEFAULT_CAPTIVE_SSID);
        delay(1000);  
        if (WiFi.status() != WL_CONNECTED) { 
            Serial.println("WiFi connection still unavailable.");
            return false;   
        }
        Serial.println("WiFi connection reestablished.");
    }

    bool success = false;
    HTTPClient http;
    http.begin(url);

    if (http.GET() == HTTP_CODE_OK) {
        Serial.println("Updated weather from open-meteo: " + url);

        String payload = http.getString();
        if (deserializeJson((*jsonDoc), payload) == DeserializationError::Ok) {
            success = true;
        } else {
            Serial.println("JSON parse failed on result from " + url);
        }
    } else {
        Serial.println("HTTP GET failed at " + url);
    }
    http.end();
    return success;
}

#endif // WEB_H