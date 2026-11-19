---
sidebar_position: 4
title: Native FTP Server
description: Guide to enabling and connecting to the built-in RFC 959 FTP server on the MGW gateway to manage Host MCU storage wirelessly.
---

# Native FTP Server

In addition to the browser-based File Explorer, the **Mini IoT Gateway (`MGW-IoT-002`)** includes a native **RFC 959 Passive-Mode FTP Server**. 

This allows field technicians, engineers, and automated CI/CD scripts to mount the Host MCU's storage media wirelessly over Wi-Fi using standard desktop FTP clients—such as **Windows File Explorer**, **FileZilla**, **WinSCP**, or **Total Commander**—and drag-and-drop files effortlessly.

---

## 1. Transparent Storage Bridging

Just like the Web File Explorer, the FTP server operates as a transparent proxy tunnel:
* Your Host Microcontroller does **not** need any TCP/IP sockets or FTP protocol code.
* If your Host MCU already implements the standard `mgw_host_file_bridge` callbacks, the Native FTP Server works out-of-the-box over either **UART1** or **SPI1**.

---

## 2. Controlling the FTP Server

Because the FTP server is protected by system security policies, ensure your session is authenticated before issuing commands.

### Start the FTP Server
To launch the FTP daemon on the default TCP port (`21`):

```text
cmd ftp-start\r\n
```

To listen on a custom TCP port (for example, `2121`):

```text
cmd ftp-start --port 2121\r\n
```

### Query Server Status
To check whether the FTP server is currently active:

```text
cmd ftp-status\r\n
```
* Returns **`RUNNING`** or **`STOPPED`**.

### Stop the FTP Server
To terminate all active FTP sessions and shut down the listening socket:

```text
cmd ftp-stop\r\n
```

---

## 3. Connecting from Your PC

Once the FTP server is running and your PC is on the same Wi-Fi network (or connected to the gateway's SoftAP):

### Option A: Using Windows File Explorer
1. Open **Windows File Explorer** (`Win + E`).
2. Click the address bar at the top and enter the gateway's IP or mDNS hostname:
   * `ftp://192.168.4.1` *(when connected via SoftAP)*
   * `ftp://mgw-xxxxxxxx.local` *(or the gateway's DHCP IP address on your router)*
3. When prompted for credentials, enter your gateway administrator username and password (default: **`admin`** / **`admin`**).
4. The Host MCU storage opens as a native folder where you can copy, paste, and delete files.

### Option B: Using FileZilla or WinSCP
* **Protocol:** FTP - File Transfer Protocol (Plain FTP over LAN/SoftAP)
* **Host:** Gateway IP address (or `mgw-xxxxxxxx.local`)
* **Port:** `21` (or your custom `--port` value)
* **Transfer Mode:** **Passive (`PASV`)** *(Supported automatically for firewall compatibility)*
* **Logon Type:** Normal (with your gateway admin username and password; anonymous logins are rejected for security)

---

## 4. Supported Operations

| Action | Desktop Client Operation | Host MCU Bridge Action |
| :--- | :--- | :--- |
| **Browse Directories** | Open folder / Refresh | Calls `list_dir` and translates entries cleanly |
| **Download Files** | Drag file from gateway to PC | Streams binary chunks via `open_read` / `read_chunk` |
| **Upload Files** | Drag file from PC to gateway | Streams binary chunks via `open_write` / `write_chunk` |
| **Delete Files** | Press Delete on a file | Calls `delete_entry` on Host MCU storage |
| **Create Folders** | Right-click → New Folder | Calls `make_dir` on Host MCU storage |

---

## Next Steps

* Check out the **[Web File Explorer Guide](./web_file_explorer.md)** for zero-client browser-based file management.
* Proceed to Chapter 5 to explore the complete **[CLI & AST Command Reference Manual](../05_reference/command_reference.md)**.
