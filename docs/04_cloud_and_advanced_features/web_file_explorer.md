---
sidebar_position: 5
title: Web File Explorer
description: Guide to browsing, uploading, downloading, and managing Host MCU storage directly from any web browser using the MGW Storage Explorer.
---

import useBaseUrl from '@docusaurus/useBaseUrl';

# Web File Explorer

The **Web File Explorer** (accessible under **File Explorer** in the MGW Admin Center) lets engineers and operators browse, upload, download, and manage files stored on the Host Microcontroller's SD card or flash memory directly from any standard web browser—with **zero desktop software installation required**.

<div style={{ textAlign: 'center', margin: '24px 0' }}>
  <img 
    src={useBaseUrl('/img/web_file_explorer.png')} 
    alt="MGW Web File Explorer Dashboard" 
    style={{ maxWidth: '100%', borderRadius: '10px', boxShadow: '0 8px 24px rgba(0,0,0,0.12)' }} 
  />
  <p style={{ fontSize: '0.85rem', color: '#64748b', marginTop: '8px' }}>
    <em>Figure 1: MGW Storage Explorer browsing Host MCU directories via mDNS</em>
  </p>
</div>

---

## 1. How It Works

The gateway operates as a transparent streaming proxy between your browser and your Host MCU:
* When you open a folder, upload a file, or click download, the gateway forwards the request over **UART1** or **SPI1** to your Host MCU.
* Data streams directly chunk-by-chunk without requiring the gateway to store or buffer entire files locally.

---

## 2. Key User Capabilities

### A. Directory Browsing & Natural Sorting
* **Folder-First Sorting:** Directories are pinned to the top and sorted naturally (for example, `log_2.csv` appears before `log_10.csv`), matching modern desktop file managers.
* **Breadcrumb Navigation:** Click any folder row to open it, click **Up** to return to the parent directory, or click **Refresh** to reload the listing from Host MCU storage.

### B. Resilient Sliced Uploads & Drag-and-Drop
* **Upload Methods:** Click **Upload File** to select one or multiple files, or simply **drag and drop** files directly onto the file table.
* **64 KB Resilient Slicing:** Large files are automatically split into 64 KB chunks with 3-attempt automatic retry on transient wireless drops.
* **Conflict Resolution:** If a file with the same name already exists in the target folder, a prompt lets you choose **Replace** or **Skip** (with an option to apply your choice to all remaining files).

### C. Real-Time Progress & Instant Cancellation
* A live progress bar displays transferred bytes and percentage progress in real time.
* Clicking **Cancel** immediately aborts the in-flight upload and instructs the Host MCU to clean up any partial file fragments so no corrupted data remains on your storage media.

### D. Downloads, Folders & Deletion
* **Download:** Click the blue download button on any file row to stream it directly to your computer.
* **New Folder:** Click **+ New Folder** to create directories on the Host MCU storage.
* **Delete:** Click the trash icon on any file or folder (protected by a confirmation dialog).

---

## 3. Selecting the Bridge Speed (UART1 vs. SPI1)

For small configuration files and logs, the default **UART1** bridge works out-of-the-box. For transferring large firmware binaries, audio assets, or high-volume logs, switch the File Explorer channel to high-speed **SPI1**:

1. In the Web Dashboard sidebar, navigate to **Hardware Config**.
2. Change **File Explorer Data Channel** to **SPI1 (High-Speed Host MCU Bridge)**.
3. Click **Save Settings** (or send `cmd set-fs-bridge-channel -c "SPI1"` via CLI).

---

## 4. Host MCU Prerequisite

To enable the Web File Explorer for your Host Microcontroller, ensure your Host MCU firmware implements the standard filesystem callbacks in the official C SDK.

* 👉 **[File Explorer Bridge Integration Guide](../03_host_mcu_integration/file_explorer_bridge.md)** — Complete C callback reference (`list_dir`, `open_read`, `open_write`, `delete_entry`).
* 👉 **[High-Speed SPI Integration Guide](../02_communication_protocols/spi_high_speed.md)** — Wiring and configuration for maximum file transfer throughput.
* 👉 **[Native FTP Server Guide](./native_ftp_server.md)** — Alternative method for mounting Host MCU storage in Windows File Explorer.