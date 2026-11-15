---
sidebar_position: 3
title: High-Speed SPI Integration
description: Guide to configuring and operating the high-speed SPI Slave interface with hardware handshake lines on the MGW gateway.
---

# High-Speed SPI Integration

While standard UART provides an easy serial-to-cloud console, high-throughput applications—such as transferring multi-megabyte files, streaming dynamic web application bundles, or pushing real-time sensor telemetry—require higher bandwidth.

The **Mini IoT Gateway (MGW)** supports a high-speed **SPI Slave interface** capable of operating at speeds between **20 MHz and 40 MHz**, delivering a $10\times$ to $40\times$ performance boost over standard serial lines.

---

## 1. Master / Slave Role Architecture

When configuring high-speed communication over SPI, roles are strictly divided:
* **Host Microcontroller (STM32, NXP, Microchip, etc.) = SPI Master**
  * Controls the bus clock (`SCK`), asserts Chip Select (`CS`), and dictates when data is clocked across `MOSI` and `MISO`.
* **MGW Gateway Module = SPI Slave**
  * Listens to the master clock, receives commands on `MOSI`, and responds on `MISO` when requested.

---

## 2. The "SPI + Handshake" Dual-Pin Architecture

Standard SPI is synchronous and master-driven, meaning an SPI slave cannot spontaneously push data to the master. To solve this limitation and enable full bi-directional streaming, the MGW gateway incorporates **two dedicated hardware handshake lines**:

```text
       [ Host MCU (SPI Master) ]                     [ MGW Gateway (SPI Slave) ]
  ┌─────────────────────────────────┐             ┌─────────────────────────────────┐
  │ SPI Master: Chip Select (CS)    │────────►│ SPI CS                          │
  │ SPI Master: MOSI                │────────►│ SPI MOSI                        │
  │ SPI Master: Serial Clock (SCK)  │────────►│ SPI SCK                         │
  │ SPI Master: MISO                │◄────────│ SPI MISO                        │
  │                                 │             │                                 │
  │ External Interrupt (EXTI Input) │◄────────│ WIFI_DATA_READY (Interrupt Out) │
  │ Flow Control (Output / GPIO)    │────────►│ WIFI_HANDSHAKE  (Flow Control)  │
  └─────────────────────────────────┘             └─────────────────────────────────┘
```

### A. WIFI_DATA_READY (MGW → Host MCU Interrupt)
* **Purpose:** Because MGW is an SPI slave, it cannot generate a clock to send data (such as an incoming cloud message or file download chunk). 
* **Operation:** When MGW has data waiting in its transmit buffer, it asserts **`WIFI_DATA_READY`** HIGH. The Host MCU captures this on an external interrupt (EXTI) pin, knows data is waiting, asserts `CS` LOW, and generates the SPI clock (`SCK`) to clock the data out.

### B. WIFI_HANDSHAKE (Host MCU → MGW Flow Control)
* **Purpose:** Prevents buffer overflows when transferring large bursts of data at high speeds.
* **Operation:** Before sending heavy data payloads, the Host MCU checks the **`WIFI_HANDSHAKE`** line. If the gateway's internal memory buffers are temporarily congested, MGW holds this line busy, instructing the Host MCU to pause clocking until buffers clear.

---

## 3. Data Transfer Sequence Over SPI

### Ingress (Host MCU → MGW)
1. Host MCU checks that `WIFI_HANDSHAKE` indicates the gateway is ready.
2. Host MCU asserts `CS` LOW.
3. Host MCU clocks command strings or file upload chunks across `MOSI`.
4. MGW receives the data via DMA and processes it instantly through its command parser.

### Egress (MGW → Host MCU)
1. MGW loads response chunks into its SPI transmit descriptor.
2. MGW asserts **`WIFI_DATA_READY`** HIGH.
3. Host MCU detects the interrupt, asserts `CS` LOW, and generates `SCK` clock pulses.
4. MGW shifts the data out across `MISO`.
5. Upon transfer completion, MGW drops `WIFI_DATA_READY` back to LOW.

---

## 4. Host MCU Integration Best Practices

1. **Configure as SPI Master (Mode 0):**
   * Ensure your microcontroller's SPI peripheral is initialized as **SPI Master, Mode 0** (`CPOL = 0`, `CPHA = 0`), MSB first.
2. **Handle the DATA_READY Interrupt:**
   * Attach `WIFI_DATA_READY` to an external interrupt pin (rising edge or active-high level trigger) on your Host MCU to wake your SPI polling or DMA read routine.
3. **Channel Selection:**
   * To direct File Explorer and Web Plugin streaming traffic over the high-speed SPI bus rather than UART, ensure the gateway's bridge channel is configured to **`SPI1`** via the Web Dashboard or by executing:
     ```text
     cmd set-fs-bridge-channel -c "SPI1"\r\n
     ```

---
