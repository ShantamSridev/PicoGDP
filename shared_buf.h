#ifndef SHARED_BUF_H
#define SHARED_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pindefs.h"

class SharedBuffer {
public:
    SharedBuffer();
    void write(size_t index1, size_t index2, uint8_t value);
    uint8_t read(size_t index1, size_t index2);
    
private:
    uint8_t buf[MEM_BUF_SIZE][MEM_BUF_ROW_SIZE];
    //std::array<uint8_t, 128> buffer_;
};


#ifdef __cplusplus
}
#endif
#endif