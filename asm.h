#ifndef ASM_H
#define ASM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pindefs.h"
#include "i2c.h"

uint8_t prev_buf[MEM_BUF_SIZE];
bool circuit_live;

uint8_t asm_select(void);

uint8_t asm_run(uint8_t state);

bool scan_buf_empty(uint8_t *scan_buf);

#ifdef __cplusplus
}
#endif
#endif