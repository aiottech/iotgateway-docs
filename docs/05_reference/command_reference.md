---
sidebar_position: 1
title: Command Reference Manual
description: Complete customer reference for all CLI commands and AST expressions supported by the MGW gateway.
---

# Command Reference Manual

The **Mini IoT Gateway (`MGW-IoT-002`)** supports two invocation formats across UART, SPI, BLE, WebSockets, and HTTP REST:

1. **CLI Command Strings:** Human-readable command-prompt syntax (e.g., `cmd set-wifi --ssid "MyRouter" --password "Pass123"`).
2. **AST Expressions:** C-style function calls (e.g., `set_wifi("MyRouter", "Pass123")`).

*(Note: All commands sent over physical UART/SPI must end with `\r\n`. Commands executed over wireless interfaces when an admin password is set must include `--token "<your_44_char_token>"` unless marked as Public).*

---

## 1. Universal Streaming Flags

Commands that stream data (such as `cmd http`, `cmd http-download`, `cmd scan-wifi`, and `cmd fs-read`) support these optional execution flags:

| Flag | Type | Default | Description |
| :--- | :---: | :---: | :--- |
| `--stream` | Boolean | Skill Default | Forces Stream Protocol V2 (`true`) or Normal Mode (`false`) |
| `--flow` | String | `"none"` | Flow control mode: `"none"`, `"ack"` (1-byte `0x06` ACK), or `"rts"` |
| `--ack-timeout` | Number | Global Setting | Custom ACK wait timeout in milliseconds for this command |

---

## 2. System, Diagnostics & Time

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `cmd version` | *None* | *(Public)* Queries the running gateway firmware version |
| `cmd get-mac` | *None* | *(Public)* Queries the 12-character hexadecimal base MAC address |
| `cmd echo` | `-c` / `--content` (String, Req) | *(Public)* Echoes back the input string for serial link verification |
| `cmd restart` | `-m` / `--mode` (String, Opt) | *(Public)* Reboots the gateway (`"configure"` or `"normal"`) |
| `cmd factory-reset` | *None* | Wipes all custom NVS settings and reboots into Configure Mode |
| `cmd set-startup-mode` | `-m` / `--mode` (String, Req) | Sets the persistent boot mode (`"configure"` or `"normal"`) |
| `cmd get-startup-mode` | *None* | Queries the currently saved boot mode |
| `cmd get-time` | *None* | Queries current date, time, and configured timezone |
| `cmd set-time` | `-t` / `--time` (String, Req)<br/>`-z` / `--zone` (String, Opt) | Sets system clock (`"dd/MM/yyyy HH:mm:ss"`) and timezone (e.g. `"UTC+7"`) |
| `cmd enable-log` | *None* | Enables diagnostic log output on the debug console |
| `cmd disable-log` | *None* | Silences diagnostic log output on the debug console |
| `cmd result-length` | *None* | *(Public)* Queries byte length of the latest cached transaction result |
| `cmd get-result` | `-f` / `--from` (Number, Req)<br/>`-t` / `--to` (Number, Req)<br/>`-i` / `--id` (Number, Opt) | *(Public)* Retrieves a byte slice from a completed transaction |

### Examples
```text
cmd echo -c "Hello Gateway"
cmd set-time --time "05/11/2026 20:15:30" --zone "UTC+7"
cmd restart --mode "normal"
```

---

## 3. Security & Authentication

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `cmd login` | `-u` / `--username` (String, Req)<br/>`-p` / `--password` (String, Req) | *(Public)* Unlocks UART/SPI session; returns a 44-char Access Token |
| `cmd verify-token` | `-t` / `--token` (String, Req) | *(Public)* Verifies whether a 44-char Access Token is valid |
| `cmd set-password` | `-p` / `--password` (String, Req)<br/>`-c` / `--confirm_password` (String, Req) | Sets a custom administrator password |
| `cmd get-password` | *None* | Queries the currently configured administrator password |
| `cmd delete-password` | *None* | Resets the administrator password back to default (`"admin"`) |
| `cmd set-credentials` | `--new_user` (String, Req)<br/>`--old_pass` (String, Req)<br/>`--new_pass` (String, Req) | Atomically updates both admin username and password |

### Examples
```text
cmd login -u "admin" -p "admin"
cmd set-credentials --new_user "operator" --old_pass "admin" --new_pass "SecurePass123!"
```

---

## 4. Hardware Interfaces & Peripherals

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `cmd setting-uart` | `-b` / `--baud_rate` (Number, Opt)<br/>`-d` / `--data_bits` (Number, Opt)<br/>`-p` / `--parity` (String, Opt)<br/>`-s` / `--stop_bits` (String, Opt) | Configures UART parameters and persists them (`9600`–`921600`, `7`/`8`, `"disable"`/`"even"`/`"odd"`, `"1"`/`"2"`) |
| `cmd get-setting-uart` | *None* | Queries the active physical UART parameters |
| `cmd uart-send` | `-c` / `--content` (String, Req) | Transmits raw string bytes directly over UART1 |
| `cmd disable-uart` | *None* | De-initializes UART peripheral drivers |
| `cmd set-fs-bridge-channel` | `-c` / `--channel` (String, Req) | Sets the bridge data channel (`"UART1"` or `"SPI1"`) |
| `cmd get-fs-bridge-channel` | *None* | Queries the active bridge data channel |
| `cmd set-global-ack-timeout` | `-t` / `--timeout` (Number, Req) | Sets global 1-byte ACK timeout in ms (100 to 60000) |
| `cmd get-global-ack-timeout` | *None* | Queries the active global ACK timeout in ms |
| `cmd gpio-config` | `-p` / `--pin` (Number, Req)<br/>`-t` / `--type` (String, Req) | Configures GPIO pin mode (`"input"` or `"output"`) |
| `cmd gpio-set` | `-p` / `--pin` (Number, Req)<br/>`-l` / `--level` (Number, Req) | Sets digital output level (`0` or `1`) on a GPIO pin |
| `cmd gpio-get` | `-p` / `--pin` (Number, Req) | Reads digital input level (`"0"` or `"1"`) from a GPIO pin |

### Examples
```text
cmd setting-uart --baud_rate 921600 --data_bits 8 --parity "disable" --stop_bits "1"
cmd set-fs-bridge-channel -c "SPI1"
cmd gpio-config --pin 8 --type "output"
cmd gpio-set --pin 8 --level 1
```

---

## 5. Wi-Fi & Access Point

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `cmd set-wifi` | `-s` / `--ssid` (String, Req)<br/>`-p` / `--password` (String, Req) | Connects gateway to a Wi-Fi router and saves credentials |
| `cmd restore-wifi` | *None* | Connects to Wi-Fi using previously saved credentials |
| `cmd wifi-status` | *None* | Queries Wi-Fi connection state (`"OK"` or `"NG"`) |
| `cmd wifi-info` | *None* | Queries saved Wi-Fi router credentials |
| `cmd get-ip` | *None* | Queries the active local IPv4 address assigned via DHCP |
| `cmd scan-wifi` | `-f` / `--force` (Boolean, Opt) | Scans surrounding Wi-Fi SSIDs (use `--force true` for a fresh scan) |
| `cmd disable-wifi` | *None* | Disconnects and powers down the Wi-Fi station driver |
| `cmd start-access-point` | *None* | Starts the local SoftAP hotspot |
| `cmd set-ap-password` | `-p` / `--password` (String, Req)<br/>`-c` / `--confirm_password` (String, Req) | Sets SoftAP WPA2 password (8 characters minimum) |
| `cmd get-ap-password` | *None* | Queries the saved SoftAP password |
| `cmd delete-ap-password` | *None* | Resets SoftAP password back to default device code |

### Examples
```text
cmd scan-wifi --force true
cmd set-wifi --ssid "Factory_Net" --password "RouterPassword"
cmd get-ip
```

---

## 6. HTTP, WebSockets & SignalR Cloud

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `cmd http` | `--url` (String, Req)<br/>`--method` (String, Opt)<br/>`--body` (String, Opt)<br/>`-h` / `--header` (String, Multi) | Sends an HTTP/HTTPS request (`GET`, `POST`, `HEAD`) and streams response |
| `cmd http-download` | `--url` (String, Req)<br/>`--filename` (String, Opt)<br/>`-h` / `--header` (String, Multi) | Downloads a remote file to storage |
| `cmd set-http-default-header` | `-h` / `--header` (String, Req) | Saves a persistent global HTTP header (e.g., Bearer Token) |
| `cmd delete-all-http-default-header` | *None* | Clears all globally saved HTTP headers |
| `cmd ws-start` | `-u` / `--url` (String, Req) | Connects background WebSocket client to a URI |
| `cmd ws-send` | `-c` / `--content` (String, Req) | Sends a text payload over the active WebSocket connection |
| `cmd ws-stop` | *None* | Terminates the active WebSocket client connection |
| `cmd signalr-start` | `-u` / `--url` (String, Req)<br/>`-t` / `--token` (String, Opt) | *(Public)* Connects and handshakes with an ASP.NET Core SignalR Hub |
| `cmd signalr-invoke` | `-m` / `--method` (String, Req)<br/>`-a` / `--args` (String, Opt) | *(Public)* Invokes a remote RPC method on the SignalR Hub |
| `cmd signalr-status` | *None* | *(Public)* Queries SignalR Hub connection status (`"OK"` or `"NG"`) |
| `cmd signalr-stop` | *None* | *(Public)* Disconnects from the SignalR Hub |
| `cmd start-http-server` | *None* | Starts the local HTTP REST server |
| `cmd start-web-server` | *None* | Starts the full Web Dashboard, mDNS, and WebSocket server |
| `cmd send-all-clients` | `-c` / `--content` (String, Req) | Broadcasts a raw text frame to all connected WebSocket browsers |

### Examples
```text
cmd http --method POST --url "https://api.example.com/data" --header "Content-Type: application/json" --body "{\"temp\":25.4}"
cmd signalr-start --url "wss://api.example.com/chathub" --token "eyJhbGciOi..."
cmd signalr-invoke --method "sendToAll" --args "[\"Node01\",\"Online\"]"
```

---

## 7. File Explorer & Native FTP Server

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `cmd fs-list` | `-p` / `--path` (String, Opt) | Lists files and folders on Host MCU storage (default `"/"`) |
| `cmd fs-read` | `-p` / `--path` (String, Req) | Streams a file from Host MCU storage |
| `cmd fs-delete` | `-p` / `--path` (String, Req) | Deletes a file or directory on Host MCU storage |
| `cmd fs-mkdir` | `-p` / `--path` (String, Req) | Creates a directory on Host MCU storage |
| `cmd ftp-start` | `-p` / `--port` (Number, Opt) | Starts the native RFC 959 FTP daemon (default port `21`) |
| `cmd ftp-stop` | *None* | Stops the active FTP daemon |
| `cmd ftp-status` | *None* | Queries FTP daemon status (`"RUNNING"` or `"STOPPED"`) |

### Examples
```text
cmd fs-list --path "/logs"
cmd ftp-start --port 21
```

---

## 8. Dynamic Web Plugins & Visibility Sync

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `cmd register-web-plugin` | `-n` / `--name` (String, Req)<br/>`-t` / `--title` (String, Req)<br/>`-i` / `--icon` (String, Opt)<br/>`-s` / `--size` (Number, Opt) | Registers a Host MCU web plugin into the Web Dashboard sidebar |
| `cmd unregister-web-plugin` | `-n` / `--name` (String, Req) | Removes a web plugin from the Web Dashboard sidebar |
| `cmd list-web-plugins` | *None* | Lists all currently registered web plugins |
| `cmd set-max-plugin-size` | `-s` / `--size` (Number, Req) | Sets the maximum allowed HTML bundle size in bytes |
| `cmd get-max-plugin-size` | *None* | Queries the active maximum plugin bundle size limit |
| `cmd is-plugin-visible` | `-n` / `--name` (String, Req) | *(Public)* Returns `"true"` if the user is currently viewing this plugin |
| `cmd get-active-view` | *None* | *(Public)* Returns active tab (`"plugin:<name>"`, `"settings"`, `"none"`) |
| `cmd get-dashboard-status` | *None* | *(Public)* Returns `"ONLINE"` if any browser is open, or `"OFFLINE"` |

### Examples
```text
cmd register-web-plugin -n "motor_ctrl" -t "Motor Controller" -i "motor" -s 32768
cmd is-plugin-visible -n "motor_ctrl"
```

---

## 9. Firmware Updates (OTA) & Cache Utilities

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `cmd get-firmware-list` | `-h` / `--header` (String, Multi) | Downloads available firmware release catalog |
| `cmd update-firmware` | `-u` / `--url` (String, Opt)<br/>`-h` / `--header` (String, Multi) | Triggers Over-the-Air firmware update and reboots |
| `cmd save-user-data` | `-k` / `--key` (String, Req)<br/>`-v` / `--value` (String, Req) | Saves a custom string key-value pair in temporary runtime memory |
| `cmd get-user-data` | `-k` / `--key` (String, Req) | Retrieves a custom string from temporary runtime memory |
| `cmd delete-user-data` | `-k` / `--key` (String, Req) | Deletes a custom string from temporary runtime memory |
| `cmd delete-all-user-data` | *None* | Clears all temporary runtime user data |

---

## 10. Bluetooth LE & Audio

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `cmd start-ble` | *None* | Starts Bluetooth LE advertising and GATT command server |
| `cmd disable-ble` | *None* | Stops and powers down the BLE controller |
| `cmd bt-connect` | `-n` / `--name` (String, Req) | Connects to a Bluetooth audio speaker by name or MAC address |
| `cmd bt-disconnect` | *None* | Disconnects from the active Bluetooth audio speaker |
| `cmd bt-speak` | `-t` / `--text` (String, Req)<br/>`-m` / `--mode` (String, Opt) | Synthesizes text to speech (`"cloud"` or `"offline"`) |
| `cmd bt-volume` | `-l` / `--level` (Number, Req) | Sets Bluetooth speaker volume (`0` to `100`) |
| `cmd bt-read-battery` | *None* | Queries battery percentage from the connected Bluetooth speaker |

---

## Next Steps

* Check the **Error Codes Reference** for troubleshooting failed responses.
