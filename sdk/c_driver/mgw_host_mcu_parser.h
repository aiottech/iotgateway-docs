/**
 * @file mgw_host_mcu_parser.h
 * @brief Official Unified Host MCU Parser Driver for MGW Gateway (Normal & Stream V2).
 * 
 * Supports 8-bit, 16-bit, and 32-bit Host MCUs (STM32, NXP, PIC, AVR, ESP32).
 * Operates with 0 dynamic memory allocations and supports both internal static buffers
 * and user-injected custom memory (Heap / PSRAM).
 */

#ifndef MGW_HOST_MCU_PARSER_H
#define MGW_HOST_MCU_PARSER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MGW_LINE_BUF_SIZE
#define MGW_LINE_BUF_SIZE  512 // Default internal line buffer reservoir (bytes)
#endif

#define MGW_STREAM_ACK_BYTE 0x06 // ASCII ACK byte for 1-Byte Flow Control

typedef enum {
    MGW_MODE_UNKNOWN = 0,
    MGW_MODE_NORMAL  = 1,
    MGW_MODE_STREAM  = 2
} mgw_transfer_mode_t;

typedef struct mgw_unified_parser mgw_unified_parser_t;

// Standard Callback Signatures
typedef void (*mgw_on_normal_frame_fn)(const char* command_name, const uint8_t* body, size_t body_len, void* user_ctx);
typedef void (*mgw_on_stream_chunk_fn)(const uint8_t* chunk_data, size_t chunk_len, void* user_ctx);

// Extended Callback Signature with Protocol Metadata (Data-Type, Hub-Name, Hub-Method)
typedef void (*mgw_on_stream_chunk_ex_fn)(const char* data_type, 
                                          const char* hub_name, 
                                          const char* hub_method, 
                                          const uint8_t* chunk_data, 
                                          size_t chunk_len, 
                                          void* user_ctx);

// Dedicated Command / File Explorer Callback Signature (Supports File-Offset!)
typedef void (*mgw_on_stream_command_fn)(const char* command_type,
                                         const char* file_action,
                                         const char* file_path,
                                         size_t file_offset,
                                         const uint8_t* chunk_data,
                                         size_t chunk_len,
                                         void* user_ctx);

typedef void (*mgw_on_stream_finish_fn)(size_t total_bytes, bool crc_matched, void* user_ctx);
typedef void (*mgw_tx_byte_fn)(uint8_t tx_byte, void* user_ctx);

/**
 * @brief Unified Parser Configuration Struct.
 * All callbacks and custom buffers are optional (can be NULL).
 */
typedef struct {
    bool                        enable_ack_flow;      // false by default (continuous streaming)
    uint8_t*                    custom_buffer;        // Set to NULL to use internal 512-byte buffer
    size_t                      custom_buffer_size;   // Set to 0 if custom_buffer is NULL
    
    // Optional Callbacks (NULL-safe)
    mgw_on_normal_frame_fn       on_normal_frame;      // Called on Normal Mode payload slices
    mgw_on_stream_chunk_fn       on_stream_chunk;      // Standard Stream chunk callback
    mgw_on_stream_chunk_ex_fn    on_stream_chunk_ex;   // Extended callback with Data-Type, Hub-Name & Hub-Method
    mgw_on_stream_command_fn     on_stream_command;    // Command callback for File Explorer & Gateway commands!
    mgw_on_stream_finish_fn      on_stream_finish;     // Called on Stream completion + CRC32 check
    mgw_tx_byte_fn               tx_byte;              // Emits 0x06 ACK byte if flow ack active
    
    void*                        user_ctx;             // User context passed back to callbacks
} mgw_parser_config_t;

struct mgw_unified_parser {
    uint8_t                 state;
    mgw_transfer_mode_t     active_mode;
    bool                    enable_ack_flow;
    size_t                  expected_len;
    size_t                  current_read;
    size_t                  total_bytes_received;
    uint32_t                running_crc32;
    uint32_t                crc32_expected;

    // Parsed Protocol Metadata Fields
    char                    data_type[32];     // e.g., "SignalR", "FileDownload", "HttpStream"
    char                    hub_name[64];      // e.g., "chathub", "cfphub"
    char                    hub_method[64];    // e.g., "sendToAll"
    
    // Parsed Command / File Explorer Metadata Fields (MGW -> Host MCU)
    char                    command_type[32];  // e.g., "FileExplorer"
    char                    file_action[32];   // e.g., "list-dir", "read-file", "write-file", "delete", "mkdir", "abort-transfer"
    char                    file_path[128];    // e.g., "/logs/data.csv"
    size_t                  file_offset;       // Starting byte offset
    size_t                  file_total;        // Total expected file size
    
    // Callbacks (with NULL guards)
    mgw_on_normal_frame_fn       on_normal_frame;
    mgw_on_stream_chunk_fn       on_stream_chunk;
    mgw_on_stream_chunk_ex_fn    on_stream_chunk_ex;
    mgw_on_stream_command_fn     on_stream_command;
    mgw_on_stream_finish_fn      on_stream_finish;
    mgw_tx_byte_fn               tx_byte;
    void*                        user_ctx;
    
    // Line Buffer Pointers
    uint8_t*                line_buf;
    size_t                  line_buf_size;
    size_t                  line_idx;

    // Internal Static Reservoir
    uint8_t                 _internal_buf[MGW_LINE_BUF_SIZE];
};

/**
 * @brief Initializes the unified parser using a single configuration struct.
 * @param parser Pointer to parser struct.
 * @param config Pointer to configuration struct.
 */
void mgw_unified_parser_init(mgw_unified_parser_t* parser, const mgw_parser_config_t* config);

/**
 * @brief Feeds incoming raw UART/SPI bytes directly into the state machine.
 * @param parser Pointer to parser struct.
 * @param data Pointer to raw incoming byte array.
 * @param len Number of bytes to feed.
 */
void mgw_unified_parser_feed_bytes(mgw_unified_parser_t* parser, const uint8_t* data, size_t len);

/**
 * @brief Resets the parser state back to idle.
 * @param parser Pointer to parser struct.
 */
void mgw_unified_parser_reset(mgw_unified_parser_t* parser);

// Metadata Accessor Getters
const char* mgw_unified_parser_get_data_type(const mgw_unified_parser_t* parser);
const char* mgw_unified_parser_get_hub_name(const mgw_unified_parser_t* parser);
const char* mgw_unified_parser_get_hub_method(const mgw_unified_parser_t* parser);
const char* mgw_unified_parser_get_command_type(const mgw_unified_parser_t* parser);
const char* mgw_unified_parser_get_file_action(const mgw_unified_parser_t* parser);
const char* mgw_unified_parser_get_file_path(const mgw_unified_parser_t* parser);
size_t      mgw_unified_parser_get_file_offset(const mgw_unified_parser_t* parser);
size_t      mgw_unified_parser_get_file_total(const mgw_unified_parser_t* parser);

#endif // MGW_HOST_MCU_PARSER_H