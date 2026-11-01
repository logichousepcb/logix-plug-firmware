#pragma once
#include <Arduino.h>

#define FW_VERSION "1.0.0"
// Logix device contract (LogixDIY Home Assistant integration)
#define LOGIX_MANUFACTURER "Logix"
#define LOGIX_MODEL "logixplug"
#define LOGIX_MODEL_NAME "Logix Plug"
#define LOGIX_API_VERSION 1

// ---- Geekbes YM-WS-1 pin map (hard-coded, see DEVICE.md §3) ----
constexpr uint8_t PIN_LED = 12;     // status LED, active low: follows the relay, blinks while offline
constexpr uint8_t PIN_BUTTON = 13;  // push button, active low, internal pull-up
constexpr uint8_t PIN_RELAY = 15;   // relay, active high (boot strap: low at boot)

// ---- Button ----
constexpr uint32_t BUTTON_SCAN_MS = 25;
constexpr uint8_t BUTTON_STABLE_SAMPLES = 3;  // 75 ms debounce

// ---- Time: US Eastern (EST/EDT) ----
#define TZ_POSIX "EST5EDT,M3.2.0,M11.1.0"
#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.nist.gov"

// ---- Network ----
constexpr uint32_t WIFI_AP_FALLBACK_MS = 60000;   // no WiFi this long: bring up the setup AP
constexpr uint32_t WIFI_AP_RETRY_MS = 60000;      // while in AP mode, retry the saved network
constexpr uint32_t WIFI_AP_ATTEMPT_MS = 20000;    // ... for this long each time
constexpr uint32_t MQTT_RETRY_MS = 10000;
