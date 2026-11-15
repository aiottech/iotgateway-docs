---
sidebar_position: 2
title: SignalR Cloud Integration
description: Guide to connecting your Host MCU to ASP.NET Core SignalR Hubs via the MGW transparent bi-directional cloud bridge.
---

# SignalR Cloud Integration

Modern enterprise cloud platforms—especially **ASP.NET Core** backends—rely heavily on **Microsoft SignalR** for real-time bi-directional messaging, live dashboards, and Remote Procedure Calls (RPC).

Low-resource Host Microcontrollers often lack the RAM and TLS cryptographic stacks required to maintain persistent SignalR WebSocket connections directly. The **Mini IoT Gateway (`MGW-IoT-002`)** acts as a transparent IoT bridge, managing the SignalR connection, TLS security, and keep-alives automatically while forwarding messages to and from your Host MCU.

---

## 1. Bidirectional Data Flow

```text
  Direction A (Outbound RPC): Host MCU ──► MGW ──► Cloud SignalR Hub
  [ Host MCU ] ──(cmd signalr-invoke)──► [ MGW Gateway ] ──(WSS JSON Frame)──► [ SignalR Hub ]

  Direction B (Inbound Push): Cloud SignalR Hub ──► MGW ──► Host MCU
  [ SignalR Hub ] ──(WSS Server Push)──► [ MGW Gateway ] ──(Stream V2 Metadata)──► [ Host MCU ]
```

---

## 2. Connecting to a SignalR Hub (`signalr-start`)

Before starting a SignalR session, ensure the gateway is connected to a Wi-Fi network with internet or LAN access to your server (e.g., via `cmd set-wifi` or `cmd restore-wifi`).

To connect and automatically perform the SignalR JSON v1 negotiation handshake, send:

```text
cmd signalr-start --url "wss://api.yourdomain.com/chathub" --token "eyJhbGciOi..."\r\n
```

### Parameters
* **`--url` / `-u` (Required):** WebSocket URI of the target SignalR Hub (`ws://` or `wss://`).
* **`--token` / `-t` (Optional):** JWT Bearer Access Token if your hub requires authentication.

---

## 3. Invoking Cloud RPC Methods (`signalr-invoke`)

Once connected, your Host MCU can invoke remote methods on the cloud server at any time:

```text
cmd signalr-invoke --method "sendToAll" --args "[\"MGW-Node-01\",\"Machine Online\"]"\r\n
```

### Parameters
* **`--method` / `-m` (Required):** Name of the target hub method to call.
* **`--args` / `-a` (Optional):** A valid JSON array containing the arguments to pass to the server method.

:::warning Important JSON Escaping Rule
Because the `--args` parameter is wrapped in outer double quotes on the command line, **you must escape all inner double quotes inside the JSON array using a backslash (`\"`)**:
* **Incorrect:** `--args "["User","Hello"]"`
* **Correct:** `--args "[\"User\",\"Hello\"]"`
:::

---

## 4. Receiving Unsolicited Cloud Pushes on Your Host MCU

Whenever the cloud server invokes a method or broadcasts a message down to the gateway, MGW automatically packages the message into a **Stream Protocol V2** frame enriched with routing metadata headers:

```text
Transfer-Mode: stream\r\n
Result: success\r\n
Data-Type: SignalR\r\n
Hub-Name: chathub\r\n
Hub-Method: sendToAll\r\n
\r\n
1F\r\n
["CloudAdmin","Start Cycle 1"]\r\n
0\r\n
Result: success\r\n
CRC32: 4a17b156\r\n
\r\n
```

### Handling Pushes with the Official Host C Driver
Using the official **`mgw_host_mcu_parser`**, you can intercept incoming SignalR messages cleanly using the `on_stream_chunk_ex` callback:

```c
#include "mgw_host_mcu_parser.h"
#include <string.h>
#include <stdio.h>

void my_on_stream_chunk_ex(const char* data_type,
                           const char* hub_name,
                           const char* hub_method,
                           const uint8_t* chunk_data,
                           size_t chunk_len,
                           void* user_ctx) {
    (void)user_ctx;

    if (strcmp(data_type, "SignalR") == 0) {
        printf("Cloud Push from Hub '%s' -> Method '%s': %.*s\n",
               hub_name, hub_method, (int)chunk_len, (const char*)chunk_data);
    }
}
```

---

## 5. Checking Status & Disconnecting

### Query Connection Status
To verify whether the gateway is currently connected and handshaked with the SignalR Hub:

```text
cmd signalr-status\r\n
```
* Returns **`OK`** if connected and active, or **`NG`** if disconnected.

### Terminate Connection
To gracefully close the SignalR WebSocket session:

```text
cmd signalr-stop\r\n
```

---

## Next Steps

* Learn how to keep your gateway firmware up to date remotely in the **OTA Firmware Updates Guide**.
