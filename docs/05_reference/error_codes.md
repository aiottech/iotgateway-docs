---
sidebar_position: 2
title: Error Codes Reference
description: Complete lookup table of hexadecimal wire error codes returned by the MGW gateway and their recommended fixes.
---

# Error Codes Reference

Whenever a command fails, the **Mini IoT Gateway (`MGW-IoT-002`)** returns `Result: error` along with an `Error-Code` header formatted in hexadecimal, followed by a descriptive error message in the response body.

### Example Wire Error Frame

```text
Transfer-Mode: normal
Result-Len: 42
Data-Len: 42
Data-Position: 0-41
Duration: 12 ms
CRC32: a1b2c3d4
Result: error
Error-Code: 9005

Invalid Access Token. Login first required.
```

---

## 1. Security & Authentication Errors

| Wire Code (Hex) | Meaning | Recommended Fix |
| :---: | :--- | :--- |
| **`9004`** | **Invalid Password** | Verify the administrator password submitted during `cmd login`. |
| **`9005`** | **Invalid Access Token / Unauthenticated Bus** | Over UART/SPI: Execute `cmd login -u admin -p admin` once on boot.<br/>Over Web/BLE: Attach a valid `--token "<44_char_token>"` flag. |
| **`b02`** | **Username Not Found** | Check the username spelling (default is `"admin"`). |
| **`b03`** | **Wrong Password** | Verify your custom admin password. If forgotten, hold Pin 15 (`IO28`) LOW for 10 seconds to perform a Factory Reset. |

---

## 2. Command Parsing & Execution Errors

| Wire Code (Hex) | Meaning | Recommended Fix |
| :---: | :--- | :--- |
| **`9001`** | **Empty / Null Command** | Ensure your command string is non-empty and properly terminated with `\r\n`. |
| **`9002`** | **Invalid Command Length** | Ensure the command string does not exceed the maximum line limit (1024 characters). |
| **`9003`** | **Invalid Command Syntax** | Commands must start with `cmd` and balance all double quotes (`"`). Escape inner quotes with `\"`. |
| **`9101`** | **Command Not Recognized** | Check the spelling of your command against the **[Command Reference Manual](./command_reference.md)**. |

---

## 3. General & System Errors

| Wire Code (Hex) | Meaning | Recommended Fix |
| :---: | :--- | :--- |
| **`101`** | **Out of Memory** | Requested allocation or plugin size exceeds active limits. Retry or reduce payload size. |
| **`102`** | **Invalid Argument** | One or more command parameters are missing, out of range, or formatted incorrectly. |
| **`103`** | **Invalid State** | The requested subsystem is not in the required state (e.g., invoking SignalR before connecting). |
| **`104`** | **Invalid Size** | Parameter length or file buffer size does not meet requirements (e.g., SoftAP password under 8 chars). |
| **`105`** | **Resource Not Found** | Target file, key, or transaction ID does not exist. |
| **`106`** | **Not Supported** | The requested operation is not supported in the current mode. |
| **`107`** | **Operation Timed Out** | Network connection, Wi-Fi DHCP lease, or flow-control ACK timed out. Verify physical links. |
| **`109`** | **Invalid CRC / Checksum** | Serial or stream data integrity check failed. Inspect wiring and ground connections, then retry. |
| **`a400`** | **Buffer Full** | Transmit or receive ring buffer saturated. Reduce polling rate or enable `--flow ack`. |
| **`a401`** | **Queue Full** | Command worker queue is busy. Wait for the active command to finish before submitting another. |
| **`a700`** | **Hardware Failure** | Underlying physical peripheral error. Verify power supply stability and pin wiring. |
| **`ffffffff`** | **Generic Failure** | General execution failure. Check the response body text for specific diagnostic details. |

---

## 4. Network & HTTP Errors

| Wire Code (Hex) | Meaning | Recommended Fix |
| :---: | :--- | :--- |
| **`9201`** | **Missing URL** | Provide a valid target URL via `--url "..."`. |
| **`9202`** | **Invalid URL Format** | Ensure the URL includes the scheme (`http://` or `https://`) and contains no unescaped spaces. |
| **`9204`** | **HTTP Buffer Overflow** | Response body exceeds Normal Mode limits. Use Stream Protocol V2 (`--stream true`). |
| **`3000`** | **Wi-Fi Link Down** | Gateway is disconnected from the router. Run `cmd restore-wifi` or `cmd set-wifi`. |

---

## Support

If you encounter persistent hardware errors that cannot be resolved through Factory Reset, please contact technical support through [**CONSTH Vietnam**](https://consth.vn/).
