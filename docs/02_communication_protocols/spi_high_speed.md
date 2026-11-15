---
sidebar_position: 3
title: High-Speed SPI Integration
description: Detailed physical pin mappings, connector specifications, and electrical guidelines for wiring your Host MCU to the MGW-IoT-002 gateway.
---

# High-Speed SPI Integration

While standard UART provides an easy serial-to-cloud console, high-throughput applications—such as transferring multi-megabyte files, streaming dynamic web application bundles, or pushing real-time sensor telemetry—require higher bandwidth.

The **Mini IoT Gateway (MGW)** supports a high-speed **SPI Slave interface** capable of operating at speeds between **20 MHz and 40 MHz**, delivering a 10x to 40x performance boost over standard serial lines.

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
       [ Host MCU (SPI Master) ]                 [ MGW Gateway (SPI Slave) ]
  ┌─────────────────────────────────┐         ┌─────────────────────────────────┐
  │ SPI Master: Chip Select (CS)    │────────►│ Pin 12 (IO10) : SPI_CS          │
  │ SPI Master: MOSI                │────────►│ Pin 9  (IO7)  : SPI_MOSI        │
  │ SPI Master: Serial Clock (SCK)  │────────►│ Pin 8  (IO6)  : SPI_SCK         │
  │ SPI Master: MISO                │◄────────│ Pin 4  (IO2)  : SPI_MISO        │
  │                                 │         │                                 │
  │ External Interrupt (Input)      │◄────────│ Pin 5  (IO3)  : DATA_READY      │
  │ Flow Control (Output)           │────────►│ Pin 17 (IO4)  : HANDSHAKE       │
  └─────────────────────────────────┘         └─────────────────────────────────┘
```

### A. DATA_READY (MGW → Host MCU)
Because MGW is an SPI slave, it cannot generate a clock to send data (e.g., cloud-to-Host MCU messages). When MGW has data waiting in its transmit buffer, it asserts **`DATA_READY`** (IO3) HIGH. The Host MCU captures this as an external interrupt, asserts `CS` LOW, and generates the SPI clock to pull the data out.

### B. HANDSHAKE (Host MCU → MGW)
Acts as hardware flow control. If MGW's internal buffers are congested, it monitors this pin. The Host MCU checks this status before sending massive file chunks to ensure the MGW gateway has sufficient head-room, preventing buffer overflows.

---

## 3. Host MCU Integration Best Practices

1. **Configure as SPI Master (Mode 0):** Ensure your microcontroller's SPI peripheral is initialized as **SPI Master, Mode 0** (`CPOL = 0`, `CPHA = 0`), MSB first.
2. **Handle the DATA_READY Interrupt:** Attach the `DATA_READY` signal to an external interrupt pin (rising edge) on your Host MCU to trigger your SPI DMA read routine.
3. **Channel Selection:** Set the gateway transport to **SPI1** via the Web Dashboard or the CLI command:
   ```text
   cmd set-fs-bridge-channel -c "SPI1"
   ```

---

## Next Steps

* Proceed to the **[Communication Protocols Overview](./data_transfer_modes.md)** to learn how data is framed and transmitted.
