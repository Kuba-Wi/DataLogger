#include "helpers.h"
#include "quadspi.h"
#include "usart.h"

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

	BSP_QSPI_EnableMemoryMappedMode();

	int32_t start_address = current_address;
	size_t counter = 0;
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