<h1 align="center">Logix Plug Firmware</h1>

<p align="center">
  Replacement firmware for the Geekbes YM-WS-1 smart plug (ESP8266EX, 1 MB).<br>
  One switched outlet, controlled from its own web page, an HTTP API or MQTT.<br>
  Installs over the Tasmota web OTA form.
</p>

## Features

- **Works on its own:** the button toggles the plug at all times. That includes with no WiFi, no MQTT, and while the network code is busy, because the button is scanned from a timer and not from the main loop.
- **Survives power cuts:** the plug's state is saved to flash the moment it changes and restored first thing at power-up, before WiFi starts.
- **One LED, like Tasmota:** it is lit while the plug is on. While WiFi is down it blinks fast (connecting) or slowly (setup AP up).
- **Name:** one device name (default `Logix Plug XXXX`), which is also the outlet's name. The hostname follows the name (e.g. `entry-table-lamp.local`).
- **Settings page:** device name and admin password, WiFi (with scan), MQTT broker, firmware update.
- **Setup access point built in:** when there is no saved network, or the network is gone for a minute, an open AP `Logix-Plug-XXXX` comes up at `192.168.4.1` with a captive portal. The saved network keeps being retried, and the AP goes away once it is back.
- **Comes back from Tasmota with its settings:** on the first boot it imports Tasmota's WiFi network, MQTT broker, friendly name and relay state (Tasmota 12.0.2 layout), then erases Tasmota's settings.
- **LogixDIY ready:** mDNS `_logix._tcp`, a public `GET /api/info` and retained MQTT discovery on `logix/discovery/<id>`, the same contract as the Logix Outlet. The API has the Logix Outlet's shape, with one outlet and one button.

See [API.md](API.md) for the HTTP and MQTT API and [DEVICE.md](DEVICE.md) for the hardware survey.

## Hardware

| GPIO | Function |
|---|---|
| 15 | Relay (active high) |
| 13 | Button (active low, internal pull-up) |
| 12 | LED (active low) |

There is no energy meter. The pin map is hard-coded in [src/config.h](src/config.h).

## Building

[PlatformIO](https://platformio.org/):

```sh
pio run
```

This produces `.pio/build/logixplug/firmware.bin` and `firmware.bin.gz`.

## Installing over Tasmota

Tasmota only has about 384 KB free for an upload, so **use `firmware.bin.gz`** (about 300 KB). The full `.bin` (about 420 KB) is too big for this step.

1. Open the Tasmota web page → **Firmware Upgrade** → **Upgrade by file upload**.
2. Choose `firmware.bin.gz` and click **Start upgrade**.
3. The device restarts into Logix Plug. On this first boot it imports the WiFi network, MQTT broker and name from Tasmota and rejoins the network. Look for it at the same IP address (if DHCP hands it out again) or at `<name>.local`, e.g. `entry-table-lamp.local`.
4. If it can't join the network, connect to the WiFi network `Logix-Plug-XXXX` and set it up at `http://192.168.4.1`.

From the command line:

```sh
curl -s http://<tasmota-ip>/up > /dev/null
curl -F "u2=@.pio/build/logixplug/firmware.bin.gz" http://<tasmota-ip>/u2
```

The plug keeps the on/off state Tasmota last saved. From then on it keeps its state through power cuts and updates.

## Updating

Settings → **Firmware** accepts `firmware.bin` or `firmware.bin.gz`. Or:

```sh
curl -F "firmware=@.pio/build/logixplug/firmware.bin.gz" http://<device>/update
```

### Going back to Tasmota

The full Tasmota image is larger than the free update space. Upload `tasmota-minimal.bin.gz` first, then the full Tasmota from its own upgrade page.

## License

[MIT](LICENSE)
