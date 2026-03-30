#ifndef MICROSERVICE_HEALTH_H
#define MICROSERVICE_HEALTH_H

#include <ArduinoJson.h>
#include <WString.h>
#include <vector>

struct ServiceHealthEndpoint {
  const char* service;
  const char* label;
  const char* url;
};

struct HealthComponent {
    String name;
    String status;
    String message;
    int response_time_ms;

    bool isHealthy() const {
        return status == "healthy";
    }

    static HealthComponent fromJson(const String& componentName, JsonObjectConst obj) {
        HealthComponent c;
        c.name             = componentName;
        c.status           = obj["status"].as<String>();
        c.message          = obj["message"].as<String>();
        c.response_time_ms = obj["response_time_ms"].as<int>();
        return c;
    }
};

struct MicroserviceHealthResponse {
    String status;
    String timestamp;
    String version;
    std::vector<HealthComponent> components;

    bool isHealthy() const {
        if (status != "healthy") return false;
        for (const HealthComponent& c : components) {
            if (!c.isHealthy()) return false;
        }
        return true;
    }

    const HealthComponent* findComponent(const String& name) const {
        for (const HealthComponent& c : components) {
            if (c.name == name) return &c;
        }
        return nullptr;
    }

    static bool fromJson(const JsonDocument& doc, MicroserviceHealthResponse& out) {
        if (doc.isNull()) return false;

        out.status    = doc["status"].as<String>();
        out.timestamp = doc["timestamp"].as<String>();
        out.version   = doc["version"].as<String>();
        out.components.clear();

        JsonObjectConst comps = doc["components"].as<JsonObjectConst>();
        if (comps.isNull()) return false;

        for (JsonPairConst kv : comps) {
            out.components.push_back(
                HealthComponent::fromJson(kv.key().c_str(), kv.value().as<JsonObjectConst>())
            );
        }

        return true;
    }
};

#endif // MICROSERVICE_HEALTH_H
