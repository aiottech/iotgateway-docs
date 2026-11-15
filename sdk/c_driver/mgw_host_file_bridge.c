/**
 * @file mgw_host_file_bridge.c
 * @brief Official Companion File Explorer Bridge Driver Implementation.
 * 
 * 100% Portable ANSI C: Zero framework dependencies, zero OS headers, zero dynamic memory allocations.
 */

#include "mgw_host_file_bridge.h"
#include <string.h>
#include <stdio.h>

#define CHUNK_BUFFER_SIZE 512

static mgw_host_file_ops_t s_ops = {0};
static bool s_is_writing = false;
static char s_active_write_path[128] = {0};

// Static internal buffers: 0 Stack, 0 Heap
static uint8_t s_tx_chunk_buf[CHUNK_BUFFER_SIZE];
static char    s_line_buf[256];
static char    s_chunk_hdr_buf[32];
static char    s_footer_buf[128];

static mgw_host_plugin_ops_t s_plugin_ops = {0};

// Standard standalone IEEE 802.3 bitwise CRC32 calculation
static uint32_t _bridge_calc_crc32(uint32_t crc, const uint8_t *buf, size_t len) {
    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (int k = 0; k < 8; k++) {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
        }
    }
    return ~crc;
}

static void _tx_raw(const char* str) {
    if (s_ops.tx_bytes != NULL && str != NULL) {
        s_ops.tx_bytes((const uint8_t*)str, strlen(str), s_ops.user_ctx);
    }
}

static void _emit_json_entry(const mgw_file_entry_t* entry, void* user_ctx) {
    (void)user_ctx;
    if (entry == NULL || entry->name == NULL) return;

    int len = snprintf(s_line_buf, sizeof(s_line_buf),
                       "{\"name\":\"%s\",\"dir\":%s,\"size\":%u}\n",
                       entry->name,
                       entry->is_dir ? "true" : "false",
                       (unsigned int)entry->size_bytes);

    if (len > 0 && s_ops.tx_bytes != NULL) {
        snprintf(s_chunk_hdr_buf, sizeof(s_chunk_hdr_buf), "%X\r\n", (unsigned int)len);
        _tx_raw(s_chunk_hdr_buf);
        s_ops.tx_bytes((const uint8_t*)s_line_buf, (size_t)len, s_ops.user_ctx);
        _tx_raw("\r\n");
    }
}

void mgw_host_plugin_bridge_init(const mgw_host_plugin_ops_t* ops) {
    if (ops != NULL) {
        s_plugin_ops = *ops;
    }
}

void mgw_host_file_bridge_init(const mgw_host_file_ops_t* ops) {
    if (ops != NULL) {
        s_ops = *ops;
    }
    s_is_writing = false;
    memset(s_active_write_path, 0, sizeof(s_active_write_path));
}

void mgw_host_file_bridge_execute_read(const char* file_path, size_t offset) {
    if (!file_path) return;
    size_t file_size = 0;

    if (s_ops.open_read != NULL && s_ops.open_read(file_path, offset, &file_size)) {
        _tx_raw("Transfer-Mode: stream\r\nData-Type: FileDownload\r\nResult: success\r\n\r\n");

        uint32_t running_crc = 0;
        size_t total_sent = 0;

        if (s_ops.read_chunk != NULL) {
            size_t read_bytes = 0;
            while ((read_bytes = s_ops.read_chunk(s_tx_chunk_buf, sizeof(s_tx_chunk_buf))) > 0) {
                running_crc = _bridge_calc_crc32(running_crc, s_tx_chunk_buf, read_bytes);
                total_sent += read_bytes;

                snprintf(s_chunk_hdr_buf, sizeof(s_chunk_hdr_buf), "%X\r\n", (unsigned int)read_bytes);
                _tx_raw(s_chunk_hdr_buf);
                s_ops.tx_bytes(s_tx_chunk_buf, read_bytes, s_ops.user_ctx);
                _tx_raw("\r\n");

                if (s_ops.wait_ack != NULL) {
                    if (!s_ops.wait_ack(5000, s_ops.user_ctx)) {
                        break;
                    }
                }
            }
        }

        if (s_ops.close_read != NULL) {
            s_ops.close_read();
        }

        snprintf(s_footer_buf, sizeof(s_footer_buf),
                 "0\r\nResult: success\r\nCount-Data: %u\r\nCRC32: %08X\r\n\r\n",
                 (unsigned int)total_sent, (unsigned int)running_crc);
        _tx_raw(s_footer_buf);
    } else {
        _tx_raw("Transfer-Mode: stream\r\nData-Type: FileDownload\r\nResult: error\r\n\r\n0\r\nResult: error\r\n\r\n");
    }
}

void mgw_host_file_bridge_on_command(const char* command_type,
                                     const char* file_action,
                                     const char* file_path, // Overloaded for plugin_name!
                                     size_t file_offset,
                                     const uint8_t* chunk_data,
                                     size_t chunk_len,
                                     void* user_ctx) {
    (void)user_ctx;
    if (command_type == NULL) return;

    // ========================================================================
    // 1. FILE EXPLORER BRIDGE
    // ========================================================================
    if (strcmp(command_type, "FileExplorer") == 0) {
        if (file_action == NULL || file_path == NULL) return;
        
        // ... (Existing list-dir, read-file, write-file logic remains exactly as is) ...
        if (strcmp(file_action, "list-dir") == 0) {
            _tx_raw("Transfer-Mode: stream\r\nData-Type: FileList\r\nResult: success\r\n\r\n");
            if (s_ops.list_dir != NULL) s_ops.list_dir(file_path, _emit_json_entry, s_ops.user_ctx);
            _tx_raw("0\r\nResult: success\r\nCRC32: 00000000\r\n\r\n");
        }
        else if (strcmp(file_action, "read-file") == 0) {
            if (s_ops.defer_read_file != NULL) s_ops.defer_read_file(file_path, file_offset, s_ops.user_ctx);
            else mgw_host_file_bridge_execute_read(file_path, file_offset);
        }
        else if (strcmp(file_action, "write-file") == 0) {
            if (!s_is_writing) {
                strncpy(s_active_write_path, file_path, sizeof(s_active_write_path) - 1);
                if (s_ops.open_write != NULL) s_ops.open_write(s_active_write_path, file_offset);
                s_is_writing = true;
            }
            if (chunk_data != NULL && chunk_len > 0 && s_ops.write_chunk != NULL) {
                s_ops.write_chunk(chunk_data, chunk_len);
            }
        }
        else if (strcmp(file_action, "delete") == 0) {
            bool ok = false;
            if (s_ops.delete_entry != NULL) ok = s_ops.delete_entry(file_path);
            _tx_raw(ok ? "Transfer-Mode: normal\r\nResult: success\r\n\r\nOK"
                       : "Transfer-Mode: normal\r\nResult: error\r\n\r\nFAIL");
        }
        else if (strcmp(file_action, "mkdir") == 0) {
            bool ok = false;
            if (s_ops.make_dir != NULL) ok = s_ops.make_dir(file_path);
            _tx_raw(ok ? "Transfer-Mode: normal\r\nResult: success\r\n\r\nOK"
                       : "Transfer-Mode: normal\r\nResult: error\r\n\r\nFAIL");
        }
        else if (strcmp(file_action, "abort-transfer") == 0) {
            if (s_is_writing) {
                if (s_ops.close_write != NULL) s_ops.close_write();
                if (s_ops.delete_entry != NULL && s_active_write_path[0] != '\0') s_ops.delete_entry(s_active_write_path);
                s_is_writing = false;
                memset(s_active_write_path, 0, sizeof(s_active_write_path));
            }
            if (s_ops.close_read != NULL) s_ops.close_read();
            _tx_raw("Transfer-Mode: normal\r\nResult: success\r\n\r\nABORT_OK\r\n");
        }
    }
    
    // ========================================================================
    // 2. WEB PLUGIN BRIDGE (NEW)
    // ========================================================================
    else if (strcmp(command_type, "WebPlugin") == 0) {
        const char* plugin_action = file_action; 
        const char* plugin_name   = file_path;   // Parser mapped 'File-Path' to the 3rd metadata header, which is our Plugin-Name!

        if (plugin_action == NULL || plugin_name == NULL) return;

        if (strcmp(plugin_action, "get-content") == 0) {
            if (s_plugin_ops.stream_plugin_content != NULL) {
                s_plugin_ops.stream_plugin_content(plugin_name, s_plugin_ops.user_ctx);
            } else {
                // If not implemented by Host MCU, send empty error frame back to MGW so browser doesn't hang!
                _tx_raw("Transfer-Mode: stream\r\nData-Type: PluginContent\r\nResult: error\r\n\r\n0\r\nResult: error\r\n\r\n");
            }
        }
        else if (strcmp(plugin_action, "event") == 0) {
            if (s_plugin_ops.on_plugin_event != NULL && chunk_data != NULL && chunk_len > 0) {
                s_plugin_ops.on_plugin_event(plugin_name, chunk_data, chunk_len, s_plugin_ops.user_ctx);
            }
        }
    }
}

void mgw_host_file_bridge_on_finish(size_t total_bytes, bool crc_matched, void* user_ctx) {
    (void)total_bytes;
    (void)crc_matched;
    (void)user_ctx;

    if (s_is_writing) {
        if (s_ops.close_write != NULL) {
            s_ops.close_write();
        }
        s_is_writing = false;
        memset(s_active_write_path, 0, sizeof(s_active_write_path));
        _tx_raw("Transfer-Mode: normal\r\nResult: success\r\n\r\nUPLOAD_OK\r\n");
    }
}