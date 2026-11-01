#pragma once

// Saved settings (LittleFS) and device identity. On the very first boot after Tasmota the
// WiFi and MQTT settings and the device name are imported from Tasmota's settings sector, so
// the plug comes back on the same network, under the same name, without visiting the setup AP.

#include <Arduino.h>

namespace Settings {

struct __attribute__((packed)) Data {
  uint32_t magic;
  char name[33];  // friendly device name, also the name of the plug's one outlet
  char ssid[33];
  char pass[65];
  char admin[65];  // web/API admin password, "" = none
  char mqttHost[65];  // "" = MQTT off
  uint16_t mqttPort;
  char mqttUser[65];
  char mqttPass[65];
  char mqttTopic[65];  // "" = default <model>/<id>
  uint8_t tasmotaImported;
  uint8_t reserved[7];
  uint32_t checksum;
};

// Call first in setup(), before Storage::begin(): finds Tasmota's settings (if this is the first
// boot after Tasmota) while they are still intact.
void readTasmota();
// Relay state saved by Tasmota (bit 0), or -1 if there was no Tasmota.
int tasmotaOutlets();
void begin();
Data &get();
bool save();

// Lowercase MAC without separators, e.g. "68c63a97c4c9"; the stable unique id.
String deviceId();
// Friendly name, e.g. "Logix Plug C4C9" (default) or "Entry Table Lamp".
String name();
// The outlet is named after the device: "<name>"
String outletName(uint8_t index);
// Hostname derived from the name, e.g. "entry-table-lamp": DHCP, mDNS, MQTT client id.
String hostname();
String hostnameFor(const String &name);
// Validate and save a new name. Returns an error message, or "" on success.
String setName(const String &name);
String defaultName();

}  // namespace Settings
