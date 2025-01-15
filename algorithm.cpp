#include "algorithm.h"

void buildAdjacencyList(std::unordered_map<int, std::vector<int>> &adjList) {
    for (size_t i = 0; i < MEM_BUF_SIZE; ++i) {
        int moduleAddress = mem_buf.read(0, i);
        std::vector<int> neighbors;
        for (size_t j = ADD_POS_NEIGHBOUR; j < ADD_ACTIVE; ++j) {
            int neighbor = mem_buf.read(j, i);
            if (neighbor != 0) {
                neighbors.push_back(neighbor);
            }
        }
        adjList[moduleAddress] = neighbors;
    }
}

void dfs(int current, int start, std::vector<int> &path, std::unordered_set<int> &visited, 
         std::unordered_map<int, std::vector<int>> &adjList, std::vector<std::vector<int>> &circuits) {
    visited.insert(current);
    path.push_back(current);

    for (int neighbor : adjList[current]) {
        if (neighbor == start && path.size() > 1) {
            std::vector<int> circuit = path;
            circuit.push_back(start);
            circuits.push_back(circuit);
        } else if (visited.find(neighbor) == visited.end()) {
            dfs(neighbor, start, path, visited, adjList, circuits);
        }
    }

    path.pop_back();
    visited.erase(current);
}

void run_algorithm(std::vector<std::vector<int>> &circuits) {
    std::unordered_map<int, std::vector<int>> adjList;
    buildAdjacencyList(adjList);
    std::unordered_set<int> visited;
    std::vector<int> path;

    dfs(ADD_BATTERY, ADD_BATTERY, path, visited, adjList, circuits);

    //minimum possible amount of modules in a circuit should be 8 but we want to be safe
    //so we filter out circuits with less than 4 modules
    filterCircuits(circuits, 4);

}

void filterCircuits(std::vector<std::vector<int>> &circuits, size_t minLength) {
    circuits.erase(
        std::remove_if(circuits.begin(), circuits.end(),
                       [minLength](const std::vector<int> &circuit) {
                           // Exclude the first and last module (which are the same) from the count
                           return (circuit.size() - 1) < minLength;
                       }),
        circuits.end());
}

//check short circuit
int check_short_circuit(std::vector<std::vector<int>> &circuits) {
    //check if any of the circuits are short circuited
    for (size_t circuit_idx = 0; circuit_idx < circuits.size(); circuit_idx++) {
        bool found_load_module = false;
        
        // Check each module in the circuit except last (duplicate of first)
        for (size_t i = 0; i < circuits[circuit_idx].size() - 1; i++) {
            uint8_t module_addr = circuits[circuit_idx][i];
            for (size_t j = 0; j < MEM_BUF_SIZE; j++) {
                if (mem_buf.read(0, j) == module_addr) {
                    uint8_t module_type = mem_buf.read(ADD_TYPE, j);
                    printf("Module type: %d\n", module_type);
                    
                    // Check if module is not a battery, wire or node
                    if (module_type == MOTOR_TYPE || 
                        module_type == LED_TYPE || 
                        module_type == BUZZER_TYPE) {
                        printf("Found load module: %d\n", module_addr);
                        found_load_module = true;
                    }
                    break;
                }
            }
            if(found_load_module){
                break;
            }
        }
        
        // If no active module found in this circuit, return its index
        if (!found_load_module) {
            return circuit_idx;
        }
    }
    return -1; // No short circuit found
}

bool enable_circuits(std::vector<std::vector<int>> &circuits) {
    bool circuit_live = false;
    for (size_t circuit_idx = 0; circuit_idx < circuits.size(); circuit_idx++) {
        bool polarisation_found = false;
        uint8_t prev_module = 0;
        for (size_t i = 0; i < circuits[circuit_idx].size() - 1; i++) {
            uint8_t module_addr = circuits[circuit_idx][i];
            //find address in mem_buf
            if (module_addr != 1){
                for (size_t j = 0; j < MEM_BUF_SIZE; j++) {
                    if (mem_buf.read(0, j) == module_addr) {
                        if ( mem_buf.read(ADD_TYPE, j) >= 8) {
                            //chekc polarisation#######################################
                            //give positive neighbour and then read polarisation state
                            printf("Writing positive neighbour: %d\n", prev_module);
                            write_positive_neighbour(I2CINSTANCE, module_addr, prev_module);
                            printf("Reading polarisation for: %d\n", module_addr);
                            uint8_t polarisation = read_active_state(I2CINSTANCE, module_addr);
                            printf("Polarisation: %d\n", polarisation);
                            if (polarisation == 1){
                                printf("Polarisation found for: %d\n", module_addr);
                                polarisation_found = true;

                                mem_buf.write(ADD_LIVE_STATE, j, BLINK_RED);
                                write_live_state(I2CINSTANCE, module_addr, BLINK_RED);
                            }
                        }
                    }
                }
            }
            prev_module = module_addr;
            
        }
        if (!polarisation_found) {
            if (!switch_check(circuits[circuit_idx])){
                printf("Polarisation not found\n");
                uint8_t pos_neighbour = 1;
                for (size_t i = 1; i < circuits[circuit_idx].size() - 1; i++) {
                    uint8_t module_addr = circuits[circuit_idx][i];
                    //find address in mem_buf
                    for (size_t j = 0; j < MEM_BUF_SIZE; j++) {
                        if (mem_buf.read(0, j) == module_addr) {
                            write_positive_neighbour(I2CINSTANCE, module_addr, pos_neighbour);
                            if (mem_buf.read(ADD_TYPE, j) == NODE_TYPE){
                                printf("Writing negative neighbour: %d\n", circuits[circuit_idx][i+1]);
                                write_negative_neighbour(I2CINSTANCE, module_addr, circuits[circuit_idx][i+1]);
                            }
                            printf("Writing live state: %d\n", module_addr);
                            write_live_state(I2CINSTANCE, module_addr, FLOW_BLUE);
                            pos_neighbour = module_addr;
                            circuit_live = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    return circuit_live;
}


void check_switches(std::vector<std::vector<int>> &circuits) {
    for (size_t circuit_idx = 0; circuit_idx < circuits.size(); circuit_idx++) {
        bool loop_on = false;
        for (size_t i = 0; i < circuits[circuit_idx].size() - 1; i++) {
            uint8_t module_addr = circuits[circuit_idx][i];
            for (size_t j = 0; j < MEM_BUF_SIZE; j++) {
                if (mem_buf.read(0, j) == 0){
                    break;
                }   
                if ((mem_buf.read(0, j) == module_addr)&& (mem_buf.read(ADD_TYPE, j) == BUTTON_TYPE || mem_buf.read(ADD_TYPE, j) == SENSOR_TYPE)){
                    printf("Reading active state for: %d\n", module_addr);
                    uint8_t active_state = read_active_state(I2CINSTANCE, module_addr);
                    mem_buf.write(ADD_ACTIVE, j, active_state);
                    if (active_state == 1){
                        loop_on = true;
                        printf("Switch on\n");
                    }
                    else{
                        loop_on = false;
                        printf("Switch off\n");
                    }
                }
            }
        }
        if (loop_on){
            for (size_t i = 0; i < circuits[circuit_idx].size() - 1; i++) {
                uint8_t module_addr = circuits[circuit_idx][i];
                if (module_addr == ADD_BATTERY){
                    led_on(LED_1_B);
                    led_on(LED_2_B);
                }
                else{
                    printf("Writing ON live state for: %d\n", module_addr);
                    write_live_state(I2CINSTANCE, module_addr, FLOW_BLUE);
                    mem_buf.write(ADD_LIVE_STATE, module_addr, FLOW_BLUE);
                }
            }
        }
        else{
            for (size_t i = 0; i < circuits[circuit_idx].size() - 1; i++) {
                uint8_t module_addr = circuits[circuit_idx][i];
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
}

bool switch_check(std::vector<int> &circuit) {
    bool loop_on = false;
    for (size_t i = 0; i < circuit.size() - 1; i++) {
        uint8_t module_addr = circuit[i];
        for (size_t j = 0; j < MEM_BUF_SIZE; j++) {
            if (mem_buf.read(0, j) == 0){
                break;
            }   
            if ((mem_buf.read(0, j) == module_addr)){
                if (mem_buf.read(ADD_TYPE, j) == BUTTON_TYPE || mem_buf.read(ADD_TYPE, j) == SENSOR_TYPE){
                    printf("Reading active state for: %d\n", module_addr);
                    uint8_t active_state = read_active_state(I2CINSTANCE, module_addr);
                    mem_buf.write(ADD_ACTIVE, j, active_state);
                    if (active_state == 1){
                        printf("Switch on\n");
                        loop_on = true;
                    }
                    else{
                        printf("Switch off\n");
                        loop_on = false;
                    }
                }
            }
        }
    }
    return loop_on;
}

void light_up_red(std::vector<int> &circuit) {
    for (size_t i = 0; i < circuit.size() - 1; i++) {
        uint8_t module_addr = circuit[i];
        printf("Lighting up red: %d\n", module_addr);
        if (module_addr == ADD_BATTERY){
            led_on(LED_1_R);
            led_on(LED_2_R);
        }
        else{
            write_live_state(I2CINSTANCE, module_addr, BLINK_RED);
            mem_buf.write(ADD_LIVE_STATE, i, BLINK_RED);
        }
    }
}

void clear_all_lights() {
    for (size_t i = 0; i < MEM_BUF_SIZE; i++) {
        if (mem_buf.read(0, i) == 0){
            break;
        }
        printf("Clearing: %d\n", mem_buf.read(0, i));
        if (mem_buf.read(0, i) == ADD_BATTERY){
            led_off(LED_1_B);
            led_off(LED_2_B);
        }
        else{
            write_live_state(I2CINSTANCE, mem_buf.read(0, i), LED_OFF);
            mem_buf.write(ADD_LIVE_STATE, i, LED_OFF);
        }
    }
}

void clear_live_modules(std::vector<std::vector<int>> &circuits) {
    for (size_t i = 0; i < MEM_BUF_SIZE; i++) {
        int p = i;
        bool found = false;
        if (mem_buf.read(0, i) == 0){
            break;
        }
        if (mem_buf.read(ADD_LIVE_STATE, i) != 0){
            //find address in circuits
            for (size_t j = 0; j < circuits.size(); j++) {
                if (std::find(circuits[j].begin(), circuits[j].end(), mem_buf.read(0, i)) != circuits[j].end()) {
                    //address found in circuit
                    found = true;
                    break;
                }
            }
            if (!found){
                if (mem_buf.read(0, i) == ADD_BATTERY){ 
                    led_off(LED_1_B);
                    led_off(LED_2_B);
                }
                else{
                    write_live_state(I2CINSTANCE, mem_buf.read(0, i), LED_OFF);
                    mem_buf.write(ADD_LIVE_STATE, p, LED_OFF);
                }
            }
        }
    }
}

void orient_circuits(std::vector<std::vector<int>> &circuits) {
    //check if circuits are oriented correctly
    //if pos_neighbour is not the second element of the circuit, reverse the circuit
    for (size_t i = 0; i < circuits.size(); i++) {
        if (circuits[i][1] != mem_buf.read(ADD_POS_NEIGHBOUR, 0)) {
            std::reverse(circuits[i].begin(), circuits[i].end());
        }
    }
}
