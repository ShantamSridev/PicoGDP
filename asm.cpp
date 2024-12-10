#include "asm.h"



uint8_t asm_select(void){
    uint8_t scan_buf[MEM_BUF_SIZE];
    i2c_scan(I2CINSTANCE, scan_buf);

    //EMPTY
    if (scan_buf_empty(scan_buf)){
        return STATE_EMPTY;
    }

    //CHANGE
    for (int i = 0; i < MEM_BUF_SIZE; i++){
        
    }

    //NO CHANGE

    //NO CHANGE LIVE

    return;
}

uint8_t asm_run(uint8_t state){
    switch (state)
    {
        case STATE_SCAN:
            uint8_t scan_buf[MEM_BUF_SIZE];
            i2c_scan(I2CINSTANCE, scan_buf);

            //EMPTY
            if (scan_buf_empty(scan_buf)){
                printf("Empty\n");
                return STATE_SCAN;
            }

            //CHANGE
            for (int i = 0; i < MEM_BUF_SIZE; i++){
                
            }

            //NO CHANGE

            //NO CHANGE LIVE
            break;
    }
}

bool scan_buf_empty(uint8_t *scan_buf) {
    for (size_t i = 0; i < MEM_BUF_SIZE; i++) {
        if (scan_buf[i] != 0) {
            return false; // Found a non-zero byte, scan_buf is not empty
        }
    }
    return true; // All bytes are zero, scan_buf is empty
}