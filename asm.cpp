#include "asm.h"
#include "i2c.h"
#include "shared_buf.h"

uint8_t prev_buf[MEM_BUF_SIZE];
bool circuit_live = false;
extern SharedBuffer mem_buf;

uint8_t asm_run(uint8_t state){
    uint8_t change_state = STATE_SCAN;
    switch (state)
    {
        case STATE_SCAN:
            uint8_t scan_buf[MEM_BUF_SIZE];
            i2c_scan(I2CINSTANCE, scan_buf);

            //EMPTY
            if (scan_buf_empty(scan_buf)){
                printf("Empty\n");
                change_state = STATE_SCAN;
                return change_state;
            }

            //REMOVE
            for (int i = 0; i < MEM_BUF_SIZE; i++){
                uint8_t address = {scan_buf[i]};
                bool deleted = mem_address_delete(scan_buf);
                if(deleted){
                    change_state = STATE_CHANGE;
                }
            }

            //ADD
            for (int i = 0; i < MEM_BUF_SIZE; i++){
                uint8_t address = {scan_buf[i]};
                if(!address_exists(address)){
                    change_state = STATE_CHANGE;
                    int j = 0;
                    while(mem_buf.read(0, j) != 0){
                        j++;
                    }
                    mem_buf.write(0, j, address);
                }
            }

            if(change_state != STATE_CHANGE){
                if (circuit_live){
                    change_state = STATE_NO_CHANGE_LIVE;
                }
                else{
                    change_state = STATE_NO_CHANGE;
                }
            }

            
            //mem_buf = scan_buf;
            std::fill_n(prev_buf, MEM_BUF_SIZE, 0);
            for (int i = 0; i < MEM_BUF_SIZE; i++){
                prev_buf[i] = mem_buf.read(0, i);
            }

            return change_state;
            break;
        
        case STATE_CHANGE:


        
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

bool address_exists(uint8_t address) {
    for (size_t i = 0; i < MEM_BUF_SIZE; i++) {
        if (prev_buf[i] ==  address) {
            return true; // Address found
        }
    }
    return false; // Address not found
}

bool mem_address_delete(uint8_t *scan_buf) {
    bool del = false;
    for (size_t i = 0; i < MEM_BUF_SIZE; i++) {
        if(mem_buf.read(0, i) == 0){
            if (del){
                return true;
            }
            else{
                return false;
            }
        }
        bool found = false;
        // Address not found in scan_buf, delete it from mem_buf
        for (size_t j = 0; j < MEM_BUF_SIZE; j++) {
            if (prev_buf[j] != 0){
                break;
            }
            if (mem_buf.read(0, i) == prev_buf[j]) {
                found = true;
            }
            
        }
        if (!found) {
            for(int k = 0; k < MEM_BUF_SIZE; k++){
                if(mem_buf.read(0,k) == 0){
                    break;
                }
                else{
                    for(int l = MEM_BUF_NEIGHBOUR_START; l < MEM_BUF_NEIGHBOUR_START+4; l++){
                        if(mem_buf.read(l,k) == mem_buf.read(0,i)){
                            mem_buf.write(l, k, 0);
                            break;
                        }
                    }
                }
                
            }
            mem_buf.write(0, i, 0);
            del = true;
        }
    }
}

//compare scan_buf with prev_buf
//if there is a change then check the type of the address and update the mem_buf