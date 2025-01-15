#include "asm.h"
#include "i2c.h"
#include "shared_buf.h"
#include "algorithm.h"
#include "pindefs.h"

extern SharedBuffer mem_buf;
int counter;
std::vector<std::vector<int>> circuits;
int count;

uint8_t asm_run(uint8_t state){
    uint8_t change_state = STATE_SCAN;
    uint8_t scan_buf[MEM_BUF_SIZE];
    bool deleted = false;
    int short_circuit_index;
    

    for (int i = 0; i < MEM_BUF_SIZE; i++) {
        scan_buf[i] = 0;
    }
    //RUN LOCAL HERE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //add ADD_BATTERY to mem_buf with subsequent data


    mem_buf.write(0, 0, ADD_BATTERY);
    mem_buf.write(ADD_TYPE, 0, BATT_TYPE);
    mem_buf.write(ADD_LIVE_STATE, 0, 0);
    mem_buf.write(ADD_POS_NEIGHBOUR, 0, 10);
    uint8_t pos_neighbour = mem_buf.read(ADD_POS_NEIGHBOUR, 0);
    mem_buf.write(ADD_POS_NEIGHBOUR + 1, 0, 16);


    if(state == STATE_SCAN){
            i2c_scan(I2CINSTANCE, scan_buf);

            //print scan_buf
            // printf("Scan Buffer: \n");
            // for (int i = 0; i < MEM_BUF_SIZE; i++) {
            //     if(scan_buf[i] == 0){
            //         break;
            //     }
            //     printf(" %d\n", scan_buf[i]);
            // }

            //EMPTY
            if (scan_buf_empty(scan_buf)){
                printf("SCAN BUFFER IS EMPTY\n");
                mem_buf_zero();
                return STATE_SCAN;
            }
            
            deleted = mem_address_delete(scan_buf);
            if(deleted){
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
            
            //print out the mem_buf
            printf("Mem Buffer: \n");
            for (int i = 0; i < MEM_BUF_SIZE; i++){
                if(mem_buf.read(0, i) == 0){
                    break;
                }
                printf(" %d\n", mem_buf.read(0, i));
            }
            
            if(change_state != STATE_CHANGE){
                count++;
                if (count < 1){
                    change_state = STATE_NO_CHANGE;
                }
                else{
                    change_state = STATE_CHANGE;
                    count = 0;
                }
            }

            return change_state;
    }
    else if(state == STATE_CHANGE){
            find_neighbours();

            // if (counter < 8){
            //     return STATE_SCAN;
            // }

            //RUN ALGORITHM
            circuits.clear();
            run_algorithm(circuits);
            //print circuits
            printf("Circuits: \n");
            for (size_t i = 0; i < circuits.size(); i++) {
                printf("Circuit %d: \n", i);
                for (size_t j = 0; j < circuits[i].size(); j++) {
                    printf(" %d ", circuits[i][j]);
                }
                printf("\n");
            }

            //check if any circuits have been formed?
            if (circuits.empty()){
                printf("Circuits empty\n");
                clear_all_lights();
                return STATE_SCAN;
            }

            //check short circuit
            int short_circuit_index = check_short_circuit(circuits);
            //COMPARE WITH FAULT PIN IF ATCUALLY A SHORT CIRCUIT
            if (short_circuit_index != -1) {
                printf("SHORT CIRCUIT FOUND\n");
                printf("Short circuit found at index: %d\n", short_circuit_index);
                if (switch_check(circuits[short_circuit_index])){
                    clear_all_lights();
                    light_up_red(circuits[short_circuit_index]);
                    led_on(LED_1_R);
                    led_on(LED_2_R);
                }
                else{
                    clear_all_lights(); //NEEDS TO BE FIXED #################
                }
            }
            else{
                //RUN A CHECK ON ALL MEM_BUF MODULES WHICH ARE LIT UP AND CHECK IF THEY EXIST IN CIRCUITS
                //IF THEY DON'T, CLEAR THE LIGHT
                printf("Clearing live modules\n");
                clear_live_modules(circuits);
                printf("Orienting circuits\n");
                orient_circuits(circuits);
                //print circuits
                printf("Circuits: \n");
                for (size_t i = 0; i < circuits.size(); i++) {
                    printf("Circuit %d: \n", i);
                    for (size_t j = 0; j < circuits[i].size(); j++) {
                        printf(" %d ", circuits[i][j]);
                    }
                    printf("\n");
                }

                printf("Enabling circuits\n");
                enable_circuits(circuits);
            }
            return STATE_SCAN;
    }

    else if(state == STATE_NO_CHANGE){
            int short_circuit_index = check_short_circuit(circuits);
            //COMPARE WITH FAULT PIN IF ATCUALLY A SHORT CIRCUIT
            if (short_circuit_index != -1) {
                printf("SHORT CIRCUIT FOUND\n");
                printf("Short circuit found at index: %d\n", short_circuit_index);
                clear_all_lights();
                if (switch_check(circuits[short_circuit_index])){
                    light_up_red(circuits[short_circuit_index]);
                    led_on(LED_1_R);
                    led_on(LED_2_R);
                }
                else{
                    for (size_t i = 0; i < circuits[short_circuit_index].size() - 1; i++) {
                        uint8_t module_addr = circuits[short_circuit_index][i];
                        if (module_addr == ADD_BATTERY){
                        led_off(LED_1_B);
                        led_off(LED_1_R);
                        led_off(LED_2_B);
                        led_off(LED_2_R);
                        }
                        else{
                            printf("Writing OFF live state for: %d\n", module_addr);
                            write_live_state(I2CINSTANCE, module_addr, LED_OFF);
                            mem_buf.write(ADD_LIVE_STATE, module_addr, LED_OFF);
                        }
                    }
                }
            }
            else{
                check_switches(circuits);
            }
            
            return STATE_SCAN;
    }
    else{
        return 0;
    }
    return STATE_SCAN;
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
    for (size_t i = 1; i < MEM_BUF_SIZE; i++) { // 1 to ignore the battery module
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
            for(int k = 1; k < MEM_BUF_SIZE; k++){
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
            mem_buf.write(0, i, 0); //clear the address
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

    for (int i = 1; i < MEM_BUF_SIZE; i++){ // 1 to ignore the battery module
        if (mem_buf.read(0, i) == 0){
            break;
        }
        printf("Finding neighbours for module: %d\n", mem_buf.read(0, i));
        uint8_t buf[4];
        read_neighbours(I2CINSTANCE, mem_buf.read(0, i), buf);
        mem_buf.write(ADD_POS_NEIGHBOUR, i, buf[0]);
        mem_buf.write(ADD_POS_NEIGHBOUR + 1, i, buf[1]);
        mem_buf.write(ADD_POS_NEIGHBOUR + 2, i, buf[2]);
        mem_buf.write(ADD_POS_NEIGHBOUR + 3, i, buf[3]);
        printf("Neighbours: %d, %d, %d, %d\n", buf[0], buf[1], buf[2], buf[3]);
        mem_buf.write(ADD_NEIGHBOUR, i, 1);
        uint8_t module_type = read_type(I2CINSTANCE, mem_buf.read(0, i));
        printf("Module type: %d\n", module_type);
        mem_buf.write(ADD_TYPE, i, module_type);
    }
}