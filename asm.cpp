#include "asm.h"
#include "i2c.h"
#include "shared_buf.h"
#include "algorithm.h"

extern SharedBuffer mem_buf;
int counter;
std::vector<std::vector<int>> circuits;

uint8_t asm_run(uint8_t state){
    uint8_t change_state = STATE_SCAN;
    uint8_t scan_buf[MEM_BUF_SIZE];
    
    for (int i = 0; i < MEM_BUF_SIZE; i++) {
        scan_buf[i] = 0;
    }
    
    //RUN LOCAL HERE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //add ADD_BATTERY to mem_buf with subsequent data
    mem_buf.write(0, 0, ADD_BATTERY);
    mem_buf.write(ADD_TYPE, 0, BATT_TYPE);
    mem_buf.write(ADD_LIVE_STATE, 0, 0);

    switch (state){
        case STATE_SCAN:
        {

            i2c_scan(I2CINSTANCE, scan_buf);

            //EMPTY
            if (scan_buf_empty(scan_buf)){
                printf("SCAN BUFFER IS EMPTY\n");
                mem_buf_zero();
                change_state = STATE_SCAN;
                return change_state;
            }
            
            //REMOVE
            bool deleted = mem_address_delete(scan_buf);
            if(deleted == true){
                printf("Deleted\n");
                change_state = STATE_CHANGE;
            }

            //ADD
            for (int i = 0; i < MEM_BUF_SIZE; i++){
                uint8_t address = scan_buf[i];
                if(scan_buf[i] == 0){
                    break;
                }
                if(!address_exists(address)){
                    change_state = STATE_CHANGE;
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
            
            // //print out the mem_buf
            // printf("Mem Buffer: \n");
            // for (int i = 0; i < MEM_BUF_SIZE; i++){
            //     if(mem_buf.read(0, i) == 0){
            //         break;
            //     printf(" %d\n", mem_buf.read(0, i));
            // }
            
            if(change_state != STATE_CHANGE){
                change_state = STATE_NO_CHANGE;
            }

            return change_state;
        }

        case STATE_CHANGE:
            find_neighbours();

            // if (counter < 8){
            //     return STATE_SCAN;
            // }

            //RUN ALGORITHM
            circuits.clear();
            run_algorithm(circuits);

            //check if any circuits have been formed?
            if (circuits.empty()){
                //CLEAR ALL LIGHTS%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                return STATE_SCAN;
            }

            //check short circuit
            int short_circuit_index = check_short_circuit(circuits);
            //COMPARE WITH FAULT PIN IF ATCUALLY A SHORT CIRCUIT
            if (short_circuit_index != -1) {
                printf("Short circuit found at index: %d\n", short_circuit_index);
                //CLEAR ALL LIGHTS%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                //LIGHT UP RED LED ON THAT CIRCUIT
            }
            else{
                //RUN A CHECK ON ALL MEM_BUF MODULES WHICH ARE LIT UP AND CHECK IF THEY EXIST IN CIRCUITS
                //IF THEY DON'T, CLEAR THE LIGHT%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                enable_circuits(circuits);
            }
            return STATE_SCAN;

        case STATE_NO_CHANGE:
            //RUN ACTIVE CHECK FOR SWITCH AND SENSOR to update led live%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            //Run 'circuits' in loop to find active modules, and their active states.
            enable_circuits(circuits);
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
        if(mem_buf.read(0, i) == 0){
            return false;
        }
    }
    return false; // Address not found
}

bool address_exists_scan_buf(uint8_t *scan_buf, uint8_t address) {
    for (size_t i = 0; i < MEM_BUF_SIZE; i++) {
        if (scan_buf[i] ==  address) {
            return true; // Address found
        }
        if(scan_buf[i] == 0){
            return false;
        }
    }
    return false; // Address not found
}


bool mem_address_delete(uint8_t *scan_buf) {
    bool del = false;
    for (size_t i = 0; i < MEM_BUF_SIZE; i++) {
        bool found = false;
        if(mem_buf.read(0,i) == 0){
            return del;
        }

        // Check if address is in mem_buf
        if (address_exists_scan_buf(scan_buf, mem_buf.read(0, i))) {
            found = true;
        }


        // Address not found in scan_buf, delete it from mem_buf
        if (!found) {
            printf("ADDRESS NOT FOUND: %d\n", mem_buf.read(0, i));
            for(int k = 0; k < MEM_BUF_SIZE; k++){
                if(mem_buf.read(0,k) == 0){
                    break;
                }
                else{ //checking if the address is in the neighbours
                    for(int l = ADD_POS_NEIGHBOUR; l < ADD_ACTIVE; l++){
                        if(mem_buf.read(l,k) == mem_buf.read(0,i)){
                            for(int m = ADD_NEIGHBOUR; m < ADD_ACTIVE; m++){
                                mem_buf.write(m, k, 0);
                            }
                            break;
                        }
                    }
                }
            }

            column_shift(i);
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

void mem_buf_zero(){
    for (size_t i = 0; i < MEM_BUF_SIZE; ++i) {
        for (size_t j = 0; j < MEM_BUF_ROW_SIZE; ++j) {
            mem_buf.write(i, j, 0);
        }
    }
}

void column_shift(int i){
    // Shift all columns left to remove the deleted address
    for (size_t col = i; col < MEM_BUF_SIZE - 1; col++) {
        if (mem_buf.read(0, col + 1) == 0) { // Stop at first empty column
            break;
        }
        // Copy next column to current column
        for (size_t row = 0; row < MEM_BUF_ROW_SIZE; row++) {
            mem_buf.write(row, col, mem_buf.read(row, col + 1));
        }
        // Zero out the next column after copying
        for (size_t row = 0; row < MEM_BUF_ROW_SIZE; row++) {
            mem_buf.write(row, col + 1, 0);
        }
    }
}

void find_neighbours() {

    for (int i = 1; i < MEM_BUF_SIZE; i++){
        if (mem_buf.read(0, i) == 0){
            break;
        }
        if(mem_buf.read(ADD_NEIGHBOUR, i) == 0){
            uint8_t buf[4];
            read_neighbours(I2CINSTANCE, mem_buf.read(0, i), buf);
            mem_buf.write(ADD_POS_NEIGHBOUR, i, buf[0]);
            mem_buf.write(ADD_POS_NEIGHBOUR + 1, i, buf[1]);
            mem_buf.write(ADD_POS_NEIGHBOUR + 2, i, buf[2]);
            mem_buf.write(ADD_POS_NEIGHBOUR + 3, i, buf[3]);
            mem_buf.write(ADD_NEIGHBOUR, i, 1);
            mem_buf.write(ADD_TYPE, i, read_type(I2CINSTANCE, mem_buf.read(0, i)));
        }
    }
}