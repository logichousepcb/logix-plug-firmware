#include "outlets.h"

#include <Ticker.h>

#include "config.h"
#include "log.h"
#include "storage.h"

namespace {

const char *const KEY = "plug";

const uint32_t STATE_MAGIC = 0x4C504C53;  // "LPLS"
const char *const STATE_PATH = "/state.bin";

const uint32_t BLINK_CONNECTING_MS = 200;
const uint32_t BLINK_AP_MS = 1000;

struct __attribute__((packed)) StoredState {
  uint32_t magic;
  uint8_t mask;  // bit 0 = relay
  uint8_t reserved[3];
  uint32_t checksum;
};

// Timer callbacks only run while loop() yields, so these never change mid-update; volatile
// because they are shared between the two contexts.
volatile uint8_t g_mask = 0;
volatile uint32_t g_changes = 0;
uint32_t g_handledChanges = 0;
uint8_t g_savedMask = 0;
volatile Outlets::Link g_link = Outlets::Link::Connecting;

struct Button {
  volatile bool pressed;
  volatile uint32_t pressCount;
  volatile uint32_t actionCount;
  volatile uint32_t lastActionAt;
  volatile uint32_t eventsTaken;
};
Button g_button = {};

Ticker g_scan;
bool g_candidate = false;  // pressed?
uint8_t g_stable = 0;

void apply(bool on) {
  digitalWrite(PIN_RELAY, on ? HIGH : LOW);
  uint8_t mask = on ? 1 : 0;
  if (mask != g_mask) {
    g_mask = mask;
    g_changes = g_changes + 1;
  }
}

void recordAction() {
  g_button.actionCount = g_button.actionCount + 1;
  g_button.lastActionAt = millis();
}

void updateLed() {
  bool lit;
  switch (g_link) {
    case Outlets::Link::Connected: lit = g_mask; break;
    case Outlets::Link::Connecting: lit = (millis() / BLINK_CONNECTING_MS) & 1; break;
    default: lit = (millis() / BLINK_AP_MS) & 1; break;
  }
  digitalWrite(PIN_LED, lit ? LOW : HIGH);
}

// Every BUTTON_SCAN_MS. The input must hold the same level for BUTTON_STABLE_SAMPLES scans
// before it counts; the relay toggles once, when a press first becomes stable.
void scanTick() {
  bool down = digitalRead(PIN_BUTTON) == LOW;
  if (down == g_candidate) {
    if (g_stable < 255) g_stable++;
  } else {
    g_candidate = down;
    g_stable = 1;
  }
  if (g_stable == BUTTON_STABLE_SAMPLES) {
    g_button.pressed = down;
    if (down) {
      g_button.pressCount = g_button.pressCount + 1;
      recordAction();
      apply(!g_mask);
    }
  }
  updateLed();
}

}  // namespace

namespace Outlets {

void begin(int initialMask) {
  StoredState s;
  bool stored = Storage::load(STATE_PATH, s, STATE_MAGIC);
  uint8_t mask = (stored ? s.mask : initialMask > 0 ? initialMask : 0) & 1;
  // Level first, then output, so the relay never glitches on.
  digitalWrite(PIN_RELAY, mask ? HIGH : LOW);
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_LED, mask ? LOW : HIGH);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  g_mask = mask;
  g_savedMask = stored ? mask : 0;
  g_handledChanges = g_changes;
  if (!stored && mask) g_changes = g_changes + 1;  // loop() saves the inherited state
  logf("[plug] restored %s", mask ? "on" : "off");
  g_scan.attach_ms(BUTTON_SCAN_MS, scanTick);
}

void loop() {
  uint32_t changes = g_changes;
  if (changes == g_handledChanges) return;
  g_handledChanges = changes;
  uint8_t mask = g_mask;
  logf("[plug] %s", mask ? "on" : "off");
  if (mask == g_savedMask) return;
  StoredState s = {};
  s.mask = mask;
  if (Storage::save(STATE_PATH, s, STATE_MAGIC)) {
    g_savedMask = mask;
  } else {
    logf("[plug] failed to save state");
  }
}

bool isOn(uint8_t index) { return index < COUNT && g_mask; }
bool anyOn() { return g_mask != 0; }

void set(uint8_t index, bool on) {
  if (index < COUNT) apply(on);
}

void toggle(uint8_t index) {
  if (index < COUNT) apply(!isOn(index));
}

uint32_t changeCount() { return g_changes; }

const char *key(uint8_t index) { return index < COUNT ? KEY : "all"; }

int parse(const String &value) {
  String v = value;
  v.trim();
  v.toLowerCase();
  if (v.isEmpty() || v == "1" || v == KEY) return 0;  // the only outlet is also the default
  if (v == "all" || v == "0") return COUNT;
  return -1;
}

bool buttonPressed(uint8_t index) { return index < COUNT && g_button.pressed; }
uint32_t buttonPressCount(uint8_t index) { return index < COUNT ? g_button.pressCount : 0; }
uint32_t buttonActionCount(uint8_t index) { return index < COUNT ? g_button.actionCount : 0; }

int32_t buttonLastActionMsAgo(uint8_t index) {
  if (index >= COUNT || g_button.actionCount == 0) return -1;
  return (int32_t)(millis() - g_button.lastActionAt);
}

void pressButton(uint8_t index) {
  if (index >= COUNT) return;
  recordAction();
  toggle(index);
}

bool takeButtonEvent(uint8_t index) {
  if (index >= COUNT) return false;
  if (g_button.eventsTaken == g_button.actionCount) return false;
  g_button.eventsTaken = g_button.eventsTaken + 1;
  return true;
}

void setLink(Link link) { g_link = link; }

}  // namespace Outlets
