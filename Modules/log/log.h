#pragma once

#include <stdint.h>

#define LOG_LEN 12 // No more than 255
#define ENTRY_MAX_LEN 453 // 5 + 14 * 32
#define PREF_LEN 5

typedef enum {
  LOG_ENTRY_LVL_DBG = 0,
  LOG_ENTRY_LVL_ERR = 1,
  LOG_ENTRY_LVL_DAT = 2
} log_entry_lvl_t;
typedef struct {
  log_entry_lvl_t level;
  uint8_t body[ENTRY_MAX_LEN]; // Actual entry stored as string
  uint8_t len;
} log_entry_t;
typedef enum {
  LOG_STATE_IDLE,
  LOG_STATE_WRITING,
  LOG_STATE_READING
} log_state_t;

static void log_add_prefix(uint8_t* entry_body, char* entry_level);

uint8_t log_add_entry(log_entry_lvl_t level, char* body, uint8_t len);
uint8_t log_send_entries(uint8_t n); // Transmit n log entries over USB interface
uint8_t log_send_next_entry(void);
uint8_t log_send_all_entries(void);
uint8_t log_device_info(void);