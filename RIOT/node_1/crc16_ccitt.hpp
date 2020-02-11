#pragma once
#include <stdint.h>
#include <stddef.h>

uint16_t crc16_ccitt(const uint8_t *data, uint8_t data_len);
