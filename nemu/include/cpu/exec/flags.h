#ifndef __FLAGS_H__
#define __FLAGS_H__

#include "common.h"

bool even_parity_b(uint8_t);
bool even_parity_w(uint16_t);
bool even_parity_l(uint32_t);

bool uadd_ok_b(uint8_t, uint8_t);
bool uadd_ok_w(uint16_t, uint16_t);
bool uadd_ok_l(uint32_t, uint32_t);

bool tadd_ok_b(uint8_t, uint8_t);
bool tadd_ok_w(uint16_t, uint16_t);
bool tadd_ok_l(uint32_t, uint32_t);

bool usub_ok_b(uint8_t, uint8_t);
bool usub_ok_w(uint16_t, uint16_t);
bool usub_ok_l(uint32_t, uint32_t);

bool tsub_ok_b(uint8_t, uint8_t);
bool tsub_ok_w(uint16_t, uint16_t);
bool tsub_ok_l(uint32_t, uint32_t);

#endif
