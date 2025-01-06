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
            uint8_t module_type = mem_buf.read(ADD_TYPE, module_addr);
            
            // Check if module is not a battery, wire or node
            if (module_type == MOTOR_TYPE || 
                module_type == LED_TYPE || 
                module_type == BUZZER_TYPE) {
                found_load_module = true;
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

void enable_circuits(std::vector<std::vector<int>> &circuits) {
    for (size_t circuit_idx = 0; circuit_idx < circuits.size(); circuit_idx++) {
        bool polarisation_found = false;
        for (size_t i = 0; i < circuits[circuit_idx].size() - 1; i++) {
            uint8_t module_addr = circuits[circuit_idx][i];
            //find address in mem_buf
            for (size_t j = 0; j < MEM_BUF_SIZE; j++) {
                if (mem_buf.read(0, j) == module_addr) {
                    if ( mem_buf.read(ADD_TYPE, j) > 8) {
                        //chekc polarisation
                        mem_buf.write(ADD_LIVE_STATE, j, 1);
                    }
                }
            }
        }
        if (!polarisation_found) {
            uint8_t pos_neighbour = 1;
            for (size_t i = 1; i < circuits[circuit_idx].size() - 1; i++) {
                uint8_t module_addr = circuits[circuit_idx][i];
                //find address in mem_buf
                for (size_t j = 0; j < MEM_BUF_SIZE; j++) {
                    if (mem_buf.read(0, j) == module_addr) {
                        write_positive_neighbour(I2CINSTANCE, module_addr, pos_neighbour);
                        write_live_state(I2CINSTANCE, module_addr, FLOW_BLUE);
                        mem_buf.write(ADD_LIVE_STATE, j, FLOW_BLUE);
                        pos_neighbour = module_addr;
                        break;
                    }
                }
            }
        }
    }
}

bool check_active_state(uint8_t address) {
    uint8_t module_type = mem_buf.read(ADD_TYPE, address);
    if (module_type == BUTTON_TYPE || module_type == SENSOR_TYPE) {
        return true;
    }
    return false;
}

