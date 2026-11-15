---
sidebar_position: 1
title: Integration Overview
description: High-level overview of the Host MCU integration strategy using the Unified Parser and File Bridge drivers.
---

# Host MCU Integration Overview

Integrating your Host Microcontroller (STM32, NXP, Microchip, PIC, AVR) with the MGW-IoT-002 gateway does not require writing complex network stacks, JSON parsers, or filesystem handlers. We provide a Unified Host MCU Driver that handles the heavy lifting for you.

---

## 1. The 3-Tier Integration Strategy

To keep your firmware footprint minimal and your development speed high, we provide two pre-compiled, 100% ANSI C-compliant driver components. You simply drop these two files into your Host MCU project and implement a small set of callbacks.

### A. Unified Host MCU Parser (mgw_host_mcu_parser)
This is a Zero-Allocation Finite State Machine (FSM). It consumes your raw UART/SPI bytes and automatically manages:
* **Stream Protocol V2:** Transparently handles framing, chunking, and CRC32 integrity validation.
* **Metadata Extraction:** Automatically parses internal protocol headers (e.g., Data-Type, File-Action, Plugin-Action).
* **Resiliency:** Handles arbitrary frame fragmentation, line noise, and back-to-back packets with 0% data loss.

### B. Companion File Bridge (mgw_host_file_bridge)
This driver acts as the "Glue" between the gateway's web file explorer and your local filesystem (e.g., FatFs, LittleFS). You map your native file functions (open, read, write, delete) to the bridge's callbacks, and the bridge handles all the heavy lifting of:
* **Resumable Slicing:** Managing byte-offsets for robust transfers.
* **Cancellation Purging:** Automatically purging partial files from your SD card if the user cancels a download.
* **Web Plugin Streaming:** Streaming HTML bundles from your storage to the gateway's dashboard on-demand.

---

## 2. Driver Blueprint Standard

| Driver Component | Responsibility | Memory Impact |
| :--- | :--- | :---: |
| Parser Engine | Raw framing, CRC32, and protocol state. | 560 units of Static RAM |
| File Bridge | FatFS/LittleFS callback mapping. | 0 units (Stack/Dynamic) |

---

## 3. Integration Lifecycle

The integration workflow for any Host MCU is a 3-step process:

1. **Physical Transport:** Implement the UART/SPI physical read/write interrupts (or DMA callbacks).
2. **Parser Registration:** Initialize `mgw_unified_parser_t` with the parser callbacks to route commands and data chunks.
3. **Bridge Binding:** Initialize `mgw_host_file_bridge` by linking your filesystem calls (open, read, write, delete) to the bridge's operational struct.

---

## Next Steps

Now that you understand the architectural strategy, let's implement the core drivers:
* **[File Explorer Bridge Integration](./file_explorer_bridge.md)** — Mapping your native filesystem to the web portal.
* **[Web Plugins Integration](./web_plugins_bridge.md)** — Streaming dynamic user interfaces from your Host MCU.