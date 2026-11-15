/**
 * @file mgw_host_mcu_parser.c
 * @brief Official Unified Host MCU Parser Driver Implementation.
 */

#include "mgw_host_mcu_parser.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Internal 6-State Machine Definitions
#define STATE_WAIT_INIT_HEADER           0
#define STATE_READ_NORMAL_HEADERS        1
#define STATE_READ_NORMAL_BODY           2
#define STATE_READ_STREAM_INIT_HEADERS   3  // Drains & parses stream metadata headers until \r\n\r\n
#define STATE_WAIT_CHUNK                 4
#define STATE_READ_STREAM_PAYLOAD        5
#define STATE_WAIT_STREAM_FOOTER         6

// Standard IEEE 802.3 CRC32 Calculation (Zero External Dependencies)
static uint32_t _calc_crc32(uint32_t crc, const uint8_t *buf, size_t len) {
    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (int k = 0; k < 8; k++) {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
        }
    }
    return ~crc;
}

static bool _str_istartswith(const char* str, const char* prefix) {
    if (!str || !prefix) return false;
    while (*prefix) {
        if (tolower((unsigned char)*str) != tolower((unsigned char)*prefix)) {
            return false;
        }
        str++;
        prefix++;
    }
    return true;
}

static size_t _hex_to_size(const char* hex_str) {
    size_t val = 0;
    while (*hex_str) {
        char c = *hex_str++;
        if (c >= '0' && c <= '9')      val = (val << 4) | (c - '0');
        else if (c >= 'a' && c <= 'f') val = (val << 4) | (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') val = (val << 4) | (c - 'A' + 10);
        else break;
    }
    return val;
}

static uint32_t _hex_to_u32(const char* hex_str) {
    uint32_t val = 0;
    while (*hex_str) {
        char c = *hex_str++;
        if (c >= '0' && c <= '9')      val = (val << 4) | (c - '0');
        else if (c >= 'a' && c <= 'f') val = (val << 4) | (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') val = (val << 4) | (c - 'A' + 10);
        else break;
    }
    return val;
}

void mgw_unified_parser_reset(mgw_unified_parser_t* parser) {
    if (!parser) return;
    parser->state                = STATE_WAIT_INIT_HEADER;
    parser->active_mode          = MGW_MODE_UNKNOWN;
    parser->expected_len         = 0;
    parser->current_read         = 0;
    parser->total_bytes_received = 0;
    parser->running_crc32        = 0;
    parser->crc32_expected       = 0;
    parser->line_idx             = 0;
    parser->file_offset          = 0;
    parser->file_total           = 0;

    memset(parser->data_type, 0, sizeof(parser->data_type));
    memset(parser->hub_name, 0, sizeof(parser->hub_name));
    memset(parser->hub_method, 0, sizeof(parser->hub_method));
    memset(parser->command_type, 0, sizeof(parser->command_type));
    memset(parser->file_action, 0, sizeof(parser->file_action));
    memset(parser->file_path, 0, sizeof(parser->file_path));

    if (parser->line_buf && parser->line_buf_size > 0) {
        parser->line_buf[0]      = '\0';
    }
}

void mgw_unified_parser_init(mgw_unified_parser_t* parser, const mgw_parser_config_t* config) {
    if (!parser) return;

    memset(parser, 0, sizeof(mgw_unified_parser_t));

    // 1. Resolve Memory Buffer (Custom vs Default Internal)
    if (config && config->custom_buffer && config->custom_buffer_size > 0) {
        parser->line_buf      = config->custom_buffer;
        parser->line_buf_size = config->custom_buffer_size;
    } else {
        parser->line_buf      = parser->_internal_buf;
        parser->line_buf_size = sizeof(parser->_internal_buf);
    }

    // 2. Bind Configuration & Callbacks
    if (config) {
        parser->enable_ack_flow    = config->enable_ack_flow;
        parser->on_normal_frame    = config->on_normal_frame;
        parser->on_stream_chunk    = config->on_stream_chunk;
        parser->on_stream_chunk_ex = config->on_stream_chunk_ex;
        parser->on_stream_command  = config->on_stream_command;
        parser->on_stream_finish   = config->on_stream_finish;
        parser->tx_byte            = config->tx_byte;
        parser->user_ctx           = config->user_ctx;
    }

    mgw_unified_parser_reset(parser);
}

void mgw_unified_parser_feed_bytes(mgw_unified_parser_t* parser, const uint8_t* data, size_t len) {
    if (!parser || !data || len == 0) return;

    size_t i = 0;

    while (i < len) {
        switch (parser->state) {

            // STATE 0: Wait for 'Transfer-Mode:'
            case STATE_WAIT_INIT_HEADER: {
                char c = (char)data[i++];

                if (c == '\n') {
                    if (parser->line_idx > 0 && parser->line_buf[parser->line_idx - 1] == '\r') {
                        parser->line_buf[parser->line_idx - 1] = '\0';
                    } else {
                        parser->line_buf[parser->line_idx] = '\0';
                    }

                    char* line = (char*)parser->line_buf;

                    if (_str_istartswith(line, "Transfer-Mode:")) {
                        char* mode_val = line + 14;
                        while (*mode_val == ' ' || *mode_val == '\t') mode_val++;

                        if (_str_istartswith(mode_val, "normal")) {
                            parser->active_mode  = MGW_MODE_NORMAL;
                            parser->expected_len = 0;
                            parser->state        = STATE_READ_NORMAL_HEADERS;
                        } else if (_str_istartswith(mode_val, "stream")) {
                            parser->active_mode          = MGW_MODE_STREAM;
                            parser->total_bytes_received = 0;
                            parser->running_crc32        = 0;
                            parser->file_offset          = 0;
                            parser->file_total           = 0;
                            parser->state                = STATE_READ_STREAM_INIT_HEADERS;
                        }
                    }

                    parser->line_idx = 0;
                } else if (c != '\r') {
                    if (parser->line_idx < parser->line_buf_size - 1) {
                        parser->line_buf[parser->line_idx++] = (uint8_t)c;
                    }
                }
                break;
            }

            // STATE 1: Read Normal Mode Headers (Extract Data-Len)
            case STATE_READ_NORMAL_HEADERS: {
                char c = (char)data[i++];

                if (c == '\n') {
                    if (parser->line_idx > 0 && parser->line_buf[parser->line_idx - 1] == '\r') {
                        parser->line_buf[parser->line_idx - 1] = '\0';
                    } else {
                        parser->line_buf[parser->line_idx] = '\0';
                    }

                    char* line = (char*)parser->line_buf;

                    if (_str_istartswith(line, "Data-Len:")) {
                        char* len_str = line + 9;
                        while (*len_str == ' ' || *len_str == '\t') len_str++;
                        parser->expected_len = (size_t)atoi(len_str);
                    }

                    // Empty Line indicates End of Headers!
                    if (strlen(line) == 0) {
                        if (parser->expected_len == 0) {
                            if (parser->on_normal_frame != NULL) {
                                parser->on_normal_frame("normal", NULL, 0, parser->user_ctx);
                            }
                            mgw_unified_parser_reset(parser);
                        } else {
                            parser->current_read = 0;
                            parser->state        = STATE_READ_NORMAL_BODY;
                        }
                    }

                    parser->line_idx = 0;
                } else if (c != '\r') {
                    if (parser->line_idx < parser->line_buf_size - 1) {
                        parser->line_buf[parser->line_idx++] = (uint8_t)c;
                    }
                }
                break;
            }

            // STATE 2: Read Normal Mode Body (Streaming Slices)
            case STATE_READ_NORMAL_BODY: {
                size_t remaining_in_body  = parser->expected_len - parser->current_read;
                size_t available_in_chunk = len - i;
                size_t to_read            = (remaining_in_body < available_in_chunk) ? remaining_in_body : available_in_chunk;

                if (parser->on_normal_frame != NULL && to_read > 0) {
                    parser->on_normal_frame("normal", &data[i], to_read, parser->user_ctx);
                }

                parser->current_read += to_read;
                i += to_read;

                if (parser->current_read >= parser->expected_len) {
                    mgw_unified_parser_reset(parser);
                }
                break;
            }

            // STATE 3: Read Stream Initial Headers
            case STATE_READ_STREAM_INIT_HEADERS: {
                char c = (char)data[i++];

                if (c == '\n') {
                    if (parser->line_idx > 0 && parser->line_buf[parser->line_idx - 1] == '\r') {
                        parser->line_buf[parser->line_idx - 1] = '\0';
                    } else {
                        parser->line_buf[parser->line_idx] = '\0';
                    }

                    char* line = (char*)parser->line_buf;

                    // Parse Data-Type:
                    if (_str_istartswith(line, "Data-Type:")) {
                        char* val = line + 10;
                        while (*val == ' ' || *val == '\t') val++;
                        strncpy(parser->data_type, val, sizeof(parser->data_type) - 1);
                    }
                    // Parse Hub-Name:
                    else if (_str_istartswith(line, "Hub-Name:")) {
                        char* val = line + 9;
                        while (*val == ' ' || *val == '\t') val++;
                        strncpy(parser->hub_name, val, sizeof(parser->hub_name) - 1);
                    }
                    // Parse Hub-Method:
                    else if (_str_istartswith(line, "Hub-Method:")) {
                        char* val = line + 11;
                        while (*val == ' ' || *val == '\t') val++;
                        strncpy(parser->hub_method, val, sizeof(parser->hub_method) - 1);
                    }
                    // Parse Command-Type: (MGW -> Host MCU)
                    else if (_str_istartswith(line, "Command-Type:")) {
                        char* val = line + 13;
                        while (*val == ' ' || *val == '\t') val++;
                        strncpy(parser->command_type, val, sizeof(parser->command_type) - 1);
                    }
                    // Parse File-Action: or Plugin-Action: (MGW -> Host MCU)
                    else if (_str_istartswith(line, "File-Action:") || _str_istartswith(line, "Plugin-Action:")) {
                        char* val = strchr(line, ':') + 1;
                        while (*val == ' ' || *val == '\t') val++;
                        strncpy(parser->file_action, val, sizeof(parser->file_action) - 1);
                    }
                    // Parse File-Path: or Plugin-Name: (MGW -> Host MCU)
                    else if (_str_istartswith(line, "File-Path:") || _str_istartswith(line, "Plugin-Name:")) {
                        char* val = strchr(line, ':') + 1;
                        while (*val == ' ' || *val == '\t') val++;
                        strncpy(parser->file_path, val, sizeof(parser->file_path) - 1);
                    }
                    // Parse File-Offset: (Resumable Transfers)
                    else if (_str_istartswith(line, "File-Offset:")) {
                        char* val = line + 12;
                        while (*val == ' ' || *val == '\t') val++;
                        parser->file_offset = (size_t)strtoull(val, NULL, 10);
                    }
                    // Parse File-Total: (Resumable Transfers)
                    else if (_str_istartswith(line, "File-Total:")) {
                        char* val = line + 11;
                        while (*val == ' ' || *val == '\t') val++;
                        parser->file_total = (size_t)strtoull(val, NULL, 10);
                    }

                    // Empty Line indicates End of Initial Stream Headers!
                    if (strlen(line) == 0) {
                        parser->state = STATE_WAIT_CHUNK;
                    }

                    parser->line_idx = 0;
                } else if (c != '\r') {
                    if (parser->line_idx < parser->line_buf_size - 1) {
                        parser->line_buf[parser->line_idx++] = (uint8_t)c;
                    }
                }
                break;
            }

            // STATE 4: Wait for Chunk Header (<hex_len>\r\n)
            case STATE_WAIT_CHUNK: {
                char c = (char)data[i++];

                if (c == '\n') {
                    if (parser->line_idx > 0 && parser->line_buf[parser->line_idx - 1] == '\r') {
                        parser->line_buf[parser->line_idx - 1] = '\0';
                    } else {
                        parser->line_buf[parser->line_idx] = '\0';
                    }

                    char* line = (char*)parser->line_buf;

                    // Stream Preemption Shield: If Transfer-Mode arrives mid-stream, abort and start new frame!
                    if (_str_istartswith(line, "Transfer-Mode:")) {
                        char* mode_val = line + 14;
                        while (*mode_val == ' ' || *mode_val == '\t') mode_val++;

                        if (_str_istartswith(mode_val, "normal")) {
                            parser->active_mode  = MGW_MODE_NORMAL;
                            parser->expected_len = 0;
                            parser->state        = STATE_READ_NORMAL_HEADERS;
                        } else if (_str_istartswith(mode_val, "stream")) {
                            parser->active_mode          = MGW_MODE_STREAM;
                            parser->total_bytes_received = 0;
                            parser->running_crc32        = 0;
                            parser->file_offset          = 0;
                            parser->file_total           = 0;
                            parser->state                = STATE_READ_STREAM_INIT_HEADERS;
                        }
                        parser->line_idx = 0;
                        break;
                    }

                    if (strlen(line) > 0) {
                        if (strcmp(line, "0") == 0) {
                            if (parser->on_stream_command != NULL && parser->command_type[0] != '\0' && parser->total_bytes_received == 0) {
                                parser->on_stream_command(parser->command_type,
                                                          parser->file_action,
                                                          parser->file_path,
                                                          parser->file_offset,
                                                          NULL,
                                                          0,
                                                          parser->user_ctx);
                            }
                            parser->state = STATE_WAIT_STREAM_FOOTER;
                        } else {
                            size_t chunk_len = _hex_to_size(line);
                            if (chunk_len > 0) {
                                parser->expected_len = chunk_len;
                                parser->current_read = 0;
                                parser->state        = STATE_READ_STREAM_PAYLOAD;
                            }
                        }
                    }

                    parser->line_idx = 0;
                } else if (c != '\r') {
                    if (parser->line_idx < parser->line_buf_size - 1) {
                        parser->line_buf[parser->line_idx++] = (uint8_t)c;
                    }
                }
                break;
            }

            // STATE 5: Read Stream Chunk Payload
            case STATE_READ_STREAM_PAYLOAD: {
                size_t remaining_in_chunk = parser->expected_len - parser->current_read;
                size_t available_in_buf   = len - i;
                size_t to_read            = (remaining_in_chunk < available_in_buf) ? remaining_in_chunk : available_in_buf;

                if (to_read > 0) {
                    parser->running_crc32 = _calc_crc32(parser->running_crc32, &data[i], to_read);
                    parser->total_bytes_received += to_read;

                    // Invoke Command Callback with file_offset!
                    if (parser->on_stream_command != NULL && parser->command_type[0] != '\0') {
                        parser->on_stream_command(parser->command_type,
                                                  parser->file_action,
                                                  parser->file_path,
                                                  parser->file_offset,
                                                  &data[i],
                                                  to_read,
                                                  parser->user_ctx);
                    }

                    // Invoke Extended Callback
                    if (parser->on_stream_chunk_ex != NULL) {
                        parser->on_stream_chunk_ex(parser->data_type, 
                                                   parser->hub_name, 
                                                   parser->hub_method, 
                                                   &data[i], 
                                                   to_read, 
                                                   parser->user_ctx);
                    }
                    // Invoke Standard Callback
                    if (parser->on_stream_chunk != NULL) {
                        parser->on_stream_chunk(&data[i], to_read, parser->user_ctx);
                    }

                    parser->current_read += to_read;
                    i += to_read;
                }

                if (parser->current_read >= parser->expected_len) {
                    if (parser->enable_ack_flow && parser->tx_byte != NULL) {
                        parser->tx_byte(MGW_STREAM_ACK_BYTE, parser->user_ctx);
                    }
                    parser->state = STATE_WAIT_CHUNK;
                }
                break;
            }

            // STATE 6: Read Stream Summary Footer (Verify CRC32)
            case STATE_WAIT_STREAM_FOOTER: {
                char c = (char)data[i++];

                if (c == '\n') {
                    if (parser->line_idx > 0 && parser->line_buf[parser->line_idx - 1] == '\r') {
                        parser->line_buf[parser->line_idx - 1] = '\0';
                    } else {
                        parser->line_buf[parser->line_idx] = '\0';
                    }

                    char* line = (char*)parser->line_buf;

                    if (_str_istartswith(line, "CRC32:")) {
                        char* crc_str = line + 6;
                        while (*crc_str == ' ' || *crc_str == '\t') crc_str++;
                        parser->crc32_expected = _hex_to_u32(crc_str);
                    }

                    if (strlen(line) == 0) {
                        bool crc_matched = (parser->running_crc32 == parser->crc32_expected);

                        if (parser->on_stream_finish != NULL) {
                            parser->on_stream_finish(parser->total_bytes_received, crc_matched, parser->user_ctx);
                        }

                        mgw_unified_parser_reset(parser);
                    }

                    parser->line_idx = 0;
                } else if (c != '\r') {
                    if (parser->line_idx < parser->line_buf_size - 1) {
                        parser->line_buf[parser->line_idx++] = (uint8_t)c;
                    }
                }
                break;
            }

            default:
                mgw_unified_parser_reset(parser);
                break;
        }
    }
}

// Metadata Accessor Getters
const char* mgw_unified_parser_get_data_type(const mgw_unified_parser_t* parser) {
    return parser ? parser->data_type : "";
}

const char* mgw_unified_parser_get_hub_name(const mgw_unified_parser_t* parser) {
    return parser ? parser->hub_name : "";
}

const char* mgw_unified_parser_get_hub_method(const mgw_unified_parser_t* parser) {
    return parser ? parser->hub_method : "";
}

const char* mgw_unified_parser_get_command_type(const mgw_unified_parser_t* parser) {
    return parser ? parser->command_type : "";
}

const char* mgw_unified_parser_get_file_action(const mgw_unified_parser_t* parser) {
    return parser ? parser->file_action : "";
}

const char* mgw_unified_parser_get_file_path(const mgw_unified_parser_t* parser) {
    return parser ? parser->file_path : "";
}

size_t mgw_unified_parser_get_file_offset(const mgw_unified_parser_t* parser) {
    return parser ? parser->file_offset : 0;
}

size_t mgw_unified_parser_get_file_total(const mgw_unified_parser_t* parser) {
    return parser ? parser->file_total : 0;
}