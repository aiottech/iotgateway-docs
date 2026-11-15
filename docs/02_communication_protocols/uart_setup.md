---
sidebar_position: 2
title: UART Serial Setup
description: Guide to configuring physical serial parameters, baud rates, parity, and stop bits for the MGW gateway UART channel.
---

# UART Serial Setup

The **UART (Universal Asynchronous Receiver-Transmitter)** interface serves as the primary, un-killable console and command bridge between your Host Microcontroller and the **Mini IoT Gateway (MGW)**. 

By default, the gateway operates out-of-box on standard asynchronous serial parameters. This guide covers how to inspect, modify, and integrate the UART channel reliably.

---

## 1. Default Serial Parameters

Out-of-box, the MGW serial interface is configured to the following standard industrial defaults:
* **Baud Rate:** `115200`
* **Data Bits:** `8`
* **Parity:** `Disable` (None)
* **Stop Bits:** `1`
* **Flow Control:** Disabled (Standard 3-wire connection: `TX`, `RX`, `GND`)

---

## 2. Configuring UART Parameters

If your Host MCU requires a custom baud rate or framing format, you can reconfigure the gateway's serial parameters using either the **Web Dashboard** or direct **CLI Commands**.

### A. Via the Web Dashboard
1. Open the MGW Web Dashboard (`http://mgw-xxxxxxxx.local/` or `http://192.168.4.1`).
2. Navigate to **Hardware Config** in the sidebar.
3. Under **UART Serial Parameters**, select your desired:
   * **Baud Rate** (9600, 19200, 38400, 57600, 115200, 230400, 921600)
   * **Data Bits** (7 or 8)
   * **Parity** (Disable, Even, Odd)
   * **Stop Bits** (1 or 2)
4. Click **Save Settings**. The gateway applies the new parameters instantly.

### B. Via CLI Command
You can also reconfigure the serial parameters programmatically by sending a command over the serial line:

```text
cmd setting-uart --baud_rate 115200 --data_bits 8 --parity "disable" --stop_bits "1"\r\n
```

To query the currently active physical UART register settings at any time, execute:

```text
cmd get-setting-uart\r\n
```

---

## 3. Host MCU Integration Best Practices

When writing firmware for your Host MCU to communicate with MGW over UART, follow these integration guidelines:

1. **Line Termination (`\r\n`):**
   * All commands sent to MGW **must** be terminated with a Carriage Return and Line Feed (`\r\n`). Commands sent without proper termination will remain buffered in the gateway until a newline arrives.
2. **Non-Blocking Line Reassembly:**
   * Serial data arrives asynchronously byte-by-byte via DMA or hardware interrupts. Host MCU firmware should implement a line-accumulator buffer that collects incoming bytes until it encounters an `\r` or `\n` character before passing the complete command string to the parser.
3. **Using the Official Host C Driver:**
   * To eliminate serial parsing bugs, CRC calculation errors, and buffer overflows, we strongly recommend integrating our zero-allocation official **Host MCU Parser Driver** (`mgw_host_mcu_parser`) on your microcontroller. It handles arbitrary token fragmentation and line noise automatically.

---

## Next Steps

* Learn about high-speed synchronous transfers in the **[High-Speed SPI Integration Guide](./spi_high_speed.md)**.
* Explore system reference commands in the **[Command Reference Manual](../05_reference/command_reference.md)**.
