#pragma once

#include <stdint.h>

typedef enum {
  BOLOM_STATE_IDLE,
  BOLOM_STATE_ADC_CAL,
  BOLOM_STATE_ADC_ACQ,
  BOLOM_STATE_ADC_CNV, // ADC CNV state and simultaneous sensor selection state
  BOLOM_STATE_DATA_PROCESSING
} bolom_state_t;

void bolometer_init(uint8_t* context_data_raw);
void bolometer_handle(void);
void bolometer_adc_cnv_handle(void); // Size of buf must be 2048
void bolometer_adc_acq_handle(void);
void bolometer_data_processing_handle();
static void sensor_select_x(uint8_t x); // x from 0 to 31
static void sensor_select_y(uint8_t y); // y from 0 to 31