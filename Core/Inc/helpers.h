#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "main.h"

void clearFlashAndResetAddress(uint32_t* address);
void sendLastNLogs(uint32_t current_address, uint8_t n);
void bytesToString(char* output, uint32_t bytes);

#endif
