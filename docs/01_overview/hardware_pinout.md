---
sidebar_position: 3
title: Hardware Pinout & Wiring
description: Detailed physical pin mappings, standalone module pinout, and electrical guidelines for wiring your Host MCU to the MGW-IoT-002 gateway.
---

# Hardware Pinout & Wiring

This section outlines the physical pin assignments and electrical interface for the standalone **`MGW-IoT-002`** hardware module. 

The module is engineered as an autonomous surface-mount / plug-in coprocessor. By wiring standard serial communication lines between your Host Microcontroller (STM32, NXP, Microchip, PIC, AVR) and the module pins, your system gains instant access to cloud networks, local web dashboards, and remote storage.

---

## 1. System Interconnection Overview

The Host MCU communicates with the `MGW-IoT-002` module across standard physical interfaces:
* **UART Console Interface:** For ASCII commands, diagnostic logs, and serial streaming.
* **High-Speed SPI Slave Interface:** For high-throughput transfers (up to 20–40 MHz), web plugins, and large file streaming.
* **Hardware Handshake Lines:** Real-time interrupt signaling (`DATA_READY`) and hardware flow control (`HANDSHAKE`).

```text
       [ External Customer Host MCU ]                 [ MGW-IoT-002 Gateway Module ]
  ┌─────────────────────────────────────┐         ┌─────────────────────────────────────┐
  │ SPI Master: Chip Select (CS)        │────────►│ Pin 12 (IO10) : SPI_CS              │
  │ SPI Master: MOSI                    │────────►│ Pin 9  (IO7)  : SPI_MOSI            │
  │ SPI Master: Serial Clock (SCK)      │────────►│ Pin 8  (IO6)  : SPI_SCK             │
  │ SPI Master: MISO                    │◄────────│ Pin 4  (IO2)  : SPI_MISO            │
  │                                     │         │                                     │
  │ External Interrupt (EXTI Input)     │◄────────│ Pin 5  (IO3)  : WIFI_DATA_READY     │
  │ Flow Control (Output / GPIO)        │────────►│ Pin 17 (IO4)  : WIFI_HANDSHAKE      │
  │                                     │         │                                     │
  │ UART Controller: TX                 │────────►│ Pin 21 (IO23) : UART_RX             │
  │ UART Controller: RX                 │◄────────│ Pin 23 (IO24) : UART_TX             │
  │                                     │         │                                     │
  │ Power Supply: +3.3V DC              │────────►│ Pin 2         : 3V3                 │
  │ Common Reference Ground             │────────►│ Pin 1, 28     : GND                 │
  └─────────────────────────────────────┘         └─────────────────────────────────────┘
```

---

## 2. MGW-IoT-002 Module Pinout Specification

| Module Pin # | Pin Name | Direction | Connect to Host MCU | Description |
| :---: | :--- | :---: | :--- | :--- |
| **1** | `GND` | Power | System Ground | Common Ground Reference Plane |
| **2** | `3V3` | Power | +3.3V Power Supply | Regulated 3.3V DC Power Input |
| **3** | `EN` | Input | System Reset (Optional) | Active-High Chip Enable / Hardware Reset |
| **4** | `IO2` | Output | Host SPI MISO | High-Speed SPI Master-In Slave-Out (DMA driven) |
| **5** | `IO3` | Output | Host External Interrupt | **WIFI_DATA_READY:** MGW signals Host to clock out data |
| **8** | `IO6` | Input | Host SPI SCK | High-Speed SPI Serial Clock (Clocked by Host) |
| **9** | `IO7` | Input | Host SPI MOSI | High-Speed SPI Master-Out Slave-In |
| **12** | `IO10` | Input | Host SPI CS | SPI Chip Select (Active Low) |
| **15** | `IO28` | Input | Reset Button (Optional) | **Factory Reset:** Hold LOW for 10 seconds to factory reset |
| **16** | `IO5` | Input | Host UART RTS (Optional) | UART Hardware Flow Control (RTS) |
| **17** | `IO4` | Input | Host Flow Control (Optional) | **WIFI_HANDSHAKE:** Hardware flow control for SPI |
| **21** | `IO23` | Input | Host UART TX | UART Receive (MGW RX line) |
| **23** | `IO24` | Output | Host UART RX | UART Transmit (MGW TX line) |
| **24** | `RX0` | Input | Debug UART (Optional) | Internal System Diagnostic Console RX |
| **25** | `TX0` | Output | Debug UART (Optional) | Internal System Diagnostic Console TX |
| **28** | `GND` | Power | System Ground | Common Ground Reference Plane |

*(Note: Pins not listed in this table are reserved for internal module functions or dedicated GPIO expansions and may be left floating).*

---

## 3. Electrical Guidelines & Interfacing Best Practices

To ensure signal integrity and robust operation in demanding industrial automation environments:

### 3.1 3.3V Logic Level Standard
* All digital I/O lines on the `MGW-IoT-002` module operate strictly at **3.3V CMOS logic levels**.
* **CRITICAL WARNING:** Do not connect 5V signals directly to any pin of the module. Interfacing with 5V microcontrollers requires external bi-directional level shifters.

### 3.2 High-Speed SPI Layout Guidelines (20 MHz – 40 MHz)
When routing the high-speed SPI lines (`IO10`, `IO7`, `IO6`, `IO2`) on your motherboard:
* Keep trace lengths as short and direct as possible ($\le 10\text{ cm}$).
* Maintain solid ground reference planes directly underneath the SPI bus traces to minimize signal reflections and crosstalk.
* If trace lengths exceed $15\text{ cm}$, add small series termination resistors ($22\ \Omega - 33\ \Omega$) on `SCK`, `MOSI`, and `MISO` close to the driver pins.

### 3.3 Software-Enabled Internal Pull-Ups
* The `MGW-IoT-002` firmware automatically enables internal $\approx 45\text{ k}\Omega$ pull-up resistors on critical input lines (including `IO4` / `WIFI_HANDSHAKE` and `IO28` / `FACTORY_RESET`). 
* External pull-up resistors are not required on your motherboard, simplifying your bill of materials (BOM).

---

## Next Steps

* Learn about the **[Communication Protocols](../02_communication_protocols/data_transfer_modes.md)** supported by the module.
* Read the **[High-Speed SPI Integration Guide](../02_communication_protocols/spi_high_speed.md)** for detailed timing diagrams and Host MCU driver implementations.
