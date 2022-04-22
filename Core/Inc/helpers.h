#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "main.h"

enum output_option {
	first = 0,
	last = 1,
	by_time = 2
};

void clearFlashAndResetAddress(uint32_t* address);
void sendLastNLogs(uint32_t current_address, uint8_t n);
void sendFirstNLogs(uint32_t current_address, uint8_t n);
void sendNLogs(uint8_t out_option, uint32_t current_address, uint8_t n);
void sendLogsWithGivenTime(uint8_t* time_str, uint32_t current_address);
void sendRequestedData(uint8_t* data_received, uint32_t current_address);
void bytesToString(char* output, uint32_t bytes);

#endif
