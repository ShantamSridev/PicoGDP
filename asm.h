#ifndef ASM_H
#define ASM_H


#include "pindefs.h"
#include "i2c.h"

extern bool circuit_live;

//uint8_t asm_select(void);

/*! \brief Run the assembly state machine
 *  \ingroup asm.h
 *
 * \param state Current state of the state machine
 * \return Next state of the state machine
 */
uint8_t asm_run(uint8_t state);

/*! \brief Check if scan buffer is empty
 *  \ingroup asm.h
 *
 * \param scan_buf Pointer to scan buffer to check
 * \return true if buffer is empty, false otherwise
 */
bool scan_buf_empty(uint8_t *scan_buf);

/*! \brief Check if address exists in memory buffer
 *  \ingroup asm.h
 *
 * \param address Address to check for
 * \return true if address exists, false otherwise
 */
bool address_exists(uint8_t address);

/*! \brief Delete address from memory buffer if not in scan buffer
 *  \ingroup asm.h
 *
 * \param scan_buf Pointer to scan buffer to compare against
 * \return true if address was deleted, false otherwise
 */
bool mem_address_delete(uint8_t *scan_buf);

/*! \brief Get the number of devices in the memory buffer
 *  \ingroup asm.h
 *
 * \return Number of devices in the memory buffer
 */
int device_count();

#endif