---
sidebar_position: 1
slug: /
title: Introduction
description: Official Technical Documentation and Developer Portal for the Mini IoT Gateway family (MGW-IoT-001 / MGW-IoT-002) distributed by CONSTH Vietnam.
---

# CONSTH MGW Developer Center

Welcome to the official developer portal for the **Mini IoT Gateway family (`MGW-IoT-001` / `MGW-IoT-002`)**, developed, manufactured, and distributed by [**Công ty Cổ phần Thương mại CONSTH (CONSTH Vietnam)**](https://consth.vn/).

The MGW series provides high-performance, industrial-grade Edge IoT coprocessors engineered to bridge external Host Microcontrollers (STM32, NXP, Microchip, PIC, AVR) to enterprise automation networks, Wi-Fi 6 infrastructure, Bluetooth LE peripherals, local Web Dashboards, and Cloud backends.

---

## Supported Product Family

| Product Code | Description | Target Use Case |
| :--- | :--- | :--- |
| **`MGW-IoT-001`** | Standard Mini IoT Gateway | Commercial automation & serial-to-cloud bridge |
| **`MGW-IoT-002`** | High-Speed Industrial IoT Gateway | Dual-Active UART + SPI High-Speed Machine Bus |

---

## Key Capabilities

* **Dual-Active Host Interface:** Concurrent **UART1** and **High-Speed SPI1** (DMA + Hardware Handshake) support.
* **Stream Protocol V2:** Resilient, HTTP-style chunked binary streaming with cumulative IEEE 802.3 CRC32 verification.
* **Zero-Allocation Host MCU C SDK:** Lightweight ANSI C drivers (`mgw_host_mcu_parser` & `mgw_host_file_bridge`) running in ≤ 560 bytes of RAM.
* **Dynamic Web Plugins:** Stream custom HTML5/CSS/JS control panels directly from your Host MCU SD card into the MGW Web Dashboard.
* **Native File Explorer & FTP Server:** Manage Host MCU storage wirelessly from a web browser or Windows File Explorer.
* **Factory Provisioned & Pre-Activated:** All production units shipped by [CONSTH](https://consth.vn/) are pre-activated and ready for immediate deployment out-of-box.

---

## Commercial Distribution & Support

The Mini IoT Gateway product line is backed by [**CONSTH Vietnam**](https://consth.vn/). For commercial inquiries, volume distribution, custom hardware integrations, or technical support, please visit:

🌐 **Official Corporate Website:** [https://consth.vn/](https://consth.vn/)

---

## Getting Started

* 👉 **[Quick Start Guide](./quick_start.md)** — Connect to the configuration portal and set up your gateway.
* 🔌 **[Hardware Pinout & Wiring](./hardware_pinout.md)** — View the `CN42` connector mapping and electrical specs.