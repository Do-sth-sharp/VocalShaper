#pragma once

#include <stdint.h>

extern size_t getRecoverySize();
extern char* getRecoveryData();
extern size_t copyRecoveryData(char* dst, size_t size);
