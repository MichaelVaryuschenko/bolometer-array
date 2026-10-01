// Uses TIM1 and SPI2+DMA peripherals

#include "bolometer.h"
#include "main.h"
#include "log.h"

extern volatile uint16_t flags_rt;
extern volatile uint16_t flags_er;

/*extern const uint16_t FLAG_RT_BOLOM_ADC_CNV_MODE;
extern const uint16_t FLAG_RT_BOLOM_ADC_ACQ_MODE;
extern const uint16_t FLAG_RT_BOLOM_DATA_ACQUIRED;*/

extern const uint16_t FLAG_ER_BOLOM_ADC_MODES_OVERLAP;

extern SPI_HandleTypeDef hspi2;

static bolom_state_t bolom_state = BOLOM_STATE_IDLE;
static uint8_t x_curr; // Not supposed to be changed outside of sensor_select_x
static uint8_t y_curr; // Not supposed to be changed outside of sensor_select_y
static uint8_t data_raw[2049]; // Array to store data from ADC; 1 extra element is needed due to SPI shenanigans (look into ADC CONV handler for details)

static void compress_data(char* res_array, uint16_t shift) {
  uint8_t* data_raw_shifted = &data_raw[shift];
  res_array[0] = data_raw_shifted[0] << 1 + data_raw_shifted[1] >> 7;
  for(int i = 1; i < 7; i++) {
    res_array[i] = data_raw_shifted[i] << i + data_raw_shifted[i + 1] >> (7 - i); // e. g.: res_array[1] = data_raw[1] << 1 + data_raw[2] >> 6
  }
}

static void sensor_select_x(uint8_t x) {
  uint8_t x_bin[5]; //x_bin[4] is MSB
  for (int i = 0; i < 5; i++) {
    x_bin[i] = x & 0x01;
    x = x >> 1;
  }
  if(x_bin[0]) {
    HAL_GPIO_WritePin(GPIOC, A1_Pin, GPIO_PIN_SET);
  }
  else {
    HAL_GPIO_WritePin(GPIOC, A1_Pin, GPIO_PIN_RESET);
  }
  if(x_bin[1]) {
    HAL_GPIO_WritePin(GPIOC, A2_Pin, GPIO_PIN_SET);
  }
  else {
    HAL_GPIO_WritePin(GPIOC, A2_Pin, GPIO_PIN_RESET);
  }
  if(x_bin[2]) {
    HAL_GPIO_WritePin(GPIOC, A3_Pin, GPIO_PIN_SET);
  }
  else {
    HAL_GPIO_WritePin(GPIOC, A3_Pin, GPIO_PIN_RESET);
  }
  if(x_bin[3]) {
    HAL_GPIO_WritePin(GPIOC, A4_Pin, GPIO_PIN_SET);
  }
  else {
    HAL_GPIO_WritePin(GPIOC, A4_Pin, GPIO_PIN_RESET);
  }
  if(x_bin[4]) {
    HAL_GPIO_WritePin(GPIOC, A5_Pin, GPIO_PIN_SET);
  }
  else {
    HAL_GPIO_WritePin(GPIOC, A5_Pin, GPIO_PIN_RESET);
  }
  x_curr = x;
}

static void sensor_select_y(uint8_t y) {
  uint8_t y_bin[5]; //y_bin[4] is MSB
  for (int i = 0; i < 5; i++) {
    y_bin[i] = y & 0x01;
    y = y >> 1;
  }
  if(y_bin[0]) {
    HAL_GPIO_WritePin(GPIOC, A6_Pin, GPIO_PIN_SET);
  }
  else {
    HAL_GPIO_WritePin(GPIOC, A6_Pin, GPIO_PIN_RESET);
  }
  if(y_bin[1]) {
    HAL_GPIO_WritePin(GPIOC, A7_Pin, GPIO_PIN_SET);
  }
  else {
    HAL_GPIO_WritePin(GPIOC, A7_Pin, GPIO_PIN_RESET);
  }
  if(y_bin[2]) {
    HAL_GPIO_WritePin(GPIOC, A8_Pin, GPIO_PIN_SET);
  }
  else {
    HAL_GPIO_WritePin(GPIOC, A8_Pin, GPIO_PIN_RESET);
  }
  if(y_bin[3]) {
    HAL_GPIO_WritePin(GPIOC, A9_Pin, GPIO_PIN_SET);
  }
  else {
    HAL_GPIO_WritePin(GPIOC, A9_Pin, GPIO_PIN_RESET);
  }
  if(y_bin[4]) {
    HAL_GPIO_WritePin(GPIOC, A10_Pin, GPIO_PIN_SET);
  }
  else {
    HAL_GPIO_WritePin(GPIOC, A10_Pin, GPIO_PIN_RESET);
  }
  y_curr = y;
}

void bolometer_init(void) {
  sensor_select_x(0);
  sensor_select_y(0);
  
  bolom_state = BOLOM_STATE_ADC_CAL; // Purely for code architecture purposes
  HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Receive(&hspi2, data_raw, 3, 5000); // Necessary to provide 24 clocks on SCLK for ADC OFFCAL routine; data_raw is used as a dummy buffer, being unused properly at this point
  bolom_state = BOLOM_STATE_IDLE;
  HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);
}

void bolometer_handle(void) {
  switch(bolom_state) {
  case BOLOM_STATE_ADC_CNV:
    bolometer_adc_cnv_handle();
    break;
  case BOLOM_STATE_ADC_ACQ:
    bolometer_adc_acq_handle();
    break;
  case BOLOM_STATE_DATA_PROCESSING:
    bolometer_data_processing_handle();
    break;
  default:
    break;
  }
}

void bolometer_adc_cnv_handle(void) {
  HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Receive_DMA(&hspi2, &data_raw[(32 * y_curr + x_curr) * 2], 3); // Writes data from sensor to corresponding segment of sensor_data_raw array; two bytes actually store valuable information, extra byte is needed to keep SCLK running
  if(x_curr < 31) {
    sensor_select_x(x_curr + 1);
  }
  else if(y_curr < 31) {
    sensor_select_x(0);
    sensor_select_y(y_curr + 1);
  }
  bolom_state = BOLOM_STATE_IDLE;
}

void bolometer_adc_acq_handle(void) {
  HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);
  bolom_state = BOLOM_STATE_IDLE;
}


/*
Each sensor outputs 14 bits on information stored in two uint8_t variables
External loop cycles through sensor y coordinate
Internal loop compresses all data from sensors with corresponding y coordinate and sends it to master device
*/
void bolometer_data_processing_handle() {
  HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);
  sensor_select_x(0);
  sensor_select_y(0);
  char data[14 * 32 / 8]; // 14 bits from each of 32 sensors in a row
  for(uint8_t i = 0; i < 32; i++) {
    for(uint8_t j = 0; j < 2048 / 32 / 8; j++) {
      compress_data(&data[7 * j], 64 * i + 8 * j);
    }
    log_add_entry(LOG_ENTRY_LVL_DAT, data, 14 * 32 / 8);
    log_send_all_entries();
  }
  bolom_state = BOLOM_STATE_IDLE;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  if(htim->Instance==TIM1) {
    if(bolom_state == BOLOM_STATE_IDLE) {
      //flags_rt |= FLAG_RT_BOLOM_ADC_CNV_MODE;
      if(x_curr == 31 && y_curr == 31) {
        bolom_state = BOLOM_STATE_DATA_PROCESSING;
      }
      else {
        bolom_state = BOLOM_STATE_ADC_ACQ;
      }
    }
    else { // Branch prevents state change for an additional clock. 
      if(bolom_state == BOLOM_STATE_ADC_CNV) { // Branch is entered if CNV state handler is still active
        flags_er |= FLAG_ER_BOLOM_ADC_MODES_OVERLAP; // TO-DO: handle this flag in superloop
      }
    }
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if(htim->Instance==TIM1) {
    if(bolom_state == BOLOM_STATE_IDLE) {
      //flags_rt |= FLAG_RT_BOLOM_ADC_ACQ_MODE;
      bolom_state = BOLOM_STATE_ADC_CNV;
    }
    /*else { // Branch prevents state change for an additional clock. 
      if(bolom_state == BOLOM_STATE_ADC_ACQ) { // Branch is entered if CNV state handler is still active
        flagS_er |= FLAG_ER_BOLOM_ADC_MODES_OVERLAP; // TO-DO: handle this flag in superloop
      }
    }*/
  }
}