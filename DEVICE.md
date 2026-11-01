# Geekbes YM-WS-1 Smart Plug — Entry Table Lamp

Device survey taken **2026-11-01** from the running Tasmota firmware at `192.168.22.57`,
using the Tasmota HTTP command API (`/cm?cmnd=...`). Nothing on the device was changed.

## 1. Summary

| Item | Value |
|---|---|
| Product | **Geekbes YM-WS-1** plug-in smart plug, US type (FCC ID 2ALRJYM-WS-1; Tasmota template name `Geekbes`): 1 switched outlet, **no energy monitoring** |
| Location | Entry table, drives a lamp |
| MCU | **ESP8266EX** with external flash, running at 80 MHz |
| Flash | **1 MB** (chip ID `0x144068`), flash mode DOUT (3), 40 MHz |
| Energy meter IC | None (no HLW/BL/CSE pins in the template, no ENERGY section in `Status 10`) |
| Outlets / relays | 1 relay: Relay1 = GPIO15 |
| Buttons | 1 GPIO button: Button1 = GPIO13 |
| LEDs | 1 status LED, active-low (GPIO12). No separate link LED |
| Current firmware | Tasmota **12.0.2** (build 2022-06-20), core 2.7.4.9, SDK 2.2.2-dev |

This is a much simpler device than the Gosund WO1: one relay, one button, one LED, nothing
to measure.

## 2. Network identity

| Item | Value |
|---|---|
| IP address | 192.168.22.57 / 255.255.255.0, via DHCP (`IPAddress1 0.0.0.0`), `WifiConfig 4` = retry |
| Gateway / DNS | 192.168.22.1 / 192.168.22.1 (DNS2 0.0.0.0) |
| MAC | `68:C6:3A:97:C4:C9` |
| Hostname | `Entry-Table-Lamp-1225` (pattern `%s-%04d`, built from the topic) |
| Wi-Fi SSID | `ESP` (SSID2 empty), channel 1, 802.11n, BSSID `22:E8:29:17:11:E7` |
| Signal | −63 dBm (≈ 73%), Wi-Fi TX power 17.0 dBm |
| Web server | Enabled, **Admin mode**. The HTTP API answers with no credentials, so there is no web password in effect |
| HTTP API | Enabled |

## 3. GPIO map (from the active template)

Template: `{"NAME":"Geekbes","GPIO":[1,1,1,1,1,1,1,1,320,32,1,224,1,0],"FLAG":0,"BASE":18}`

The ESP8266 template array is in the order GPIO0, 1, 2, 3, 4, 5, 9, 10, 12, 13, 14, 15, 16, A0.
Code `1` means "User" (left free for the user to assign), i.e. not used by the hardware as far
as the template author knew.

| GPIO | Tasmota code | Function | Notes |
|---|---|---|---|
| GPIO0 | 1 | User (free) | Boot strap pin, must stay high at boot |
| GPIO1 | 1 | User (free) | UART TX |
| GPIO2 | 1 | User (free) | Boot strap pin, must stay high at boot |
| GPIO3 | 1 | User (free) | UART RX |
| GPIO4 | 1 | User (free) | — |
| GPIO5 | 1 | User (free) | — |
| GPIO9 | 1 | User | **Do not use:** wired to the external flash on an ESP8266EX |
| GPIO10 | 1 | User | **Do not use:** wired to the external flash on an ESP8266EX |
| GPIO12 | 320 | Led_i 1 | Status LED, active-low |
| GPIO13 | 32 | Button 1 | Push button, active-low (Tasmota default: input pull-up, pressed = low) |
| GPIO14 | 1 | User (free) | — |
| GPIO15 | 224 | Relay 1 | Outlet relay, active-high. Boot strap pin, must be low at boot (fine for a relay driver) |
| GPIO16 | 1 | User (free) | — |
| A0 | 0 | None | Not used |

Unlike the WO1, the button is a real GPIO, so no ADC resistor-ladder handling is needed.

The public pin maps for the YM-WS-1 agree: the [Blakadder template](https://templates.blakadder.com/geekbes_YM-WS-1.html)
and the [ESPHome config](https://devices.esphome.io/devices/geekbes-ym-ws-1/) both have GPIO12 LED (inverted),
GPIO13 button and GPIO15 relay. Blakadder also lists GPIO14 as `Switch4`. Nothing on this
plug's template uses it, and the ESPHome config doesn't either, so the firmware leaves GPIO14
an unused input.

## 4. Button and LED

| Setting | Value | Meaning |
|---|---|---|
| ButtonDebounce | 50 ms | — |
| ButtonTopic | 0 | Button acts on the local relay |
| SetOption1 | OFF | Multi-press and hold actions are allowed (Tasmota defaults) |
| SetOption11 | OFF | Single press = toggle, double press = no swap |
| SetOption13 | OFF | Multi-press detection is on, so single press is not "instant-only" |
| SetOption32 | 40 | Hold time 4 s (Tasmota: 10 × hold time = 40 s long press resets to defaults) |
| SetOption73 | OFF | Button is not decoupled from the relay |
| LedState | 1 | LED follows the relay power state |
| LedPower1 | OFF | — |
| LedMask | FFFF | — |
| BlinkCount / BlinkTime | 10 / 10 | — |

With no `LedLink` pin in the template, Tasmota also uses this one LED to blink the Wi-Fi /
MQTT link state while it is not connected.

**Confirmed on site (2026-11-01):** the GPIO13 button toggles the lamp, and the case LED is
the GPIO12 LED, lit while the relay is on (see §12).

## 5. Energy monitoring

None. The template has no energy-meter pins and `Status 10` returns only the time. The
replacement firmware needs no metering, calibration or energy-total code for this device.

## 6. Behaviour and configuration

| Setting | Value | Meaning |
|---|---|---|
| PowerOnState | 3 | Restore the last saved relay state after a power cycle |
| SaveState / SaveData | 1 / 1 | Relay state and settings are saved to flash |
| SetOption0 | ON | Save power state |
| Interlock | OFF | — (only one relay) |
| PulseTime | all 0 | No auto-off |
| SetOption65 | ON | Fast power-cycle factory reset **disabled** |
| SetOption26 | OFF | Plain `POWER` (not `POWER1`) in MQTT/JSON |
| StateText1 / 2 | OFF / ON | — |
| Sleep | 50 ms (dynamic) | — |
| TelePeriod | 300 s | — |
| Timers | All 16 disabled, master OFF | — |
| Rules 1/2/3 | Empty, all OFF | — |
| Friendly name | `Entry Table Lamp` | Device name `Entry Table Lamp` |
| Relay state at survey | OFF | — |
| Boot count / saves | 127 boots / 3874 flash saves | Since 2022-08-10 |
| Uptime at survey | 2 min 10 s | The device had just restarted (reason: Software/System restart) |
| Free memory | Heap ≈ 23–24 KB; program 619 KB, free sketch space 384 KB | — |
| Serial | 115200 8N1, SerialLog 2, WebLog 2 | — |

The stored `OtaUrl` is `http://ota.tasmota.com/tasmota/release-9.1.0/tasmota-lite.bin.gz`,
so this plug was most likely first flashed with Tasmota 9.1.0 lite and later upgraded to
12.0.2 by file upload.

## 7. MQTT

| Item | Value |
|---|---|
| Broker | `192.168.1.130:1883` (different subnet from the device) |
| User | `esp` (password not readable) |
| Client ID | `DVES_97C4C9` (mask `DVES_%06X`) |
| Topic | `Entry_Table_Lamp`, group topic `sonoffs` |
| FullTopic | `%prefix%/%topic%/` → e.g. `cmnd/Entry_Table_Lamp/POWER` |
| Prefixes | `cmnd` / `stat` / `tele` |
| Connection | Connected (MqttCount 1), SetOption3 ON |
| Home Assistant discovery | Off (SetOption19 OFF) |
| Keepalive | 30 s, socket timeout 4 s, max packet 1200 |
| Retain flags | All off (Power, Button, Switch, Sensor, Info, State) |

Note the group topic is `sonoffs` here, but `tasmotas` on the WO1.

## 8. Time and location

| Item | Value |
|---|---|
| NTP | `pool.ntp.org` |
| Timezone | `+01:00` with EU DST rules (last Sun Mar 02:00 → +2h, last Sun Oct 03:00 → +1h) |
| Lat / Long | 48.858360, 2.294442 (**Tasmota defaults: Eiffel Tower**) |

Same as the WO1: these are Tasmota's out-of-box defaults and almost certainly wrong for this
house.

## 9. Flash and OTA constraints for the replacement firmware

- **1 MB total flash**, same layout as the WO1. Tasmota reports 384 KB free for an upload,
  so the first install from Tasmota's web UI must be a **`.bin.gz`** under that size (the WO1
  firmware's `.gz` was about 310 KB; this device needs less code since there is no energy
  meter).
- ESP8266EX with external flash: GPIO9/10 belong to the flash and must stay untouched.
- **Moving off Tasmota:** Firmware Upgrade → Upgrade by file upload → `firmware.bin.gz`,
  exactly as for the WO1. A failed install means opening the plug for serial flashing
  (TX/RX/GND/3V3 + GPIO0 to GND), which is easier than for an in-wall receptacle but still
  means taking the case apart.
- Keep a working rollback path. Never ship an image that cannot accept the next OTA.

## 10. Tasmota settings sector (for the first-boot import)

- Tasmota **12.0.2**, `SaveAddress` = **`FA000`**, `CR 380/699` (text pool use), `CfgHolder 4617`.
- 12.0.2 rotates its settings through the eight sectors `0xF4000`–`0xFB000`; the current copy
  is the one with the highest `save_flag`. `0xFA000` lies inside the WO1 firmware's LittleFS
  range (`0xEB000`–`0xFB000`), so the same "scan all eight sectors before mounting LittleFS"
  import logic applies unchanged.
- The settings version is the same as the Family Room and Dinning Room WO1s (12.0.2), so the
  offsets in the WO1's DEVICE.md §11 apply. Fields of interest for this device:
  - SSID1 / password 1, MQTT host / user / password / port (text pool and `0xEFC`)
  - saved relay state at `0x2E8` (bit 0 only)
  - device name / friendly name and topic, if the new firmware wants to carry over
    `Entry Table Lamp`
  - no energy total or calibration to import
- The config dump `/dl` was decoded once (in a temporary folder, deleted afterwards, passwords
  never printed) to confirm the layout on this plug. Everything matched: `save_flag` 3874
  (= `SaveCount`), version `0x0C000200`, relay state 0 at `0x2E8`, MQTT port 1883 at `0xEFC`,
  SSID `ESP` at text 4, MQTT host/user at texts 12/14.
- **Friendly name 1 is text index 44** (`Entry Table Lamp`). Texts 16–27 are FullTopic, Topic
  (`Entry_Table_Lamp`), button/switch topics, group topic, the four state texts and the three
  NTP servers. 28–43 are the empty `Mem1`–`Mem16`. The firmware imports text 44 as its device
  name, unless it is Tasmota's default `Tasmota`.

## 11. Raw data

The full `Status 0` response from the survey is kept here for reference (trimmed of
`SetOption`/`Features` bitmaps):

```json
{"Status":{"Module":0,"DeviceName":"Entry Table Lamp","FriendlyName":["Entry Table Lamp"],"Topic":"Entry_Table_Lamp","ButtonTopic":"0","Power":0,"PowerOnState":3,"LedState":1,"LedMask":"FFFF","SaveData":1,"SaveState":1,"SwitchTopic":"0","SwitchMode":[0,0,0,0,0,0,0,0]},
 "StatusPRM":{"Baudrate":115200,"SerialConfig":"8N1","GroupTopic":"sonoffs","OtaUrl":"http://ota.tasmota.com/tasmota/release-9.1.0/tasmota-lite.bin.gz","RestartReason":"Software/System restart","BootCount":127,"BCResetTime":"2022-08-10T10:09:28","SaveCount":3874,"SaveAddress":"FA000"},
 "StatusFWR":{"Version":"12.0.2(tasmota)","BuildDateTime":"2022-06-20T12:41:03","Boot":4,"Core":"2_7_4_9","SDK":"2.2.2-dev(38a443e)","CpuFrequency":80,"Hardware":"ESP8266EX","CR":"380/699"},
 "StatusMEM":{"ProgramSize":619,"Free":384,"Heap":24,"ProgramFlashSize":1024,"FlashSize":1024,"FlashChipId":"144068","FlashFrequency":40,"FlashMode":3},
 "StatusNET":{"Hostname":"Entry-Table-Lamp-1225","IPAddress":"192.168.22.57","Gateway":"192.168.22.1","Subnetmask":"255.255.255.0","DNSServer1":"192.168.22.1","DNSServer2":"0.0.0.0","Mac":"68:C6:3A:97:C4:C9","Webserver":2,"HTTP_API":1,"WifiConfig":4,"WifiPower":17.0},
 "StatusMQT":{"MqttHost":"192.168.1.130","MqttPort":1883,"MqttClientMask":"DVES_%06X","MqttClient":"DVES_97C4C9","MqttUser":"esp","MqttCount":1,"MAX_PACKET_SIZE":1200,"KEEPALIVE":30,"SOCKET_TIMEOUT":4},
 "StatusSNS":{"Time":"2026-11-01T15:11:57"},
 "StatusSTS":{"POWER":"OFF","Wifi":{"AP":1,"SSId":"ESP","BSSId":"22:E8:29:17:11:E7","Channel":1,"Mode":"11n","RSSI":72,"Signal":-64}}}
```

## 12. Open items

None.

### Resolved

- **LED confirmed 2026-11-01:** the case LED is GPIO12 and is lit while the lamp is on.
- **Button confirmed 2026-11-01** under Logix Plug: 3 presses on GPIO13 gave 3 clean toggles
  (on, off, on), with `pressCount` 3 and no double counts. The raw input read pressed/released
  as expected.

- **Installed Logix Plug 1.0.0 on 2026-11-01** over Tasmota's upload form (`firmware.bin.gz`).
  The first boot imported the Tasmota 12.0.2 settings from `0xFA000`: WiFi `ESP`, MQTT
  `192.168.1.130:1883`, the name `Entry Table Lamp` and relay state off. It rejoined at
  192.168.22.57 as `entry-table-lamp` and connected to MQTT. Free space for the next
  update is 520 KB, so plain `firmware.bin` uploads work from now on.

- Model: Geekbes YM-WS-1, US plug (see §1 and §3).
- Unused pins: the public pin maps agree with the template. GPIO14 (`Switch4` in Blakadder)
  isn't used by anything, so it is left alone.
- Naming: one outlet, so no Top/Bottom suffix. The device name is the outlet name, and it
  is imported from Tasmota's friendly name (§10).
