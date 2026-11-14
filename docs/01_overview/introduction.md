---
sidebar_position: 1
slug: /
title: Introduction
description: Official Technical Documentation and Developer Portal for the Mini IoT Gateway family (MGW-IoT-001 / MGW-IoT-002) distributed by CONSTH Vietnam.
---

import useBaseUrl from '@docusaurus/useBaseUrl';

# CONSTH MGW Developer Center

Welcome to the official developer portal for the **Mini IoT Gateway (`MGW-IoT-002`)**, developed, manufactured, and distributed by [**Công ty Cổ phần Thương mại CONSTH (CONSTH Vietnam)**](https://consth.vn/).

The `MGW-IoT-002` is an industrial-grade, surface-mount Edge IoT coprocessor engineered to bring instant wireless networking, remote control capabilities, and cloud connectivity to any external embedded microcontroller or industrial machine.

<div style={{ textAlign: 'center', margin: '30px 0' }}>
  <img 
    src={useBaseUrl('/img/mgw_module.png')} 
    alt="CONSTH MGW-IoT-002 Module" 
    style={{ maxWidth: '340px', borderRadius: '12px', boxShadow: '0 8px 24px rgba(0,0,0,0.15)' }} 
  />
  <p style={{ fontSize: '0.85rem', color: '#64748b', marginTop: '8px' }}>
    <em>Figure 1: The CONSTH MGW-IoT-002 Hardware Module with Integrated RF Shielding</em>
  </p>
</div>

---

## What is MGW-IoT-002?

In traditional embedded engineering, adding modern Wi-Fi 6, Bluetooth LE, local web servers, and cloud telemetry directly onto a host microcontroller (such as an STM32, NXP, Microchip, or PIC) requires massive RAM overhead, complex network stacks, and months of driver development.

The **`MGW-IoT-002`** completely solves this problem. It acts as an autonomous coprocessor: all you need to do is connect your Host MCU to the gateway over **UART** or **high-speed SPI** and send human-readable, command-prompt-style commands (e.g., `cmd set-wifi`, `cmd http`, `cmd version`).

---

## Core Application Scenarios

Following the proven architecture of the MGW gateway family, the `MGW-IoT-002` provides three fundamental operational pillars:

### 1. Universal Network Connectivity
The gateway acts as an intelligent bridge connecting your Host MCU to local devices, automation networks, and remote cloud infrastructure:
* **Wi-Fi 6 Infrastructure:** Outbound HTTP/HTTPS REST requests, file downloads, and WebSockets.
* **Bluetooth LE (BLE 5.3):** Wireless peripheral pairing, sensor data acquisition, and configuration.
* **Dual-Active Host Interface:** Supports standard asynchronous **UART1** and high-speed **SPI1 (DMA + Hardware Handshake)** running concurrently up to 20–40 MHz.

### 2. Direct Peripheral & Remote Control
In addition to bridging data, the module features on-board General Purpose Input/Output (GPIO) pins. You can directly control external hardware peripherals with minimal host firmware overhead:
* **Digital Outputs:** Drive relays, indicator LEDs, or industrial solenoid valves (`cmd gpio-set`).
* **Digital Inputs:** Monitor limit switches, optical sensors, or alarm interlocks (`cmd gpio-get`).

### 3. Cloud Firmware Updates (OTA)
The gateway eliminates the need for physical programming cables in the field. Firmware can be updated Over-the-Air (OTA) directly through the internet:
* Whenever new features, bug fixes, or custom personas are released, the gateway securely fetches and flashes the binary image from cloud servers while maintaining partition rollback safety.

---

## Advanced Enterprise Capabilities

Beyond basic connectivity, the `MGW-IoT-002` incorporates modern SaaS edge features:
* **Built-in Web Dashboard:** A responsive SaaS administrative dashboard hosted directly on the module for status monitoring and configuration.
* **Dynamic Web Plugins:** Allows your Host MCU to stream its own custom HTML5/CSS/JS control panel (micro-frontend) directly into the gateway's web dashboard.
* **Stream Protocol V2:** Lightweight HTTP-style chunked transfer (`<hex_len>\r\n<payload>\r\n`) with cumulative IEEE 802.3 CRC32 verification.
* **Zero-Allocation Host MCU C Driver:** The official open-source drivers (`mgw_host_mcu_parser` and `mgw_host_file_bridge`) operate in ≤ 560 bytes of static RAM with zero dynamic memory allocation (`malloc`).

---

## Product Family Comparison

| Model | Host Interfaces | Maximum Bus Speed | Target Applications |
| :--- | :--- | :---: | :--- |
| **`MGW-IoT-001`** | UART | Up to 921.6 Kbps | Standard serial-to-Wi-Fi/BLE telemetry & basic IoT bridging |
| **`MGW-IoT-002`** | **Dual-Active UART + SPI (DMA)** | **Up to 20–40 MHz** | High-speed industrial machines, file explorers, dynamic web plugins |

---

## Commercial Distribution & Support

The Mini IoT Gateway product line is backed and distributed by [**Công ty Cổ phần Thương mại CONSTH (CONSTH Vietnam)**](https://consth.vn/). Every production unit is fully tested, provisioned, and pre-activated at the factory, arriving ready for immediate deployment out-of-box.

For commercial inquiries, volume procurement, custom hardware integrations, or technical support, please visit:

🌐 **Official Corporate Website:** [https://consth.vn/](https://consth.vn/)

---

## Next Steps

* 👉 **[Quick Start Guide](./quick_start.md)** — Power up your gateway and perform initial settings.
* 🔌 **[Hardware Pinout & Wiring](./hardware_pinout.md)** — Learn how to connect your Host MCU via the `CN42` connector.