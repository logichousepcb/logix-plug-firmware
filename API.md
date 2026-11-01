# Logix Plug HTTP and MQTT API

All endpoints are on port 80 of the device (e.g. `http://192.168.22.57` or `http://<hostname>.local`).

- **Requests:** `POST` parameters are sent form-encoded (`application/x-www-form-urlencoded`) or as a query string.
- **Responses:** JSON.
- **Authentication:** none by default. Once an admin password is set, every endpoint except `GET /api/info` requires HTTP Basic auth with user `admin` (for curl: `-u admin:<password>`).

Actions return `{"ok":true,"message":"..."}` with HTTP 200. Errors return `{"ok":false,"message":"..."}` with HTTP 400. Actions that restart the device also return `"restart":true`.

**The plug** has one outlet, number 1 with key `plug`. Anywhere an outlet is given, `1`, `plug` or `all` is accepted. The outlet carries the device name (e.g. `Entry Table Lamp`). The API has the same shape as the Logix Outlet's (arrays of outlets and buttons), with one entry each.

# Identifying the device

The device follows the same Logix contract as the Logix Outlet, Logix 3 and Logix Vent, so the LogixDIY integration can discover it.

## mDNS / zeroconf

The device advertises `_logix._tcp.local.` (and `_http._tcp`) on port 80, with these TXT records:

| Key | Example | Meaning |
|---|---|---|
| `model` | `logixplug` | machine-readable model; stays the same across firmware versions |
| `id` | `68c63a97c4c9` | lowercase MAC without separators; stable unique ID |
| `fw` | `1.0.0` | firmware version |
| `api` | `1` | API contract version; increases only on breaking changes |
| `name` | `Entry Table Lamp` | device name |
| `manufacturer` | `Logix` | |

## GET /api/info

Never requires a password.

```json
{
  "manufacturer": "Logix", "model": "logixplug", "name": "Entry Table Lamp",
  "id": "68c63a97c4c9", "mac": "68:C6:3A:97:C4:C9", "hostname": "entry-table-lamp", "ip": "192.168.22.57",
  "fw": "1.0.0", "api": 1, "authRequired": false,
  "capabilities": {
    "outlets": 1, "buttons": 1,
    "outletKeys": ["plug"],
    "outletNames": ["Entry Table Lamp"],
    "actions": ["single"],
    "energy": false, "energyMeters": 0
  },
  "mqtt": {"enabled": true, "connected": true, "topic": "logixplug/68c63a97c4c9"}
}
```

## MQTT discovery

When MQTT is enabled, the same JSON is published, retained, to `logix/discovery/<id>`.

# The plug

## Read everything

`GET /api/controller`

```json
{
  "uptimeMs": 81234,
  "name": "Entry Table Lamp",
  "outlets": [
    {"outlet": 1, "key": "plug", "name": "Entry Table Lamp", "state": "on"}
  ],
  "buttons": [
    {"button": 1, "key": "plug", "pressed": false, "pressCount": 3, "lastAction": "single", "actionCount": 4, "lastActionMsAgo": 5120}
  ]
}
```

- `buttons[0].pressCount`: physical presses since boot.
- `buttons[0].actionCount`: physical and simulated presses since boot. When polling, a change means a new press.

## Switch the plug

`POST /api/outlet`

| Parameter | Value |
|---|---|
| `outlet` | `1`, `plug` or `all` (default: the plug) |
| `state` | `on`, `off` or `toggle` (default `toggle`) |

```sh
curl -X POST -d "outlet=plug&state=on" http://192.168.22.57/api/outlet
# {"ok":true,"message":"Plug on."}
```

The new state is saved to flash immediately and restored after a power cut.

## Simulate a button press

`POST /api/controller/press` with `button=1|plug`. This behaves exactly like the physical button: it toggles the plug and counts as an action.

# Device

| Endpoint | Purpose |
|---|---|
| `GET /api/info` | identity and capabilities (no password required) |
| `GET /api/status` | firmware, network, uptime, heap, raw button state |
| `POST /api/device` | `name` (1-32 chars; restarts if the hostname changes), `admin` (4-64 chars), `adminClear=1` |
| `GET /api/scan` | WiFi scan; returns `{"scanning":true}` until the results are ready; `?fresh=1` starts a new scan |
| `POST /api/wifi` | `ssid`, `pass` (blank for an open network); saves and restarts |
| `POST /api/forget` | erase the saved network and restart into the setup AP |
| `GET /api/mqtt` | MQTT settings and connection status (the password is never returned) |
| `POST /api/mqtt` | `host` (blank turns MQTT off), `port`, `user`, `pass` (blank keeps the saved one), `passClear=1`, `topic` (blank = default) |
| `GET /api/log` | recent log lines (plain text) |
| `POST /api/reboot` | restart |
| `POST /update` | firmware upload, multipart field `firmware`; `.bin` or `.bin.gz` |

# MQTT

Set the broker on the Settings page. The device connects with its hostname as the client ID and retries every 10 s. Every topic sits under the base topic `T`. The default is `logixplug/<id>`. All messages are QoS 0.

## Published by the device

| Topic | Retained | Payload |
|---|---|---|
| `T/status` | yes | `online`, or `offline` (last will) |
| `T/state` | yes | same JSON as `GET /api/controller`, without `uptimeMs` and `lastActionMsAgo`. Sent on every change and at least every 60 s |
| `T/button/1/action` | no | `single`: one message per button press, physical or simulated |
| `T/result` | no | reply to each command: `{"command":"outlet","ok":true,"message":"Plug on."}` |

## Commands

| Topic | Payload | HTTP equivalent |
|---|---|---|
| `T/outlet/N/set` | `ON`, `OFF` or `TOGGLE` (N = `1`, `plug`, `all`) | `POST /api/outlet` |
| `T/set` | JSON: `{"plug":"on"}`, `{"outlet1":false}` or `{"all":"toggle"}` | `POST /api/outlet` |
| `T/press` | `1`, or `{"button":1}` | `POST /api/controller/press` |
| `T/get` | anything | republishes `T/state` |

```sh
mosquitto_pub -h <broker> -t logixplug/68c63a97c4c9/outlet/plug/set -m ON
mosquitto_sub -h <broker> -t 'logixplug/68c63a97c4c9/#' -v
```
