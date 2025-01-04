#ifndef I2C_H
#define I2C_H


#include "pindefs.h"

void i2c_start_init(i2c_inst_t *);

bool reserved_addr(uint8_t);

/*! \brief  Scan I2C bus for devices
 *  \ingroup i2c.h
 *
 * \param i2c Either \ref i2c0 or \ref i2c1
 * \param buf Pointer to buffer to receive addresses of devices found
 */
void i2c_scan(i2c_inst_t *, uint8_t *);

/*! \brief  Attempt to read specified number of bytes from address, blocking
 *  \ingroup i2c.h
 *
 * \param i2c Either \ref i2c0 or \ref i2c1
 * \param addr 7-bit address of device to read from
 * \param internal_address Address of internal register to read from
 * \param buf Pointer to buffer to receive data
 * \param len Length of data in bytes to receive
 */
void i2c_read(i2c_inst_t *, uint8_t, uint8_t, uint8_t *, size_t);

/*! \brief  Attempt to write specified number of bytes to address, blocking
 *  \ingroup i2c.h
 *
 * \param i2c Either \ref i2c0 or \ref i2c1
 * \param addr 7-bit address of device to write to
 * \param internal_address Address of internal register to write to
 * \param data Data to write
 * \param len Length of data in bytes to write
 */
void i2c_write(i2c_inst_t *, uint8_t, uint8_t, uint8_t, size_t);

/*! \brief  Read type of device at address
 *  \ingroup i2c.h
 *
 * \param i2c Either \ref i2c0 or \ref i2c1
 * \param addr 7-bit address of device to read from
 * \return Type of device at address
 */
uint8_t read_type(i2c_inst_t *i2c, uint8_t addr);

/*! \brief  Read neighbours of device at address
 *  \ingroup i2c.h
 *
 * \param i2c Either \ref i2c0 or \ref i2c1
 * \param addr 7-bit address of device to read from
 * \param buf Pointer to buffer to receive neighbours
 */
void read_neighbours(i2c_inst_t *i2c, uint8_t addr, uint8_t *buf);

/*! \brief  Read active state of device at address
 *  \ingroup i2c.h
 *
 * \param i2c Either \ref i2c0 or \ref i2c1
 * \param addr 7-bit address of device to read from
 * \return Active state of device
 */
uint8_t read_active(i2c_inst_t *i2c, uint8_t addr);

#endif