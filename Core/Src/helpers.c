#include "helpers.h"
#include "quadspi.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>

void clearFlashAndResetAddress(uint32_t* address) {
	*address = 0;
	BSP_QSPI_Erase_Chip();
	BSP_QSPI_Write((uint8_t*)address, LAST_SUBSECTOR_ADDRESS, sizeof(address));

	rtc_wakeup_flag = false;
	button_center_flag = false;
}

void sendLastNLogs(uint32_t current_address, uint8_t n) {
	if (current_address == 0) {
		return;
	}

	int32_t start_address = current_address;
	size_t counter = 0;

	BSP_QSPI_EnableMemoryMappedMode();

	while (--start_address > 0) {
		if (((uint8_t*)QSPI_FLASH_ADDRESS)[start_address] == '\n') {
			++counter;
			if (counter == n + 1) {
				++start_address;
				break;
			}
		}
	}

	HAL_UART_Transmit(&huart2, (uint8_t*)(QSPI_FLASH_ADDRESS + start_address), current_address - start_address, HAL_MAX_DELAY);
	HAL_QSPI_Abort(&hqspi);
}

void sendFirstNLogs(uint32_t current_address, uint8_t n) {
	if (current_address == 0 || n == 0) {
		return;
	}

	uint32_t end_address = 0;
	size_t counter = 0;

	BSP_QSPI_EnableMemoryMappedMode();

	while (++end_address < current_address) {
		if (((uint8_t*)QSPI_FLASH_ADDRESS)[end_address] == '\n') {
			++counter;
			if (counter == n) {
				++end_address;
				break;
			}
		}
	}

	HAL_UART_Transmit(&huart2, (uint8_t*)QSPI_FLASH_ADDRESS, end_address, HAL_MAX_DELAY);
	HAL_QSPI_Abort(&hqspi);
}

void sendLogsWithGivenTime(uint8_t* time_str, uint32_t current_address) {
	if (current_address == 0) {
		return;
	}

	const uint8_t time_str_length = USART_BUF_SIZE - 1;
	uint32_t begin_address = 0;
	uint32_t end_address = 0;

	BSP_QSPI_EnableMemoryMappedMode();

	while(begin_address < current_address) {
		if (memcmp((uint8_t*)(QSPI_FLASH_ADDRESS + begin_address), time_str, time_str_length) == 0) {
			end_address = begin_address;
			while (++end_address < current_address) {
				if (((uint8_t*)QSPI_FLASH_ADDRESS)[end_address] == '\n') {
					++end_address;
					break;
				}
			}
			HAL_UART_Transmit(&huart2, (uint8_t*)(QSPI_FLASH_ADDRESS + begin_address), end_address - begin_address, HAL_MAX_DELAY);
			begin_address = end_address;
		} else {
			++begin_address;
		}
	}

	HAL_QSPI_Abort(&hqspi);
}

void sendRequestedData(uint8_t* data_received, uint32_t current_address) {
	if (data_received[0] == first) {
		sendFirstNLogs(current_address, data_received[1]);
	} else if (data_received[0] == last) {
		sendLastNLogs(current_address, data_received[1]);
	} else if (data_received[0] == by_time) {
		sendLogsWithGivenTime(data_received + 1, current_address);
	}
}

void bytesToString(char* output, uint32_t bytes) {
	if (bytes >= 1024 * 1024) {
		sprintf(output, "%ldMB", bytes / (1024 * 1024));
	} else if (bytes >= 1024) {
		sprintf(output, "%ldKB", bytes / 1024);
	} else {
		sprintf(output, "%ldB", bytes);
	}
}
