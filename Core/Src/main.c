/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lcd.h"
#include "quadspi.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "helpers.h"
#include "lsm303c.h"
#include "spi.h"

#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
volatile bool rtc_wakeup_flag = false;
volatile bool button_center_flag = false;
volatile bool tim6_period_flag = false;
volatile bool usart_receive_flag = false;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == BUTTON_CENTER_Pin) {
		button_center_flag = true;
	}
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc) {
	rtc_wakeup_flag = true;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if (htim == &htim6) {
		tim6_period_flag = true;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart2) {
		usart_receive_flag = true;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_QUADSPI_Init();
  MX_SPI2_Init();
  MX_LCD_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  BSP_QSPI_Init();
  BSP_LCD_GLASS_Init();

  uint16_t init = (0b100001 << 8) | (0x27);
  LSM303C_AccInit(init);

  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
  uint32_t current_address = 0;

  char log_data[100];
  double acc_data[3];
  char lcd_data[6];

  const uint8_t buffer_size = 2;
  uint8_t usart_buffer[buffer_size];

  bool lcd_print_data = false;

  BSP_QSPI_Read((uint8_t*)&current_address, LAST_SUBSECTOR_ADDRESS, sizeof(current_address));
  if (current_address >= LAST_SUBSECTOR_ADDRESS) {
	  BSP_LCD_GLASS_DisplayString((uint8_t*)"CLEAR");
	  clearFlashAndResetAddress(&current_address);
	  BSP_LCD_GLASS_Clear();
  }

  //wakeup every 10 seconds
  HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, RTC_WAKEUP_COUNTER, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
  HAL_TIM_Base_Start_IT(&htim6);

  HAL_UART_Receive_IT(&huart2, usart_buffer, buffer_size);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (rtc_wakeup_flag) {
		  rtc_wakeup_flag = false;
		  HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
		  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
		  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
		  LSM303C_AccReadXYZinM_s2(acc_data);

		  sprintf(log_data,
				  "%02d:%02d:%02d, x = %f, y = %f, z = %f\r\n",
				  time.Hours,
				  time.Minutes,
				  time.Seconds,
				  acc_data[0],
				  acc_data[1],
				  acc_data[2]);

		  BSP_QSPI_Write((uint8_t*)log_data, current_address, strlen(log_data));
		  current_address += strlen(log_data);

		  if (current_address >= LAST_SUBSECTOR_ADDRESS) {
			  BSP_LCD_GLASS_DisplayString((uint8_t*)"CLEAR");
			  clearFlashAndResetAddress(&current_address);
			  BSP_LCD_GLASS_Clear();
		  } else {
			  BSP_QSPI_Erase_Block(LAST_SUBSECTOR_ADDRESS);
			  BSP_QSPI_Write((uint8_t*)&current_address, LAST_SUBSECTOR_ADDRESS, sizeof(current_address));
		  }
	  }

	  if (button_center_flag) {
		  button_center_flag = false;
		  HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);

		  sendLastNLogs(current_address, 10);
	  }

	  if (tim6_period_flag) {
		  tim6_period_flag = false;
		  if (lcd_print_data) {
			  bytesToString(lcd_data, current_address);
			  BSP_LCD_GLASS_Clear();
			  BSP_LCD_GLASS_DisplayString((uint8_t*)lcd_data);
		  } else {
			  BSP_LCD_GLASS_Clear();
			  BSP_LCD_GLASS_DisplayString((uint8_t*)"FILLED");
		  }
		  lcd_print_data = !lcd_print_data;
	  }

	  if (usart_receive_flag) {
		  usart_receive_flag = false;
		  HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
		  sendNLogs(usart_buffer[0], current_address, usart_buffer[1]);

		  HAL_UART_Receive_IT(&huart2, usart_buffer, buffer_size);
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

