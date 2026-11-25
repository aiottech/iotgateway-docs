---
sidebar_position: 1
title: Command Reference Manual
description: Complete customer reference for all CLI commands supported by the MGW gateway.
---

# Command Reference Manual

The **Mini IoT Gateway (`MGW-IoT-002`)** provides a standardized, human-readable Command Line Interface (CLI) accessible across all physical and wireless interfaces: **UART1**, **SPI1**, **Bluetooth LE**, **WebSockets**, and **HTTP REST**.

### General Rules:
1. **Line Termination:** All commands transmitted over physical serial lines (**UART1** or **SPI1**) must end with standard Carriage Return and Line Feed: `\r\n`.
2. **Command Prefix:** All command strings must start with the `cmd ` prefix (e.g., `cmd get-time`).
3. **Authentication Token:** When an administrator password is set on the gateway, commands executed over wireless/network interfaces (**WebSockets**, **HTTP REST**, **BLE**) must include the `--token "<token>"` parameter unless the command is classified as Public. Commands sent over physical **UART1** and **SPI1** do not require tokens after performing a one-time `cmd login`.

---

## 1. Universal Streaming Flags

Commands capable of streaming high-volume or chunked data (such as `cmd http`, `cmd http-download`, `cmd scan-wifi`, and `cmd fs-read`) support these optional execution flags:

| Parameter Flag | Type | Default | Description |
| :--- | :---: | :---: | :--- |
| **`--stream <bool>`** | Boolean | Skill Default | Forces Stream Protocol V2 (`true`) or atomic Normal Mode (`false`). |
| **`--flow <mode>`** | String | `"none"` | Flow control mode: `"none"` (continuous), `"ack"` (1-byte `0x06` software ACK handshake), or `"rts"` (hardware RTS/CTS). |
| **`--ack-timeout <ms>`** | Integer | Global NVS Setting | Overrides the flow-control ACK wait timeout limit in milliseconds for this specific command (e.g., `--ack-timeout 10000`). |

---

## 2. Platform Diagnostics & Utility Commands

Basic system utilities registered on boot. These commands bypass standard license and password checks, making them accessible on raw, unactivated gateways.

### 2.1 `version`
* **Purpose:** Queries the compiled gateway application firmware version.
* **Parameters:** None.
* **Return Value:** Firmware version string (e.g., `"1.6.0"`).
* **CLI Syntax:** `cmd version`

---

### 2.2 `get-mac`
* **Purpose:** Queries the physical base MAC address of the gateway's network adapter.
* **Parameters:** None.
* **Return Value:** 12-character hexadecimal MAC string (e.g., `"d0cf13ef641e"`).
* **CLI Syntax:** `cmd get-mac`

---

### 2.3 `result-length`
* **Purpose:** Queries the total byte length of the latest transaction payload stored in the ring buffer.
* **Parameters:** None.
* **Return Value:** Numeric string indicating byte count (e.g., `"1562"`).
* **CLI Syntax:** `cmd result-length`

---

### 2.4 `get-result`
* **Purpose:** Retrieves a sliced byte segment of a completed transaction from the historical ring catalog.
* **Parameters:**
  * `--from` / `-f` (Required, Integer): Starting byte offset.
  * `--to` / `-t` (Required, Integer): Ending byte offset.
  * `--id` / `-i` (Optional, Integer): Target Transaction ID (defaults to latest if omitted).
* **Return Value:** Raw byte stream of the requested segment.
* **CLI Syntax:** `cmd get-result --from 0 --to 500 --id 14`

---

### 2.5 `get-time`
* **Purpose:** Queries the currently active system date, time, and configured timezone.
* **Parameters:** None.
* **Return Value:** Formatted DateTime string (e.g., `"01/01/1970 00:45:15 (UTC+0)"`).
* **CLI Syntax:** `cmd get-time`

---

### 2.6 `set-time`
* **Purpose:** Modifies the gateway's system clock and saves the timezone setting to persistent storage.
* **Parameters:**
  * `--time` / `-t` (Required, String): New DateTime string in `"dd/MM/yyyy HH:mm:ss"` format.
  * `--zone` / `-z` (Optional, String): POSIX timezone identifier (e.g., `"UTC+7"`).
* **Return Value:** `"Time set successfully."` on success.
* **CLI Syntax:** `cmd set-time --time "05/11/2026 20:15:30" --zone "UTC+7"`

---

### 2.7 `enable-log`
* **Purpose:** Re-enables on-board diagnostic UART0 logging prints.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd enable-log`

---

### 2.8 `disable-log`
* **Purpose:** Silences all target diagnostic UART0 logging output (Production Mode optimization).
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd disable-log`

---

### 2.9 `echo`
* **Purpose:** Repeats back input text payloads for diagnostic latency and echo verification tests.
* **Parameters:**
  * `--content` / `-c` (Required, String): Text payload to repeat back.
* **Return Value:** The exact input string.
* **CLI Syntax:** `cmd echo -c "Hello Gateway"`

---

## 3. Storage & Configuration Commands

### 3.1 `save-user-data`
* **Purpose:** Writes a custom key-value record to the in-memory contiguous cache.
* **Parameters:**
  * `--key` / `-k` (Required, String): Unique key identifier.
  * `--value` / `-v` (Required, String): String value to associate with the key.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd save-user-data --key "http_header0" --value "Authorization: Bearer Token123"`

---

### 3.2 `get-user-data`
* **Purpose:** Retrieves a custom data record from the in-memory contiguous cache.
* **Parameters:**
  * `--key` / `-k` (Required, String): Target key identifier.
* **Return Value:** The saved value string.
* **CLI Syntax:** `cmd get-user-data --key "http_header0"`

---

### 3.3 `delete-user-data`
* **Purpose:** Erases a specific custom record from the in-memory cache and compacts storage.
* **Parameters:**
  * `--key` / `-k` (Required, String): Target key to delete.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd delete-user-data --key "http_header0"`

---

### 3.4 `delete-all-user-data`
* **Purpose:** Completely clears all custom key-value records from the in-memory cache.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd delete-all-user-data`

---

### 3.5 `set-startup-mode`
* **Purpose:** Configures the default operating startup mode in persistent NVS flash.
* **Parameters:**
  * `--mode` / `-m` (Required, String): Boot mode (`"configure"`, `"normal"`, or `"ota"`).
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd set-startup-mode --mode "configure"`

---

### 3.6 `get-startup-mode`
* **Purpose:** Queries the saved boot mode from persistent NVS flash.
* **Parameters:** None.
* **Return Value:** `"configure"`, `"normal"`, or `"ota"`.
* **CLI Syntax:** `cmd get-startup-mode`

---

### 3.7 `set-nvs`
* **Purpose:** Writes a raw configuration string directly into persistent NVS storage.
* **Parameters:**
  * `--key` / `-k` (Required, String): Storage key (maximum 15 characters).
  * `--value` / `-v` (Required, String): Value string.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd set-nvs --key "custom_param" --value "12345"`

---

### 3.8 `get-nvs`
* **Purpose:** Reads a raw configuration string directly from persistent NVS storage.
* **Parameters:**
  * `--key` / `-k` (Required, String): Storage key.
* **Return Value:** The stored string value.
* **CLI Syntax:** `cmd get-nvs --key "custom_param"`

---

### 3.9 `delete-nvs`
* **Purpose:** Erases a key-value record from persistent NVS storage.
* **Parameters:**
  * `--key` / `-k` (Required, String): Target key to erase.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd delete-nvs --key "custom_param"`

---

## 4. Peripherals & System Control Commands

### 4.1 `gpio-config`
* **Purpose:** Configures the direction of a physical GPIO pin.
* **Parameters:**
  * `--pin` / `-p` (Required, Integer): Target GPIO pin number.
  * `--type` / `-t` (Required, String): Mode (`"input"` or `"output"`).
* **Return Value:** `"gpio config done successfully"`.
* **CLI Syntax:** `cmd gpio-config --pin 8 --type "output"`

---

### 4.2 `gpio-set`
* **Purpose:** Writes a digital high or low level to an output GPIO pin.
* **Parameters:**
  * `--pin` / `-p` (Required, Integer): Target GPIO pin number.
  * `--level` / `-l` (Required, Integer): Output level (`0` for Low, `1` for High).
* **Return Value:** `"gpio set level done successfully"`.
* **CLI Syntax:** `cmd gpio-set --pin 8 --level 1`

---

### 4.3 `gpio-get`
* **Purpose:** Reads the digital input level of a target GPIO pin.
* **Parameters:**
  * `--pin` / `-p` (Required, Integer): Target GPIO pin number.
* **Return Value:** `"0"` (Low) or `"1"` (High).
* **CLI Syntax:** `cmd gpio-get --pin 8`

---

### 4.4 `restart`
* **Purpose:** Triggers a software-controlled hardware reboot of the gateway.
* **Parameters:**
  * `--mode` / `-m` (Optional, String): Boot mode override before rebooting (`"configure"`, `"normal"`, or `"ota"`).
* **Return Value:** `"System will restart soon... Please wait!"`.
* **CLI Syntax:** `cmd restart --mode "configure"`

---

### 4.5 `factory-reset`
* **Purpose:** Performs a complete factory wipe of all user configurations (passwords, Wi-Fi credentials, AP keys, UART baud rate, channel routes), restores out-of-box Configure Mode, and reboots.
* **Parameters:** None.
* **Return Value:** `"Factory reset initiated. Rebooting..."`.
* **CLI Syntax:** `cmd factory-reset`

---

### 4.6 `confirm`
* **Purpose:** Signals user confirmation to unblock a waiting background task (such as legacy BLE pairing).
* **Parameters:**
  * `--option` / `-o` (Required, String): Decision (`"OK"` or `"Cancel"`).
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd confirm --option "OK"`

---

### 4.7 `setting-uart`
* **Purpose:** Applies custom serial configurations dynamically to UART1 and persists them to NVS.
* **Parameters:**
  * `--baud_rate` / `-b` (Optional, Integer): Baud rate (`9600`, `19200`, `38400`, `57600`, `115200`, `230400`, `921600`).
  * `--data_bits` / `-d` (Optional, Integer): Data bits (`5`, `6`, `7`, `8`, `9`).
  * `--parity` / `-p` (Optional, String): Parity mode (`"disable"`, `"even"`, `"odd"`).
  * `--stop_bits` / `-s` (Optional, String): Stop bits (`"1"`, `"1.5"`, `"2"`).
* **Return Value:** `"UART setting done successfully!"`.
* **CLI Syntax:** `cmd setting-uart --baud_rate 115200 --data_bits 8 --parity "disable" --stop_bits "1"`

---

### 4.8 `get-setting-uart`
* **Purpose:** Queries the active physical UART1 settings from registers (snapped to standard nominal rates).
* **Parameters:** None.
* **Return Value:** Formatted string (e.g., `"baudrate: 115200, databits: 8, parity: disable, stopbits: 1"`).
* **CLI Syntax:** `cmd get-setting-uart`

---

### 4.9 `uart-water-mark`
* **Purpose:** Queries the stack high watermark remaining bytes for the background UART listener thread.
* **Parameters:** None.
* **Return Value:** High watermark value in bytes (e.g., `"3500"`).
* **CLI Syntax:** `cmd uart-water-mark`

---

### 4.10 `uart-send`
* **Purpose:** Transmits a raw byte array directly across physical UART1 lines.
* **Parameters:**
  * `--content` / `-c` (Required, String): Text string or payload to transmit.
* **Return Value:** The transmitted string.
* **CLI Syntax:** `cmd uart-send --content "Hello Host MCU"`

---

### 4.11 `disable-uart`
* **Purpose:** Uninstalls and de-initializes the physical UART1 peripheral drivers.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd disable-uart`

---

### 4.12 `debug-mode`
* **Purpose:** Switches the gateway into Debug Mode, activating diagnostic logging and memory auditing.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd debug-mode`

---

### 4.13 `production-mode`
* **Purpose:** Switches the gateway into Production Mode, silencing diagnostic logs to maximize processing speed.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd production-mode`

---

### 4.14 `set-global-ack-timeout`
* **Purpose:** Configures the global default flow-control ACK wait timeout limit across all streaming interfaces.
* **Parameters:**
  * `--timeout` / `-t` (Required, Integer): Timeout limit in milliseconds ($100 \le \text{timeout} \le 60000$).
* **Return Value:** `"Global ACK timeout set to <ms> ms"`.
* **CLI Syntax:** `cmd set-global-ack-timeout --timeout 10000`

---

### 4.15 `get-global-ack-timeout`
* **Purpose:** Queries the currently active global flow-control ACK wait timeout from persistent storage.
* **Parameters:** None.
* **Return Value:** Formatted string (e.g., `"5000 ms"`).
* **CLI Syntax:** `cmd get-global-ack-timeout`

---

### 4.16 `set-fs-bridge-channel`
* **Purpose:** Selects the physical hardware transport channel used by the Web Dashboard to bridge file operations and web plugins.
* **Parameters:**
  * `--channel` / `-c` (Required, String): Target channel (`"UART1"` or `"SPI1"`).
* **Return Value:** `"File Explorer channel set to '<channel>'"`.
* **CLI Syntax:** `cmd set-fs-bridge-channel --channel "SPI1"`

---

### 4.17 `get-fs-bridge-channel`
* **Purpose:** Queries the currently configured hardware channel used for Host MCU bridging.
* **Parameters:** None.
* **Return Value:** Channel identifier string (`"UART1"` or `"SPI1"`).
* **CLI Syntax:** `cmd get-fs-bridge-channel`

---

### 4.18 `start-spi`
* **Purpose:** Initializes and starts the high-speed SPI1 Slave listener thread on demand.
* **Parameters:** None.
* **Return Value:** `"SPI1 listener started."`.
* **CLI Syntax:** `cmd start-spi`

---

### 4.19 `disable-spi`
* **Purpose:** Stops the high-speed SPI1 Slave listener thread and frees DMA buffers back to internal SRAM.
* **Parameters:** None.
* **Return Value:** `"SPI1 listener stopped."`.
* **CLI Syntax:** `cmd disable-spi`

---

## 5. Security, Authentication & Licensing Commands

### 5.1 `login`
* **Purpose:** Authenticates administrator credentials. When executed over physical UART1 or SPI1, permanently unlocks the stateful hardware bus session. When executed over wireless/network interfaces, returns a 44-character Base64 Access Token.
* **Parameters:**
  * `--username` / `-u` (Required, String): Administrator username (default: `"admin"`).
  * `--password` / `-p` (Required, String): Administrator password (default: `"admin"`).
* **Return Value:** 44-character Base64 Access Token string.
* **CLI Syntax:** `cmd login --username "admin" --password "admin"`

---

### 5.2 `verify-token`
* **Purpose:** Verifies whether a 44-character Access Token is currently valid in gateway memory.
* **Parameters:**
  * `--token` / `-t` (Required, String): Access token string to verify.
* **Return Value:** `"Valid token."` on success.
* **CLI Syntax:** `cmd verify-token --token "p7mQC4o6daZhRPTKvlkcbgZQUhJ5lSPj8Leq1ByzBM8="`

---

### 5.3 `set-password`
* **Purpose:** Configures a custom administrator password in persistent NVS storage.
* **Parameters:**
  * `--password` / `-p` (Required, String): New administrator password.
  * `--confirm_password` / `-c` (Required, String): Confirmation matching password.
* **Return Value:** `"password set successfully!"`.
* **CLI Syntax:** `cmd set-password -p "Secret123" -c "Secret123"`

---

### 5.4 `get-password`
* **Purpose:** Queries the currently active administrator password from persistent NVS storage.
* **Parameters:** None.
* **Return Value:** Password string.
* **CLI Syntax:** `cmd get-password`

---

### 5.5 `delete-password`
* **Purpose:** Erases the custom administrator password, resetting it back to factory default `"admin"`.
* **Parameters:** None.
* **Return Value:** `"Delete password successfully!"`.
* **CLI Syntax:** `cmd delete-password`

---

### 5.6 `set-credentials`
* **Purpose:** Atomically updates both the administrator username and password in persistent storage.
* **Parameters:**
  * `--new_user` (Required, String): New administrator username.
  * `--old_pass` (Required, String): Current password for authorization.
  * `--new_pass` (Required, String): New administrator password.
* **Return Value:** `"Credentials updated successfully! Please re-login."`.
* **CLI Syntax:** `cmd set-credentials --new_user "operator" --old_pass "admin" --new_pass "Pass1234!"`

---

### 5.7 `activate-license`
* **Purpose:** Connects to the official GitHub raw repository, streams the device code catalogs, downloads the ECDSA P-256 signature, validates authenticity, and registers the license key permanently.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd activate-license`

---

### 5.8 `check-license`
* **Purpose:** Queries persistent storage to verify if the gateway holds a valid cryptographic activation key.
* **Parameters:** None.
* **Return Value:** `"OK"` if activated, error frame otherwise.
* **CLI Syntax:** `cmd check-license`

---

## 6. Wireless & Audio Communication Commands

### 6.1 `set-ap-password`
* **Purpose:** Sets the WPA2 passphrase for the gateway's SoftAP configuration hotspot.
* **Parameters:**
  * `--password` / `-p` (Required, String): Hotspot password ($\ge 8$ characters).
  * `--confirm_password` / `-c` (Required, String): Confirmation password.
* **Return Value:** `"Access point password set successfully!"`.
* **CLI Syntax:** `cmd set-ap-password -p "12345678" -c "12345678"`

---

### 6.2 `get-ap-password`
* **Purpose:** Queries the active SoftAP WPA2 password from persistent storage.
* **Parameters:** None.
* **Return Value:** Passphrase string.
* **CLI Syntax:** `cmd get-ap-password`

---

### 6.3 `delete-ap-password`
* **Purpose:** Erases the custom SoftAP password, resetting it back to the unique hardware device code.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd delete-ap-password`

---

### 6.4 `start-access-point`
* **Purpose:** Initializes and launches the local SoftAP configuration hotspot on IP `192.168.4.1`.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd start-access-point`

---

### 6.5 `start-ble`
* **Purpose:** Powers on the NimBLE controller and begins Bluetooth LE advertising.
* **Parameters:** None.
* **Return Value:** `"Start BLE done"`.
* **CLI Syntax:** `cmd start-ble`

---

### 6.6 `disable-ble`
* **Purpose:** Shuts down the NimBLE stack and returns Bluetooth controller DRAM back to internal SRAM.
* **Parameters:** None.
* **Return Value:** `"Disable BLE done"`.
* **CLI Syntax:** `cmd disable-ble`

---

### 6.7 `set-wifi`
* **Purpose:** Connects the gateway Station interface to an external Wi-Fi router, saves credentials to NVS, and optionally triggers online cryptographic activation upon connection.
* **Parameters:**
  * `--ssid` / `-s` (Required, String): Router network SSID.
  * `--password` / `-p` (Required, String): Router WPA2/WPA3 password.
  * `--activate` / `-a` (Optional, Boolean): If `true`, automatically executes online activation immediately upon connection.
* **Return Value:** `"Connect wifi successfully."` or `"Wi-Fi connected and license activated successfully!"`.
* **CLI Syntax:** `cmd set-wifi --ssid "MyRouter" --password "MyPassword" --activate true`

---

### 6.8 `restore-wifi`
* **Purpose:** Re-establishes a Wi-Fi connection using previously saved NVS credentials. Provides clear diagnostic feedback if credentials have never been saved or if the router is unreachable.
* **Parameters:** None.
* **Return Value:** `"Restore wifi connection to '<SSID>' successfully!"`.
* **CLI Syntax:** `cmd restore-wifi`

---

### 6.9 `wifi-status`
* **Purpose:** Queries whether the Wi-Fi Station interface is currently connected to an access point.
* **Parameters:** None.
* **Return Value:** `"OK"` (Connected) or `"NG"` (Disconnected).
* **CLI Syntax:** `cmd wifi-status`

---

### 6.10 `wifi-info`
* **Purpose:** Queries the saved router SSID and password currently stored in NVS.
* **Parameters:** None.
* **Return Value:** Formatted string (e.g., `"ssid = 'MyRouter', password = '***'"`).
* **CLI Syntax:** `cmd wifi-info`

---

### 6.11 `get-ip`
* **Purpose:** Queries the active IPv4 address assigned to the gateway.
* **Parameters:** None.
* **Return Value:** IPv4 address string (e.g., `"192.168.1.100"`).
* **CLI Syntax:** `cmd get-ip`

---

### 6.12 `scan-wifi`
* **Purpose:** Returns discovered Wi-Fi access points. Returns pre-cached boot scan results instantly or forces an active live channel scan.
* **Parameters:**
  * `--force` / `-f` (Optional, Boolean): If `true`, bypasses cache and performs an immediate live channel scan.
* **Return Value:** Comma-separated list of discovered network SSIDs.
* **CLI Syntax:** `cmd scan-wifi --force true`

---

### 6.13 `disable-wifi`
* **Purpose:** Disconnects and stops the physical Wi-Fi controller.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd disable-wifi`

---

### 6.14 `bt-connect`
* **Purpose:** Scans and connects to an external Bluetooth LE audio speaker.
* **Parameters:**
  * `--name` / `-n` (Required, String): Target speaker name or MAC address.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd bt-connect -n "SRS-XB23"`

---

### 6.15 `bt-disconnect`
* **Purpose:** Terminates the active Bluetooth speaker connection.
* **Parameters:** None.
* **Return Value:** `"Disconnected from Bluetooth speaker."`.
* **CLI Syntax:** `cmd bt-disconnect`

---

### 6.16 `bt-speak`
* **Purpose:** Synthesizes text into speech and streams audio directly to the connected Bluetooth speaker.
* **Parameters:**
  * `--text` / `-t` (Required, String): Text sentence to synthesize.
  * `--mode` / `-m` (Optional, String): Synthesis mode (`"cloud"` for high-fidelity TTS, or `"offline"` for S.A.M. speech synthesis).
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd bt-speak -t "System Online" -m "cloud"`

---

### 6.17 `bt-volume`
* **Purpose:** Sets the volume of the connected Bluetooth speaker.
* **Parameters:**
  * `--level` / `-l` (Required, Integer): Volume level from `0` (Mute) to `100` (Max).
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd bt-volume -l 80`

---

### 6.18 `bt-read-battery`
* **Purpose:** Queries the battery percentage of the connected Bluetooth speaker via GATT characteristic `0x2A19`.
* **Parameters:** None.
* **Return Value:** Battery percentage string (e.g., `"Speaker Battery Level: 85%"`).
* **CLI Syntax:** `cmd bt-read-battery`

---

## 7. Network Protocols & Web Server Commands

### 7.1 `http`
* **Purpose:** Sends an outbound HTTP/HTTPS request directly to a target web server. Automatically streams responses via Stream Protocol V2, handles custom headers, and auto-restores Wi-Fi if dropped.
* **Parameters:**
  * `--url` (Required, String): Full destination URL.
  * `--method` (Optional, String): Request method (`"GET"`, `"POST"`, `"HEAD"`). Default: `"GET"`.
  * `--body` (Optional, String): Request body payload (e.g., JSON string).
  * `--header` / `-h` (Optional, Multi-Argument): Custom HTTP header (can be specified up to 30 times).
* **CLI Syntax Examples:**
  ```text
  cmd http --url "https://reqbin.com/echo" --method GET
  
  # Multiple headers & JSON body:
  cmd http --method POST --url "http://192.168.1.50/api/data" \
    --header "Content-Type: application/json" \
    --header "Accept: application/json" \
    --body "{\"status\":\"active\"}"
  ```

---

### 7.2 `http-download`
* **Purpose:** Downloads a file from an authenticated HTTP/HTTPS server and streams it to storage.
* **Parameters:**
  * `--url` (Required, String): Target download URL.
  * `--filename` (Optional, String): Destination filename override.
  * `--header` (Optional, String): Authentication or custom header. Supports multiple `--header` flags.
* **Return Value:** `"File download successfully!"`.
* **CLI Syntax:** `cmd http-download --url "https://my-server.com/firmware.bin" --filename "fw.bin"`

---

### 7.3 `set-http-default-header`
* **Purpose:** Saves a default HTTP header globally in memory to be automatically attached to all subsequent outbound HTTP requests.
* **Parameters:**
  * `--header` / `-h` (Required, String): Raw header string (e.g., `"Authorization: Bearer <tok>"`).
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd set-http-default-header --header "Authorization: Bearer my_jwt_token"`

---

### 7.4 `delete-all-http-default-header`
* **Purpose:** Clears all globally saved default HTTP headers from memory.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd delete-all-http-default-header`

---

### 7.5 `ws-start`
* **Purpose:** Connects to an external WebSocket server.
* **Parameters:**
  * `--url` / `-u` (Required, String): WebSocket URI (e.g., `"ws://192.168.1.50:8080/ws"`).
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd ws-start --url "ws://192.168.1.50:8080/ws"`

---

### 7.6 `ws-send`
* **Purpose:** Transmits a text frame across the active outbound WebSocket connection.
* **Parameters:**
  * `--content` / `-c` (Required, String): String packet to send.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd ws-send --content "Hello Server"`

---

### 7.7 `ws-stop`
* **Purpose:** Closes the active outbound WebSocket connection.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd ws-stop`

---

### 7.8 `start-web-server`
* **Purpose:** Starts the complete Web Dashboard stack (REST API, mDNS hostname, and WebSocket server). In Normal Mode, validates that Wi-Fi is actively connected before starting. Returns the clickable URL.
* **Parameters:** None.
* **Return Value:** Accessible URLs (e.g., `"http://mgw-18a93114.local/ (http://192.168.0.124/)"`).
* **CLI Syntax:** `cmd start-web-server`

---

### 7.9 `get-web-address`
* **Purpose:** Queries the active URL of the running MGW Web Dashboard. Returns an error if the server is offline or if Wi-Fi has disconnected.
* **Parameters:** None.
* **Return Value:** Active URL string (e.g., `"http://mgw-18a93114.local/ (http://192.168.0.124/)"`).
* **CLI Syntax:** `cmd get-web-address`

---

### 7.10 `send-all-clients`
* **Purpose:** Broadcasts a WebSocket text frame to all connected browser dashboards.
* **Parameters:**
  * `--content` / `-c` (Required, String): Message string to broadcast.
* **Return Value:** The broadcasted string.
* **CLI Syntax:** `cmd send-all-clients --content "Sensor Alert: Threshold Exceeded"`

---

### 7.11 `update-firmware`
* **Purpose:** Saves firmware update parameters to NVS, sets boot mode to OTA, and restarts into an un-fragmented SRAM environment to download and flash new firmware.
* **Parameters:**
  * `--url` / `-u` (Optional, String): URL of target binary. Defaults to official GitHub catalog if omitted.
  * `--header` / `-h` (Optional, String): Custom authentication header for private OTA servers. Supports multiple `--header` flags.
* **Return Value:** `"System entering OTA mode. Restarting..."`.
* **CLI Syntax:** `cmd update-firmware --url "https://my-server.com/fw.bin"`

---

### 7.12 `get-firmware-list`
* **Purpose:** Downloads and returns the available firmware JSON release catalog from GitHub.
* **Parameters:** None.
* **Return Value:** Raw JSON array string.
* **CLI Syntax:** `cmd get-firmware-list`

---

### 7.13 `fs-list`
* **Purpose:** Queries directory entries from Host MCU storage across the configured bridge channel (`UART1` or `SPI1`).
* **Parameters:**
  * `--path` / `-p` (Optional, String): Directory path (default: `"/"`).
* **Return Value:** Stream Protocol V2 NDJSON file list chunks.
* **CLI Syntax:** `cmd fs-list --path "/logs"`

---

### 7.14 `fs-read`
* **Purpose:** Reads a file from Host MCU storage and streams it in chunks.
* **Parameters:**
  * `--path` / `-p` (Required, String): File path to stream.
* **Return Value:** Stream Protocol V2 binary payload slices.
* **CLI Syntax:** `cmd fs-read --path "/logs/sensor.csv"`

---

### 7.15 `fs-delete`
* **Purpose:** Deletes a file or directory from Host MCU storage.
* **Parameters:**
  * `--path` / `-p` (Required, String): Target path to delete.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd fs-delete --path "/logs/old.log"`

---

### 7.16 `fs-mkdir`
* **Purpose:** Creates a directory on Host MCU storage.
* **Parameters:**
  * `--path` / `-p` (Required, String): Directory path to create.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd fs-mkdir --path "/logs/2026"`

---

### 7.17 `ftp-start`
* **Purpose:** Starts the background RFC 959 passive mode FTP server daemon.
* **Parameters:**
  * `--port` / `-p` (Optional, Integer): TCP port to listen on (default: `21`).
* **Return Value:** `"FTP server started on port <port>."`.
* **CLI Syntax:** `cmd ftp-start --port 21`

---

### 7.18 `ftp-stop`
* **Purpose:** Stops the active FTP server daemon and closes client sockets.
* **Parameters:** None.
* **Return Value:** `"FTP server stopped."`.
* **CLI Syntax:** `cmd ftp-stop`

---

### 7.19 `ftp-status`
* **Purpose:** Queries whether the native FTP daemon is actively running.
* **Parameters:** None.
* **Return Value:** `"RUNNING"` or `"STOPPED"`.
* **CLI Syntax:** `cmd ftp-status`

---

## 8. SignalR Cloud Integration Commands

### 8.1 `signalr-start`
* **Purpose:** Connects to an ASP.NET Core SignalR Hub over WebSockets, completes the JSON v1 negotiation handshake (`{"protocol":"json","version":1}\x1E`), and starts background listening.
* **Parameters:**
  * `--url` / `-u` (Required, String): SignalR Hub WebSocket URI.
  * `--token` / `-t` (Optional, String): JWT Access Token string.
* **Return Value:** `"Connected to SignalR Hub successfully: '<url>'"`.
* **CLI Syntax:** `cmd signalr-start --url "ws://192.168.1.50:45455/chathub" --token "eyJhbGciOi..."`

---

### 8.2 `signalr-invoke`
* **Purpose:** Invokes a Remote Procedure Call (RPC) method on the connected SignalR Hub.
* **Parameters:**
  * `--method` / `-m` (Required, String): Target hub method name.
  * `--args` / `-a` (Optional, String): JSON array of arguments (e.g., `'["User","Message"]'`).
* **Return Value:** `"SignalR method '<method>' invoked successfully."`.
* **CLI Syntax:** `cmd signalr-invoke --method "sendToAll" --args "[\"MGW-01\",\"Online\"]"`

---

### 8.3 `signalr-status`
* **Purpose:** Queries whether the SignalR client is actively connected and handshaked.
* **Parameters:** None.
* **Return Value:** `"OK"` (Connected) or `"NG"` (Disconnected).
* **CLI Syntax:** `cmd signalr-status`

---

### 8.4 `signalr-stop`
* **Purpose:** Gracefully terminates the active SignalR Hub session.
* **Parameters:** None.
* **Return Value:** `"SignalR Hub disconnected successfully."`.
* **CLI Syntax:** `cmd signalr-stop`

---

## 9. Dynamic Web Plugin & Host MCU State Synchronization Commands

Allows Host MCUs (STM32, NXP) to inject their own HTML5/JS control panels directly into the MGW Web Dashboard and synchronize UI states.

### 9.1 `register-web-plugin`
* **Purpose:** Registers a Host MCU web application with the gateway. Triggers a live WebSocket broadcast to dynamically add the item to the dashboard sidebar menu in real time without refreshing the page.
* **Parameters:**
  * `--name` / `-n` (Required, String): Unique plugin key (lowercase alphanumeric, e.g., `"motor-ctrl"`).
  * `--title` / `-t` (Required, String): Display title in the dashboard menu.
  * `--icon` / `-i` (Optional, String): SVG micro-icon name (`"motor"`, `"gauge"`, `"chart"`, `"power"`, `"sensor"`, `"cpu"`, `"tool"`). Default: `"plugin"`.
  * `--size` / `-s` (Optional, Integer): Expected size in bytes of the single HTML bundle.
* **Return Value:** `"Plugin '<name>' registered successfully."`.
* **CLI Syntax:** `cmd register-web-plugin -n motor-ctrl -t "Motor Controller" -i "motor" --size 32768`

---

### 9.2 `unregister-web-plugin`
* **Purpose:** Removes an active web plugin and broadcasts a live deletion update to all connected browser dashboards.
* **Parameters:**
  * `--name` / `-n` (Required, String): Plugin key to unregister.
* **Return Value:** `"Plugin unregistered."`.
* **CLI Syntax:** `cmd unregister-web-plugin -n motor-ctrl`

---

### 9.3 `list-web-plugins`
* **Purpose:** Lists all currently registered Host MCU web plugins.
* **Parameters:** None.
* **Return Value:** Formatted multi-line text list showing index, name, title, icon, and size.
* **CLI Syntax:** `cmd list-web-plugins`

---

### 9.4 `set-max-plugin-size`
* **Purpose:** Configures the maximum allowed size ceiling for Host MCU web plugin HTML bundles and saves it to persistent storage (`plugin_max_sz`).
* **Parameters:**
  * `--size` / `-s` (Required, Integer): Maximum size in bytes (e.g., `1048576` for 1MB).
* **Return Value:** `"Max plugin size set to <bytes> bytes."`.
* **CLI Syntax:** `cmd set-max-plugin-size --size 1048576`

---

### 9.5 `get-max-plugin-size`
* **Purpose:** Queries the active maximum allowed size ceiling for web plugin bundles from persistent storage.
* **Parameters:** None.
* **Return Value:** Formatted string (e.g., `"1048576 bytes (1024 KB)"`).
* **CLI Syntax:** `cmd get-max-plugin-size`

---

### 9.6 `is-plugin-visible`
* **Purpose:** Queries whether a specific plugin is currently visible on the user's active browser screen. Allows the Host MCU to pause heavy telemetry streaming and enter low-power sleep when the user navigates away.
* **Parameters:**
  * `--name` / `-n` (Required, String): Plugin key to query.
* **Return Value:** `"true"` (dashboard open on this plugin) or `"false"` (closed or viewing another panel).
* **CLI Syntax:** `cmd is-plugin-visible -n motor-ctrl`

---

### 9.7 `get-active-view`
* **Purpose:** Queries the exact menu item or plugin currently visible on the active Web Dashboard.
* **Parameters:** None.
* **Return Value:** View identifier string: `"plugin:<name>"`, `"p1"`, `"p2"`, `"p3"`, `"p4"`, `"p5"`, `"p6"`, or `"none"` (dashboard closed).
* **CLI Syntax:** `cmd get-active-view`

---

### 9.8 `get-dashboard-status`
* **Purpose:** Queries whether any browser is actively connected to the Web Dashboard. Automatically detects window closure via beacons and 6-second heartbeat expiration.
* **Parameters:** None.
* **Return Value:** `"ONLINE"` (active browser connected) or `"OFFLINE"` (all browser tabs closed).
* **CLI Syntax:** `cmd get-dashboard-status`

---

## 10. Memory Diagnostics & Auditing Commands

### 10.1 `enable-memory-record`
* **Purpose:** Enables dynamic memory allocation tracking and record auditing.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd enable-memory-record`

---

### 10.2 `disable-memory-record`
* **Purpose:** Disables dynamic memory allocation tracking to optimize execution speed.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd disable-memory-record`

---

### 10.3 `audit-memory`
* **Purpose:** Triggers an immediate diagnostic audit of allocated heap structures and VMM PSRAM pools.
* **Parameters:** None.
* **Return Value:** `"OK"`.
* **CLI Syntax:** `cmd audit-memory`

---

## Next Steps

* Refer to the **Unified Customer Communication Protocols & Host MCU Driver Manual** for protocol timing and packet diagrams.
* Check the **Error Codes Reference** for troubleshooting failed command return frames.