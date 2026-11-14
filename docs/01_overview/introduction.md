---
sidebar_position: 1
slug: /
title: Introduction
description: Official Technical Documentation and Developer Portal for the Mini IoT Gateway (MGW-IoT-002).
---

# AIoT Tech MGW Developer Center

Welcome to the official documentation portal for the **Mini IoT Gateway (`MGW-IoT-002`)**.

The `MGW-IoT-002` is an industrial-grade Edge IoT coprocessor designed to bridge external Host Microcontrollers (STM32, NXP, Microchip, PIC, AVR) to enterprise Wi-Fi 6 networks, Bluetooth LE peripherals, local Web Dashboards, and Cloud SignalR backends.

---

## Key Capabilities

* **Dual-Active Host Interface:** Concurrent **UART1** and **High-Speed SPI1** (DMA + Hardware Handshake) support.
* **Stream Protocol V2:** Resilient, HTTP-style chunked binary streaming with cumulative IEEE 802.3 CRC32 verification.
* **Zero-Allocation Host MCU C SDK:** Lightweight ANSI C drivers (`mgw_host_mcu_parser` & `mgw_host_file_bridge`) running in ≤ 560 bytes of RAM.
* **Dynamic Web Plugins:** Stream custom HTML5/CSS/JS control panels directly from your Host MCU SD card into the MGW Web Dashboard.
* **Native File Explorer & FTP Server:** Manage Host MCU storage wirelessly from a browser or Windows File Explorer.

---

## Getting Started

* 👉 **[Quick Start Guide](./quick_start.md)** — Connect to the configuration portal and set up your gateway.
* 🔌 **[Hardware Pinout & Wiring](./hardware_pinout.md)** — View the `CN42` connector mapping and electrical specs.