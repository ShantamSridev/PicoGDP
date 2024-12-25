#ifndef SHARED_BUF_H
#define SHARED_BUF_H


#include "pindefs.h"

class SharedBuffer {
public:
    /*! \brief Constructor for SharedBuffer class
     *  \ingroup shared_buf.h
     */
    SharedBuffer();

    /*! \brief Write a value to the buffer at specified indices
     *  \ingroup shared_buf.h
     *
     * \param index1 Row index into buffer
     * \param index2 Column index into buffer 
     * \param value Value to write
     */
    void write(size_t index1, size_t index2, uint8_t value);

    /*! \brief Read a value from the buffer at specified indices
     *  \ingroup shared_buf.h
     * 
     * \param index1 Row index into buffer
     * \param index2 Column index into buffer
     * \return Value read from buffer
     */
    uint8_t read(size_t index1, size_t index2);
    
private:
    uint8_t buf[MEM_BUF_SIZE][MEM_BUF_ROW_SIZE];
    //std::array<uint8_t, 128> buffer_;
};

#endif