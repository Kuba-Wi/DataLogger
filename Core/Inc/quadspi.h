/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.h
  * @brief   This file contains all the function prototypes for
  *          the quadspi.c file
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
#ifndef __QUADSPI_H__
#define __QUADSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern QSPI_HandleTypeDef hqspi;

/* USER CODE BEGIN Private defines */

uint8_t CSP_QUADSPI_Init(void);
uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
uint8_t CSP_QSPI_Write(uint8_t* buffer, uint32_t address, uint32_t buffer_size);
uint8_t CSP_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t CSP_QSPI_Erase_Block(uint32_t BlockAddress);
uint8_t CSP_QSPI_EnableMemoryMappedMode(void);
uint8_t CSP_QSPI_Erase_Chip (void);

/* USER CODE END Private defines */

void MX_QUADSPI_Init(void);

/* USER CODE BEGIN Prototypes */

#define QSPI_FLASH_ADDRESS 0x90000000

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __QUADSPI_H__ */
