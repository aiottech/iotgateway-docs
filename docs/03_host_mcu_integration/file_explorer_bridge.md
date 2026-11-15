---
sidebar_position: 2
title: File Explorer Bridge Integration
description: Learn how to map your Host MCU native filesystem (FatFs/LittleFS) to the MGW File Explorer bridge callbacks.
---

# File Explorer Bridge Integration

The `mgw_host_file_bridge` driver allows you to expose your Host MCU's local storage (SD Card, SPI Flash) to the gateway's Web Dashboard. By implementing the `mgw_host_file_ops_t` contract, you enable full remote management: upload, download, rename, and directory listing.

---

## 1. The Integration Contract

To enable the bridge, you must define the `mgw_host_file_ops_t` structure. This structure tells the bridge which native functions (like `f_open`, `f_read`) to call when the gateway requests a file operation.

```c
typedef struct {
    // 1. Directory Listing
    bool (*list_dir)(const char* path, mgw_emit_file_entry_fn emit_entry, void* user_ctx);
    
    // 2. Read/Write Operations (with offset seeking for resilience)
    bool (*open_read)(const char* path, size_t offset, size_t* out_file_size);
    size_t (*read_chunk)(uint8_t* dest, size_t max_len);
    void (*close_read)(void);

    bool (*open_write)(const char* path, size_t offset);
    bool (*write_chunk)(const uint8_t* data, size_t len);
    void (*close_write)(void);

    // 3. Maintenance
    bool (*delete_entry)(const char* path);
    bool (*make_dir)(const char* path);

    // 4. Hardware Transport (Bind your UART/SPI Send)
    void (*tx_bytes)(const uint8_t* data, size_t len, void* user_ctx);
    bool (*wait_ack)(uint32_t timeout_ms, void* user_ctx);
    
    void* user_ctx;
} mgw_host_file_ops_t;
```

---

## 2. Implementation Checklist

When mapping your filesystem, follow these critical requirements:

1. **Offset-Aware Seeking:** Your `open_read` and `open_write` functions **must** support the `offset` parameter. If a file transfer is interrupted (Wi-Fi drop), the gateway will request a resume at the last successful byte offset.
2. **Chunked Transfers:** Implement `read_chunk` and `write_chunk` to handle small buffers (typically 512 bytes to 4 KB). Do not attempt to load the entire file into memory.
3. **1-Byte ACK Flow Control:** Your `wait_ack` callback must block until the Host MCU receives the `0x06` ACK byte from the gateway. This prevents the gateway from overflowing the Host MCU's serial receive buffers.
4. **Cleanup:** Always ensure your `close_read` and `close_write` callbacks flush and release file handles, even if a transfer is aborted mid-stream.

---

## 3. Registering the Bridge

Initialize the bridge once during your Host MCU's startup sequence:

```c
#include "mgw_host_file_bridge.h"

void app_init(void) {
    mgw_host_file_ops_t file_ops = {
        .list_dir     = my_fatfs_list_dir,
        .open_read    = my_fatfs_open_read,
        .read_chunk   = my_fatfs_read_chunk,
        .close_read   = my_fatfs_close_read,
        .open_write   = my_fatfs_open_write,
        .write_chunk  = my_fatfs_write_chunk,
        .close_write  = my_fatfs_close_write,
        .delete_entry = my_fatfs_delete,
        .make_dir     = my_fatfs_mkdir,
        .tx_bytes     = my_host_uart_tx,
        .wait_ack     = my_host_wait_ack,
        .user_ctx     = NULL
    };
    
    mgw_host_file_bridge_init(&file_ops);
}
```

---

## Next Steps

* **[Web Plugins Integration](./web_plugins_bridge.md)** — Learn how to stream your own HTML control panels into the MGW dashboard!
