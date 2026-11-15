---
sidebar_position: 1
title: Security & Authentication
description: Guide to the MGW Dual Trust Zone security model, Host MCU stateful hardware sessions, wireless access tokens, and credential management.
---

# Security & Authentication

Industrial Edge IoT gateways must strike a critical balance between **high-speed serial performance** for the Host MCU and **strict zero-trust security** across wireless networks.

To achieve both without compromise, the **Mini IoT Gateway (`MGW-IoT-002`)** enforces a **Dual Trust Zone Security Policy**.

---

## 1. The Dual Trust Zone Architecture

```text
  ┌────────────────────────────────────────────────────────────────────────────┐
  │                       MGW DUAL TRUST ZONE ARCHITECTURE                     │
  ├────────────────────────────────────────────────────────────────────────────┤
  │ ZONE 1: PHYSICAL HARDWARE BUS (UART1 / SPI1) ──► STATEFUL SESSION          │
  │ • Host MCU authenticates ONCE on boot via 'cmd login'                      │
  │ • Physical bus remains unlocked in gateway memory until reboot             │
  │ • Zero token overhead on all subsequent serial commands                    │
  ├────────────────────────────────────────────────────────────────────────────┤
  │ ZONE 2: WIRELESS & NETWORK (Wi-Fi / Web / BLE) ──► STATELESS TOKENS        │
  │ • Zero implicit trust across wireless interfaces                           │
  │ • Every protected command MUST include a 44-character Access Token         │
  │ • Web Dashboard enforces a 15-minute inactivity auto-logout                │
  └────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Zone 1: Physical Hardware Bus (UART & SPI)

When your Host Microcontroller connects directly to the MGW module via physical PCB traces (UART1 or SPI1), appending long cryptographic tokens to every serial command wastes bandwidth and complicates MCU string handling.

### One-Time Boot Handshake
Instead, your Host MCU performs a **single login command** upon booting:

```text
cmd login -u "admin" -p "admin"\r\n
```

Once the gateway verifies the credentials and returns `Result: success`, **the physical hardware channel (UART and SPI) is unlocked for the remainder of the power cycle.**

Your Host MCU can now send all protected commands directly without passing any token:

```text
cmd set-time -t "05/11/2026 20:15:30"\r\n
cmd register-web-plugin -n "motor_ctrl" -t "Motor Controller" -i "motor"\r\n
```

---

## 3. Zone 2: Wireless & Network Access (Web & BLE)

Commands arriving over wireless interfaces (HTTP REST, WebSockets, or Bluetooth LE) are treated as untrusted external traffic.

### Per-Command Token Verification
1. When a wireless client logs in via `cmd login`, the gateway returns a **44-character Base64 Access Token**.
2. Every protected command submitted over Web or BLE must attach this token using the `--token` flag:

```text
cmd set-time -t "05/11/2026 20:15:30" --token "p7mQC4o6daZhRPTKvlkcbgZQUhJ5lSPj8Leq1ByzBM8="
```

If the token is missing or invalid, the gateway rejects the command immediately with error code `0x9005` (`Invalid Access Token`).

### Web Dashboard Session Protections
The built-in MGW Web Dashboard automatically manages tokens for operators using a 3-layer security shield:
* **Tab Isolation:** Tokens are stored exclusively in browser session memory and are destroyed immediately when the browser tab is closed.
* **15-Minute Inactivity Timeout:** If an operator leaves the browser idle for 15 minutes, the session expires automatically and forces a return to the login screen.
* **Manual Sign Out:** Clicking **Sign Out** in the top header immediately purges active credentials.

---

## 4. Command Permission Categories

Every gateway command falls into one of two access levels:
* **Diagnostic & Onboarding Commands:** Free to run across all interfaces without requiring authentication (e.g., `cmd version`, `cmd echo`, `cmd get-mac`, `cmd is-plugin-visible`, `cmd get-active-view`).
* **Protected System Commands:** Require an unlocked hardware bus session or a valid `--token` when a custom password is active (e.g., `cmd set-time`, `cmd set-wifi`, `cmd setting-uart`, `cmd factory-reset`, `cmd update-firmware`).

---

## 5. Updating Credentials & Factory Recovery

### Updating Username and Password
Default factory credentials are **`admin`** / **`admin`**. We strongly recommend changing these prior to field deployment using:

```text
cmd set-credentials --new_user "operator" --old_pass "admin" --new_pass "StrongPass123!"\r\n
```

Or update only the password:

```text
cmd set-password -p "StrongPass123!" -c "StrongPass123!"\r\n
```

### Password Recovery (Hardware Factory Reset)
If administrator credentials are lost or forgotten, you can restore factory defaults (`admin` / `admin`) at any time:
* **Physical Reset Pin:** Ground **Pin 15 (`IO28`)** continuously for **10 seconds** while the gateway is powered on.
* **Result:** All custom passwords, Wi-Fi credentials, and serial overrides are wiped, and the gateway reboots into Configure Mode.

---

## Next Steps

* Learn how to connect your gateway to enterprise cloud hubs in the **SignalR Cloud Integration Guide**.
