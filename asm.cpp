#include "asm.h"
#include "i2c.h"
#include "shared_buf.h"


extern SharedBuffer mem_buf;
bool circuit_live = false;

uint8_t asm_run(uint8_t state){
    uint8_t change_state = STATE_SCAN;
    switch (state){
        case STATE_SCAN:
            uint8_t scan_buf[MEM_BUF_SIZE];
            i2c_scan(I2CINSTANCE, scan_buf);

            //EMPTY
            if (scan_buf_empty(scan_buf)){
                //printf("Empty\n");
                change_state = STATE_SCAN;
                return change_state;
            }

            //REMOVE
            for (int i = 0; i < MEM_BUF_SIZE; i++){
                uint8_t address = scan_buf[i];
                bool deleted = mem_address_delete(scan_buf);
                if(deleted == true){
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

            
            return change_state;
            break;
            
        //case STATE_CHANGE:

        default:
            return change_state;
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
        if (mem_buf.read(0, i) ==  address) {
            return true; // Address found
        }
    }
    return false; // Address not found
}

bool mem_address_delete(uint8_t *scan_buf) {
    bool del = false;
    for (size_t i = 0; i < MEM_BUF_SIZE; i++) {
        bool found = false;
        if(scan_buf[i] == 0){
            return del;
        }
        

        // Check if address is in mem_buf
        for (size_t j = 0; j < MEM_BUF_SIZE; j++) {
            if (address_exists(scan_buf[i]) || mem_buf.read(0, j) == 0) {
                found = true;
                break;
            }
        }

        // Address not found in scan_buf, delete it from mem_buf
        if (!found) { 
            for(int k = 0; k < MEM_BUF_SIZE; k++){
                if(mem_buf.read(0,k) == 0){
                    break;
                }
                else{
                    for(int l = ADD_POS_NEIGHBOUR; l < ADD_POS_NEIGHBOUR+4; l++){
                        if(mem_buf.read(l,k) == mem_buf.read(0,i)){
                            mem_buf.write(ADD_NEIGHBOUR, k, 0); //Neighbour now needs to be read
                            mem_buf.write(ADD_POS_NEIGHBOUR, k, 0);
                            mem_buf.write(ADD_POS_NEIGHBOUR+1, k, 0);
                            mem_buf.write(ADD_POS_NEIGHBOUR+2, k, 0);
                            mem_buf.write(ADD_POS_NEIGHBOUR+3, k, 0);
                            break;
                        }
                    }
                }
            }
            mem_buf.write(0, i, 0);
            //REMOVE THIS LINE ALLTOGETHER, DONT LEAVE IT IN as 0
            del = true;
        }
    }
    return del;
}