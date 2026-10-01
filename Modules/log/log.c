#include "log.h"
#include "usbd_cdc_if.h"

static const log_entry_lvl_t log_lvl = LOG_ENTRY_LVL_DBG; // Messages with level below this threshold will not be logged
static log_entry_t log[LOG_LEN];
static log_state_t log_state = LOG_STATE_IDLE;
static uint8_t curr_entry = 0; // Index of entry being read at the moment
static uint8_t num_of_entries = 0;

static void log_add_prefix(uint8_t* entry_body, char* entry_prefix) { // entry_prefix is supposed to be of length PREF_LEN
  for(int i = 0; i < PREF_LEN; i++) {
    entry_body[i] = entry_prefix[i];
  }
}

// Returns 0 for successful operation; returns 1 if log is inaccessible at the moment
uint8_t log_add_entry(log_entry_lvl_t level, char* body, uint8_t len) {
  if(log_state != LOG_STATE_IDLE) {
    return 1;
  }
  else {
    log_state = LOG_STATE_WRITING;
    if(!level < log_lvl) {
      log_entry_t temp = log[LOG_LEN - 1];
      for (uint8_t i = LOG_LEN - 1; i > 0; i--) {
        log[i] = log[i - 1];
      }
      log[0] = temp;
      log[0].level = level;
      log[0].len = len + PREF_LEN;
      switch (level) {
      case LOG_ENTRY_LVL_DBG:
        log_add_prefix(log[0].body, "DBG: ");
        break;
      case LOG_ENTRY_LVL_ERR:
        log_add_prefix(log[0].body, "ERR: ");
        break;
      case LOG_ENTRY_LVL_DAT:
        log_add_prefix(log[0].body, "DAT: ");
        break;
      }
      for(int i = 0; i < len; i++) {
        log[0].body[i] = body[i + 3];
      }
      num_of_entries++;
    }
    log_state = LOG_STATE_IDLE;
    return 0;
  }
}

// Returns 0 for successful operation; returns 1 if log is inaccessible at the moment
uint8_t log_send_entries(uint8_t n) {
  if(log_state != LOG_STATE_IDLE) {
    return 1;
  }
  else {
    log_state = LOG_STATE_READING;
    curr_entry = n - 1;
    CDC_Transmit_FS(log[curr_entry].body, log[curr_entry].len);
    return 0;
  }
}

uint8_t log_send_next_entry() {
  if(log_state != LOG_STATE_READING) { // If transmission is not supposed to happen at all at the moment
    // TO-DO: raise flag for logging an error entry in main.c
    return 1;
  }
  else {
    if(curr_entry > 0) {
      curr_entry--;
      return CDC_Transmit_FS(log[curr_entry].body, log[curr_entry].len);
    }
    else {
      log_state = LOG_STATE_IDLE;
      return 0;
    }
  }
}

uint8_t log_send_all_entries(void) {
  uint8_t retval = log_send_entries(num_of_entries);
  num_of_entries = 0;
  return retval;
}

uint8_t log_device_info(void) {
  // TO-DO: implement!
  uint8_t retval = 0;
  retval += log_add_entry(LOG_ENTRY_LVL_DAT, "Bolometric matrix, firmware v1.0.0 (IN DEV)\n", 44);
  retval += log_add_entry(LOG_ENTRY_LVL_DAT, "Source repo: https://github.com/MichaelVaryuschenko/bolometer-array\n", 68);
  retval += log_send_all_entries();
  return retval;
}
