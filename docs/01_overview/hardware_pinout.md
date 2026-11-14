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
```

---

### Step 2: Update `docs/01_overview/hardware_pinout.md`

Replace the contents of `docs/01_overview/hardware_pinout.md` with this clean version:

```markdown
---
sidebar_position: 3
title: Hardware Pinout & Wiring
description: Detailed physical pin mappings, connector specifications, and electrical guidelines for wiring your Host MCU to the MGW-IoT-002 gateway.
---

# Hardware Pinout & Wiring

This section outlines the physical interface connectors and pin assignments for the `MGW-IoT-002` gateway module. Proper wiring ensures reliable communication between your Host Microcontroller (STM32, NXP, Microchip, etc.) and the gateway over UART or high-speed SPI.

---

## 1. Physical Interface Overview

The `MGW-IoT-002` module communicates with external host hardware through a standardized 20-pin board-to-board connector (`CN WIFI` on the gateway side, mating with `CN42` on the Host MCU motherboard).

```text
       [ HOST MCU Motherboard (CN42) ]               [ MGW Gateway Module (CN WIFI) ]
  ┌─────────────────────────────────────┐         ┌─────────────────────────────────────┐
  │ Pin 3   : SPI_WIFI_CS               │────────►│ Pin 5   : IO10 (Chip Select)        │
  │ Pin 5   : SPI_WIFI_MOSI             │────────►│ Pin 7   : IO7  (Master Out, Slave In)│
  │ Pin 9   : SPI_WIFI_SCK              │────────►│ Pin 9   : IO6  (Serial Clock)       │
  │ Pin 15  : SPI_WIFI_MISO             │◄────────│ Pin 15  : IO2  (Master In, Slave Out)│
  │ Pin 14  : WIFI_DATA_READY           │◄────────│ Pin 14  : IO3  (MGW Interrupt Out)  │
  │ Pin 13  : WIFI_HANDSHAKE            │────────►│ Pin 13  : IO4  (Hardware Flow Control)│
  │ Pin 4   : USART2_TX                 │◄────────│ Pin 4   : IO24 (UART TX)            │
  │ Pin 6   : USART2_RX                 │────────►│ Pin 6   : IO23 (UART RX)            │
  │ Pin 19,20: GND                      │────────►│ Pin 17,19: GND                      │
  └─────────────────────────────────────┘         └─────────────────────────────────────┘
```

---

## 2. Connector Pin Mapping Table (`CN42` / `CN WIFI`)

| Pin # | Signal Name (Host MCU `CN42`) | MGW Internal GPIO (`CN WIFI`) | Direction | Description |
| :---: | :--- | :--- | :---: | :--- |
| **1, 2** | `VCC_WIFI` / `+3.3V` | `3V3` | Power | Regulated 3.3V Power Supply input |
| **3** | `SPI_WIFI_CS` | **`IO10`** | Host → MGW | SPI Chip Select (Active Low) |
| **4** | `USART2_TX` | **`IO24`** | Host → MGW | UART Transmit (Host to MGW RX) |
| **5** | `SPI_WIFI_MOSI` | **`IO7`** | Host → MGW | High-Speed SPI Master-Out Slave-In |
| **6** | `USART2_RX` | **`IO23`** | MGW → Host | UART Receive (Host from MGW TX) |
| **7** | `SPI_WIFI_SCK` | **`IO6`** | Host → MGW | High-Speed SPI Serial Clock (Master driven) |
| **11** | `WIFI_HANDSHAKE` | **`IO5`** | Host → MGW | UART Hardware Flow Control (RTS) |
| **13** | `WIFI_HANDSHAKE` | **`IO4`** | Host → MGW | SPI Hardware Flow Control (Busy / Ready) |
| **14** | `WIFI_DATA_READY` | **`IO3`** | MGW → Host | **External Interrupt:** MGW signals Host that data is ready to clock out |
| **15** | `SPI_WIFI_MISO` | **`IO2`** | MGW → Host | High-Speed SPI Master-In Slave-Out (DMA driven) |
| **19, 20**| `GND` | `GND` | Ground | Common system ground reference plane |

---

## 3. Electrical Guidelines & Best Practices

To ensure signal integrity and reliable operation in noisy industrial environments:

1. **Logic Level (3.3V Tolerant):**
   * All digital I/O lines on the `MGW-IoT-002` operate strictly at **3.3V CMOS logic levels**. 
   * **WARNING:** Do not connect 5V signals directly to any pin. Interfacing with 5V microcontrollers (such as legacy AVR Arduino boards) requires external level shifters.
2. **Ground Plane Reference:**
   * Always ensure Pins 17, 19, and 20 (`GND`) have a solid, low-impedance connection to the Host MCU ground plane, especially when operating high-speed SPI at **20 MHz+**.
3. **Internal Pull-Ups:**
   * Control lines like `WIFI_HANDSHAKE` and `WIFI_DATA_READY` feature software-enabled internal pull-up resistors on the gateway, but clean PCB routing away from high-voltage AC lines or motor PWM traces is strongly recommended.

---
