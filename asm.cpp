#include "asm.h"
#include "i2c.h"
#include "shared_buf.h"


extern SharedBuffer mem_buf;
bool circuit_live = false;
int counter;

uint8_t asm_run(uint8_t state){
    uint8_t change_state = STATE_SCAN;
    uint8_t scan_buf[MEM_BUF_SIZE];
    for (int i = 0; i < MEM_BUF_SIZE; i++) {
        scan_buf[i] = 0;
    }
    
    switch (state){
        case STATE_SCAN:
        {

            i2c_scan(I2CINSTANCE, scan_buf);

            //EMPTY
            if (scan_buf_empty(scan_buf)){
                printf("SCAN BUFFER IS EMPTY\n");
                printf("Setting mem_buf to 0\n");
                for (size_t i = 0; i < MEM_BUF_SIZE; ++i) {
                    for (size_t j = 0; j < MEM_BUF_ROW_SIZE; ++j) {
                        mem_buf.write(i, j, 0);
                    }
                }
                change_state = STATE_SCAN;
                return change_state;
            }
            
            //REMOVE
            bool deleted = mem_address_delete(scan_buf);
            if(deleted == true){
                printf("Deleted\n");
                if (circuit_live){
                    change_state = STATE_CHANGE_LIVE;
                }
                else{
                    change_state = STATE_CHANGE;
                }
            }


            //ADD
            for (int i = 0; i < MEM_BUF_SIZE; i++){
                uint8_t address = scan_buf[i];
                printf("Address: %d\n", address);
                if(scan_buf[i] == 0){
                    break;
                }
                if(!address_exists(address)){
                    if (circuit_live){
                        change_state = STATE_CHANGE_LIVE;
                    }
                    else{
                        change_state = STATE_CHANGE;
                    }

                    int j = 0;
                    while(mem_buf.read(0, j) != 0){
                        j++;
                    }
                    mem_buf.write(0, j, address);
                    printf("Added: %d\n", address);
                }
            }

            counter = device_count();
            printf("Device Count: %d\n", counter);
            
            //print out the mem_buf
            printf("Mem Buffer: \n");
            for (int i = 0; i < MEM_BUF_SIZE; i++){
                if(mem_buf.read(0, i) == 0){
                    break;
                }
                printf(" %d\n", mem_buf.read(0, i));
            }

            if(change_state != STATE_CHANGE){
                change_state = STATE_NO_CHANGE;
            }

            return change_state;
        }

        case STATE_CHANGE:
            return STATE_SCAN;

        case STATE_NO_CHANGE:
            return STATE_SCAN;

        case STATE_CHANGE_LIVE:
            return STATE_SCAN;

        default:
            return change_state;
    }
    return change_state;
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
            mem_buf.write(0, i, 0); //shift all elements to the left
            //REMOVE THIS device column LINE ALLTOGETHER, DONT LEAVE IT IN as 0
            del = true;
        }
    }
    return del;
}

int device_count(){
    for (int i = 0; i < MEM_BUF_SIZE; i++){
        if (mem_buf.read(0, i) == 0){
            return i;
        }
    }
    return MEM_BUF_SIZE;
}