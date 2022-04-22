#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "main.h"

enum logs_area {
	begin = 0,
	end = 1
};

void clearFlashAndResetAddress(uint32_t* address);
void sendLastNLogs(uint32_t current_address, uint8_t n);
void sendFirstNLogs(uint32_t current_address, uint8_t n);
void sendNLogs(uint8_t log_area, uint32_t current_address, uint8_t n);
void bytesToString(char* output, uint32_t bytes);

#endif
