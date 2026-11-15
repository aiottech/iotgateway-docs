---
sidebar_position: 1
title: Data Transfer Modes
description: Comprehensive wire protocol guide covering Normal Mode and Stream Protocol V2 for the MGW gateway.
---

# Data Transfer Modes

When communicating with the **Mini IoT Gateway (MGW)** over serial lines (UART or high-speed SPI), transactions are handled through two distinct data transfer modes: **Normal Mode** and **Stream Protocol V2**.

Choosing the correct mode ensures optimal memory efficiency, reliable error detection, and smooth handling of everything from simple status queries to multi-megabyte file transfers.

---

## 1. Normal Mode (Atomic Single-Block Frames)

**Normal Mode** is designed for fast, short, atomic commands where the entire response can be packaged and transmitted in a single, self-contained frame.

### Typical Use Cases:
* Querying system status (`cmd wifi-status`, `cmd check-license`)
* Retrieving configuration values (`cmd get-time`, `cmd get-mac`)
* Executing diagnostic echo tests (`cmd echo`)

### Wire Format Structure
A Normal Mode response begins with standardized key-value header lines terminated by `\r\n`, followed by a blank line (`\r\n\r\n`) and the raw payload:

```text
Transfer-Mode: normal\r\n
Result-Len: <Total_Bytes>\r\n
Data-Len: <Payload_Bytes>\r\n
Data-Position: <Start>-<End>\r\n
Duration: <Elapsed_ms> ms\r\n
CRC32: <Payload_CRC32_Hex>\r\n
Result: <success | error>\r\n
\r\n
<Raw_Binary_or_Text_Payload>
```

---

## 2. Stream Protocol V2 (HTTP-Style Chunked Transfer)

**Stream Protocol V2** is engineered for high-volume, continuous, or large-scale data transfers. Rather than attempting to buffer entire files or large responses in memory, Stream Protocol V2 breaks data into sequential, manageable slices.

### Typical Use Cases:
* Downloading files from the File Explorer (`cmd fs-read`)
* Streaming dynamic Web Plugin HTML bundles (`cmd register-web-plugin`)
* Pushing real-time sensor telemetry or cloud messages

### Wire Format Structure
Stream Protocol V2 utilizes an HTTP-style chunked transfer encoding (`<hex_len>\r\n<payload>\r\n`), wrapped between an initial header frame and a concluding summary footer:

```text
1. INITIAL HEADER FRAME (Sent ONCE at stream start):
   Transfer-Mode: stream\r\n
   Result: success\r\n
   [Optional Metadata Headers: Data-Type, Command-Type, File-Action, File-Path]\r\n
   \r\n

2. CONTINUOUS CHUNK FRAMES (Streamed sequentially):
   <Hex_Chunk_Length_1>\r\n
   <Raw_Payload_Slice_1>\r\n
   <Hex_Chunk_Length_2>\r\n
   <Raw_Payload_Slice_2>\r\n
   ...

3. STREAM SUMMARY FOOTER (Sent ONCE upon completion):
   0\r\n
   Result: success\r\n
   Count-Data: <Total_Streamed_Bytes>\r\n
   Duration: <Elapsed_ms> ms\r\n
   CRC32: <Cumulative_Payload_CRC32_Hex>\r\n
   \r\n
```

---

## 3. Data Integrity & CRC32 Checksum Verification

In industrial automation environments, electrical noise and long cable runs can occasionally induce bit flips on serial lines. 

* **Guaranteed Accuracy:** Every transaction (both Normal Mode and Stream Protocol V2) calculates a strict **IEEE 802.3 CRC32 checksum** of the payload data.
* **Validation:** The checksum is transmitted in the response headers. The receiving system can instantly compare its locally calculated CRC32 against the header value to verify 100% bitwise data integrity before accepting or storing the payload.

---

## 4. Software Flow Control (1-Byte ACK Handshake)

When streaming large amounts of data to or from slow storage media (such as physical SD cards or flash memory), fast transmitters can overwhelm slow receivers, causing buffer overflows.

To prevent this, MGW supports optional software flow control (`--flow ack`):
1. **The Pause Mechanism:** The sender transmits a data chunk and automatically pauses.
2. **The Acknowledge Byte:** Once the receiver successfully writes the chunk to storage, it transmits a single **`0x06` (ASCII ACK)** byte back across the wire.
3. **Resumption:** Upon receiving the `0x06` ACK, the sender releases its lock and transmits the next chunk.

---

## Next Steps

* Learn how to configure serial port parameters in the **[UART Setup Guide](./uart_setup.md)**.
* Read about hardware-accelerated streaming in the **[High-Speed SPI Integration Guide](./spi_high_speed.md)**.
