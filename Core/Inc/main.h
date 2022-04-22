/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern volatile bool rtc_wakeup_flag;
extern volatile bool button_center_flag;
extern volatile bool tim6_period_flag;
extern volatile bool usart_receive_flag;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TIM6_PERIOD 2000-1
#define RTC_WAKEUP_COUNTER 20480-1
#define TIM6_PRESCALER 40000-1
#define USART_BUF_SIZE 9
#define BUTTON_CENTER_Pin GPIO_PIN_0
#define BUTTON_CENTER_GPIO_Port GPIOA
#define BUTTON_CENTER_EXTI_IRQn EXTI0_IRQn
#define LD4_Pin GPIO_PIN_2
#define LD4_GPIO_Port GPIOB
#define LD5_Pin GPIO_PIN_8
#define LD5_GPIO_Port GPIOE
#define MEMS_ACC_CS_Pin GPIO_PIN_0
#define MEMS_ACC_CS_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
