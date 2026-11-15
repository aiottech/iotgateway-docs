/**
 * @file mgw_host_file_bridge.h
 * @brief Official Companion File Explorer Bridge Driver for Host MCUs (STM32, NXP, PIC, AVR, etc.).
 * 
 * Maps Web File Explorer actions directly to Host MCU FatFS / LittleFS function pointers.
 * 100% Portable ANSI C: Zero framework dependencies, zero OS headers, zero dynamic memory allocations.
 */

#ifndef MGW_HOST_FILE_BRIDGE_H
#define MGW_HOST_FILE_BRIDGE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "mgw_host_mcu_parser.h"

typedef struct {
    const char* name;
    bool        is_dir;
    size_t      size_bytes;
} mgw_file_entry_t;

// Callback: Host MCU emits one directory entry back to the bridge
typedef void (*mgw_emit_file_entry_fn)(const mgw_file_entry_t* entry, void* user_ctx);

// File Operations Contract implemented by the Host MCU developer
typedef struct {
    // 1. Directory Listing (e.g. calls f_opendir / f_readdir)
    bool (*list_dir)(const char* path, mgw_emit_file_entry_fn emit_entry, void* user_ctx);

    // 2. Open file for reading (supports offset for resumable download)
    bool (*open_read)(const char* path, size_t offset, size_t* out_file_size);

    // 3. Read next file chunk (e.g. f_read)
    size_t (*read_chunk)(uint8_t* dest, size_t max_len);

    // 4. Close active read file (e.g. f_close)
    void (*close_read)(void);

    // 5. Open file for writing (supports offset for resumable upload / retry)
    bool (*open_write)(const char* path, size_t offset);

    // 6. Write chunk of uploaded file (e.g. f_write)
    bool (*write_chunk)(const uint8_t* data, size_t len);

    // 7. Close active written file (e.g. f_close)
    void (*close_write)(void);

    // 8. Delete file or directory (e.g. f_unlink)
    bool (*delete_entry)(const char* path);

    // 9. Create directory (e.g. f_mkdir)
    bool (*make_dir)(const char* path);

    // 10. Hardware UART/SPI TX function pointer to emit bytes back to MGW
    void (*tx_bytes)(const uint8_t* data, size_t len, void* user_ctx);

    // 11. Flow Control: Wait for 0x06 ACK byte from MGW before sending next chunk
    bool (*wait_ack)(uint32_t timeout_ms, void* user_ctx);

    // 12. Optional Concurrency Hook: Defer heavy read_file operation to a background task
    bool (*defer_read_file)(const char* file_path, size_t offset, void* user_ctx);

    void* user_ctx;
} mgw_host_file_ops_t;

// --- HOST MCU WEB PLUGIN CONTRACT ---
typedef struct {
    // 1. Stream the requested plugin HTML bundle (e.g. read from local SD card)
    void (*stream_plugin_content)(const char* plugin_name, void* user_ctx);
    
    // 2. Handle an event sent by the user clicking a button on the browser plugin
    void (*on_plugin_event)(const char* plugin_name, const uint8_t* json_payload, size_t payload_len, void* user_ctx);

    void* user_ctx;
} mgw_host_plugin_ops_t;

void mgw_host_plugin_bridge_init(const mgw_host_plugin_ops_t* ops);

/**
 * @brief Initializes the Host File Bridge with the user's filesystem operations.
 * @param ops Pointer to populated operations structure.
 */
void mgw_host_file_bridge_init(const mgw_host_file_ops_t* ops);

/**
 * @brief Stream command callback to be passed directly to mgw_parser_config_t.on_stream_command.
 */
void mgw_host_file_bridge_on_command(const char* command_type,
                                     const char* file_action,
                                     const char* file_path,
                                     size_t file_offset,
                                     const uint8_t* chunk_data,
                                     size_t chunk_len,
                                     void* user_ctx);

/**
 * @brief Stream finish callback to be called from mgw_parser_config_t.on_stream_finish.
 */
void mgw_host_file_bridge_on_finish(size_t total_bytes, bool crc_matched, void* user_ctx);

/**
 * @brief Standalone file reading execution loop.
 * @param file_path File path to stream.
 * @param offset Starting byte offset to seek.
 */
void mgw_host_file_bridge_execute_read(const char* file_path, size_t offset);

#endif // MGW_HOST_FILE_BRIDGE_H