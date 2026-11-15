---
sidebar_position: 3
title: OTA Firmware Updates
description: Guide to performing safe, remote Over-the-Air (OTA) firmware updates on the MGW gateway via the Web Dashboard or CLI commands.
---

# OTA Firmware Updates

The **Mini IoT Gateway (`MGW-IoT-002`)** supports secure **Over-the-Air (OTA)** firmware updates over Wi-Fi, allowing you to deploy new features, security patches, and custom builds to devices in the field without physical programming cables.

The gateway uses a **dedicated dual-partition update architecture** with built-in image validation and automatic rollback protection.

---

## 1. Prerequisites

Before initiating an OTA firmware update, ensure:
1. **Active Internet / Network Link:** The gateway must be connected to a Wi-Fi router with access to the firmware server (verified via `cmd wifi-status` returning `OK`).
2. **Stable Power Supply:** Do not disconnect power while the update is downloading and flashing.
3. **Authentication:** Because updating firmware is a protected system operation, ensure your session is authenticated (logged in over UART/SPI, or passing `--token` over wireless interfaces).

---

## 2. Method 1: Updating via the Web Dashboard

For technicians using a browser on-site or over a local network:

1. Open the **MGW Admin Center** (`http://mgw-xxxxxxxx.local/` or `http://192.168.4.1`) and sign in.
2. In the left sidebar, click **Firmware Update**.
3. The panel displays your **Current Version** and automatically fetches the **Available Personas** from the official release catalog.
4. Select your target firmware version from the dropdown menu.
5. Click **[ Download & Flash ]** and confirm the prompt.

The gateway will automatically restart, download the binary image, validate its checksum, and reboot into the new firmware.

---

## 3. Method 2: Updating via CLI / Host MCU

Host Microcontrollers and automated scripts can query available firmware releases and trigger updates programmatically.

### A. Query Available Releases
To fetch the JSON catalog of available firmware builds:

```text
cmd get-firmware-list\r\n
```

Example response body:

```json
[
  {
    "name": "MGW Core",
    "version": "1.6.0",
    "url": "https://your-server.com/bin/mgw_core_v1.6.0.bin"
  }
]
```

### B. Trigger Update (Default Catalog)
To update using the default official release channel:

```text
cmd update-firmware\r\n
```

### C. Trigger Update (Custom or Private Server)
You can point the gateway to any custom HTTP/HTTPS server, and optionally attach custom authentication headers (such as a Bearer token) if your server is private:

```text
cmd update-firmware --url "https://ota.yourcompany.com/firmware.bin" --header "Authorization: Bearer my_secret_token"\r\n
```

*(Multiple `--header` flags are supported if your server requires additional headers).*

---

## 4. Safe Update Lifecycle & Rollback Protection

When `cmd update-firmware` is executed, the gateway performs a safe, deterministic update cycle:

```text
  [ Active Operation ] ──► 1. Saves target URL & headers safely
                                │
                                ▼
  [ Dedicated Update State ] ◄── 2. Reboots cleanly to maximize available memory
                                │
                                ▼
  [ Download & Flash ] ──────► 3. Downloads binary in chunks with 3-pass auto-retry
                                │
                                ▼
  [ Validation & Swap ] ─────► 4. Verifies image integrity & switches boot partition
                                │
                                ▼
  [ Updated Normal Mode ] ◄─── 5. Reboots into new firmware automatically
```

### Built-In Fail-Safe Rollback
* **Network Drops:** If the Wi-Fi link drops during download, the gateway automatically retries up to 3 times. If the download still fails, it aborts safely and reboots back into your existing working firmware.
* **Corrupted Images:** Before switching boot partitions, the gateway verifies the binary checksum and header signatures. Any invalid image is rejected automatically, ensuring your gateway can never be bricked by a bad download.

---

## Next Steps

* Learn how to manage Host MCU files wirelessly using native desktop tools in the **Native FTP Server Guide**.
