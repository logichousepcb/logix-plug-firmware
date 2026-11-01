#pragma once

// The plug's one outlet (relay), its button and its LED.
//
// The button is scanned from a timer, not from loop(), so it keeps toggling the relay while
// the network code is busy (WiFi connecting, MQTT broker unreachable, HTTP request). The
// relay state is written to flash on every change and restored first thing at boot.
//
// The API keeps the Logix Outlet shape (arrays of outlets and buttons) with one entry each,
// so the same integration code handles both devices.

#include <Arduino.h>

namespace Outlets {

constexpr uint8_t COUNT = 1;  // index 0 = the plug

// Restores the saved state (or `initialMask` if nothing is saved yet and it is >= 0, e.g. the
// state Tasmota left) and starts the button scan. Call first in setup().
void begin(int initialMask = -1);
void loop();   // saves state changes to flash

bool isOn(uint8_t index);
bool anyOn();
void set(uint8_t index, bool on);
void toggle(uint8_t index);
// Bumped on every outlet change, physical or remote; compare to detect changes.
uint32_t changeCount();

// "plug"
const char *key(uint8_t index);
// "", "1", "plug" or "all" (returns COUNT for all); -1 if not valid.
int parse(const String &value);

// ---- button (index as for outlets) ----
bool buttonPressed(uint8_t index);
uint32_t buttonPressCount(uint8_t index);
// Completed actions since boot (physical and simulated) and ms since the last one (-1 = none).
uint32_t buttonActionCount(uint8_t index);
int32_t buttonLastActionMsAgo(uint8_t index);
// Simulate a press exactly like the physical button: toggles the outlet.
void pressButton(uint8_t index);
// True once per action not yet taken; used to publish one MQTT event per press.
bool takeButtonEvent(uint8_t index);

// ---- LED ----
// The one LED shows the relay state while WiFi is up, and blinks while it isn't (fast while
// connecting, slow while the setup AP is up), like Tasmota without a separate link LED.
enum class Link : uint8_t { Connected, Connecting, SetupAp };
void setLink(Link link);

}  // namespace Outlets
