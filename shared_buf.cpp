#include "shared_buf.h"
#include <stdexcept>

SharedBuffer::SharedBuffer() {
    for (size_t i = 0; i < MEM_BUF_SIZE; ++i) {
        for (size_t j = 0; j < MEM_BUF_ROW_SIZE; ++j) {
            buf[i][j] = 0;
        }
    }
}

void SharedBuffer::write(size_t index1,size_t index2, uint8_t value) {
    //std::lock_guard<std::mutex> lock(mutex_);
    if (index1 >= sizeof(buf[0])) {
        //throw std::out_of_range("Index out of bounds");
    }
    buf[index1][index2] = value;
}

uint8_t SharedBuffer::read(size_t index1,size_t index2) {
    //std::lock_guard<std::mutex> lock(mutex_);
    if (index1 >= sizeof(buf[0])) {
        //throw std::out_of_range("Index out of bounds");
    }
    return buf[index1][index2];
}

//its column number first and then row number
//so to access address its [0][addr]


//ADD MULTICORE MUTEX LOCKS PICO