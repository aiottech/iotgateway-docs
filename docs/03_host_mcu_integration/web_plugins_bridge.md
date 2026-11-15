---
sidebar_position: 3
title: Dynamic Web Plugins
description: Learn how to inject custom HTML5/CSS/JS micro-frontends from your Host MCU directly into the MGW Web Dashboard.
---

# Dynamic Web Plugins (Micro-Frontends)

The **Dynamic Web Plugin Subsystem** enables your Host Microcontroller to inject custom, machine-specific web interfaces (such as motor control panels, live oscilloscope charts, or sensor calibration gauges) directly into the MGW Web Dashboard sidebar.

Your Host MCU stores the UI as a single self-contained HTML file (HTML + CSS + JS) and streams it to the browser on demand via the gateway—requiring **zero changes to the gateway firmware**.

---

## 1. Architecture Overview

```text
  [ Host MCU Boot ] ──(1. cmd register-web-plugin)──► [ MGW Gateway Registry ]
                                                              │
                                                              ▼ (Live Sidebar Update)
  [ Browser User ] ──(2. Clicks App in Sidebar)──────► [ MGW Web Dashboard ]
                                                              │
                                                              ▼ (Stream V2: get-content)
  [ Host MCU ] ◄──────────────────────────────────────────────┘
       │
       └──(Streams HTML Bundle)──► [ Sandboxed IFrame Viewport ]
```

---

## 2. Step 1: Registering Your Plugin on Boot

When your Host MCU boots and establishes serial communication with MGW, announce your plugin using a single CLI command over UART or SPI:

```text
cmd register-web-plugin -n "motor_ctrl" -t "Motor Controller" -i "motor" --size 32768\r\n
```

### Command Parameters
* **`-n` / `--name`:** Unique plugin identifier (lowercase alphanumeric, no spaces, e.g., `motor_ctrl`).
* **`-t` / `--title`:** Friendly display name shown in the Web Dashboard sidebar.
* **`-i` / `--icon`:** Sidebar icon hint (`gauge`, `motor`, `chart`, `power`, `cpu`, `sensor`, `tool`).
* **`--size`:** *(Optional)* Expected HTML bundle size in bytes.

Once sent, the new menu item appears instantly in any open Web Dashboard without requiring a page reload.

To remove a plugin dynamically, send:
```text
cmd unregister-web-plugin -n "motor_ctrl"\r\n
```

---

## 3. Step 2: Implementing the Host MCU Bridge

In your Host MCU C firmware, bind your plugin handlers to the official `mgw_host_plugin_ops_t` contract inside `mgw_host_file_bridge.h`:

```c
#include "mgw_host_file_bridge.h"

static void my_stream_plugin_content(const char* plugin_name, void* user_ctx) {
    // Open the HTML bundle (e.g., from Host MCU SD Card or Flash) and stream it using Stream V2
}

static void my_on_plugin_event(const char* plugin_name, const uint8_t* json_payload, size_t payload_len, void* user_ctx) {
    // Handle button clicks or slider adjustments sent by the browser user
}

void app_init(void) {
    mgw_host_plugin_ops_t plugin_ops = {
        .stream_plugin_content = my_stream_plugin_content,
        .on_plugin_event       = my_on_plugin_event,
        .user_ctx              = NULL
    };
    mgw_host_plugin_bridge_init(&plugin_ops);
}
```

---

## 4. Step 3: Streaming the HTML Bundle (`stream_plugin_content`)

When a user clicks your plugin in the Web Dashboard, MGW sends a `WebPlugin` command with action `get-content` down to your Host MCU. Your `stream_plugin_content` callback should stream the HTML file back using Stream Protocol V2 headers:

```c
static void my_stream_plugin_content(const char* plugin_name, void* user_ctx) {
    // 1. Transmit Initial Stream Header
    char hdr[160];
    int hlen = snprintf(hdr, sizeof(hdr),
        "Transfer-Mode: stream\r\nResult: success\r\nData-Type: PluginContent\r\nPlugin-Name: %s\r\n\r\n",
        plugin_name);
    my_serial_tx((const uint8_t*)hdr, hlen);

    // 2. Stream HTML content in chunks (<hex_len>\r\n<chunk>\r\n)
    // ... read from SD card or flash .rodata array ...

    // 3. Transmit Summary Footer
    char ftr[128];
    int flen = snprintf(ftr, sizeof(ftr),
        "0\r\nResult: success\r\nCount-Data: %u\r\nCRC32: %08X\r\n\r\n",
        (unsigned int)total_size, (unsigned int)crc);
    my_serial_tx((const uint8_t*)ftr, flen);
}
```

---

## 5. Step 4: Bidirectional Browser Events & Live Telemetry

Your HTML plugin runs inside a sandboxed viewport and communicates securely with your Host MCU using standard `postMessage` JSON packets.

### A. Sending User Actions (Browser → Host MCU)
Inside your HTML plugin's JavaScript, trigger an action when the user clicks a button:

```javascript
function startMotor(targetRpm) {
    window.parent.postMessage({
        type: 'MGW_PLUGIN_EVENT',
        plugin: 'motor_ctrl',
        action: 'start_motor',
        rpm: targetRpm
    }, '*');
}
```
MGW forwards this JSON payload directly into your Host MCU's `on_plugin_event()` C callback!

### B. Pushing Real-Time Telemetry (Host MCU → Browser)
To update live gauges or oscilloscope charts at high frame rates, your Host MCU can push unsolicited Stream V2 telemetry packets at any time:

```text
Transfer-Mode: stream\r\n
Result: success\r\n
Data-Type: PluginTelemetry\r\n
Plugin-Name: motor_ctrl\r\n
\r\n
24\r\n
{"temp":42.5,"rpm":1500,"running":1}\r\n
0\r\n
Result: success\r\n
CRC32: 00000000\r\n
\r\n
```

Inside your HTML plugin's JavaScript, listen for incoming telemetry:

```javascript
window.addEventListener('message', (event) => {
    if (event.data && event.data.type === 'MGW_HOST_TELEMETRY') {
        const telemetry = event.data.payload;
        document.getElementById('temp-lbl').innerText = telemetry.temp + ' °C';
    }
});
```

---

## 6. Optimizing Bandwidth with Visibility Queries

To avoid wasting serial bandwidth or CPU cycles pushing telemetry when nobody is looking at the screen, your Host MCU can query the user's current view state at any time:

| CLI Command | Possible Return Values | Purpose |
| :--- | :--- | :--- |
| `cmd is-plugin-visible -n "motor_ctrl"` | `true` / `false` | Check if a user currently has your plugin open on their screen |
| `cmd get-active-view` | `plugin:<name>`, `settings`, `none` | Identify which menu tab is currently visible |
| `cmd get-dashboard-status` | `ONLINE` / `OFFLINE` | Verify if any browser is actively connected to the dashboard |

:::tip Best Practice
Before starting a high-frequency 60 FPS telemetry loop on your Host MCU, query `cmd is-plugin-visible`. Pause telemetry whenever the gateway returns `false`!
:::

---

## Next Steps

* Proceed to Chapter 4 to learn about **Security & Dual-Zone Authentication** and **SignalR Cloud Integration**.
