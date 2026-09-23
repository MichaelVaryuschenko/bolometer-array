/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PIN_A8_Pin GPIO_PIN_13
#define PIN_A8_GPIO_Port GPIOC
#define PIN_A9_Pin GPIO_PIN_14
#define PIN_A9_GPIO_Port GPIOC
#define PIN_A10_Pin GPIO_PIN_15
#define PIN_A10_GPIO_Port GPIOC
#define PIN_NSS_Pin GPIO_PIN_12
#define PIN_NSS_GPIO_Port GPIOB
#define PIN_A1_Pin GPIO_PIN_6
#define PIN_A1_GPIO_Port GPIOC
#define PIN_A2_Pin GPIO_PIN_7
#define PIN_A2_GPIO_Port GPIOC
#define PIN_A3_Pin GPIO_PIN_8
#define PIN_A3_GPIO_Port GPIOC
#define PIN_A4_Pin GPIO_PIN_9
#define PIN_A4_GPIO_Port GPIOC
#define PIN_A5_Pin GPIO_PIN_10
#define PIN_A5_GPIO_Port GPIOC
#define PIN_A6_Pin GPIO_PIN_11
#define PIN_A6_GPIO_Port GPIOC
#define PIN_A7_Pin GPIO_PIN_12
#define PIN_A7_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
