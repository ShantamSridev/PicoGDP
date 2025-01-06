#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "pindefs.h"
#include "i2c.h"
#include "shared_buf.h"

extern SharedBuffer mem_buf;  // Declare external reference to mem_buf

/*! \brief Build adjacency list from memory buffer
 *  \ingroup algorithm.h
 *
 * \param adjList Reference to adjacency list
 */
void buildAdjacencyList(std::unordered_map<int, std::vector<int>> &adjList);

/*! \brief Depth-first search to find circuits
 *  \ingroup algorithm.h
 *
 * \param current Current node
 * \param start Starting node
 * \param path Path taken
 * \param visited Visited nodes
 * \param adjList Adjacency list
 * \param circuits Circuits found
 */
void dfs(int current, int start, std::vector<int> &path, std::unordered_set<int> &visited, 
         std::unordered_map<int, std::vector<int>> &adjList, std::vector<std::vector<int>> &circuits);

/*! \brief Run algorithm to find circuits
 *  \ingroup algorithm.h
 *
 * \param circuits Reference to circuits
 */
void run_algorithm(std::vector<std::vector<int>> &circuits);

/*! \brief Filter short circuits
 *  \ingroup algorithm.h
 *
 * \param circuits Reference to circuits
 * \param minLength Minimum length of circuit
 */
void filterCircuits(std::vector<std::vector<int>> &circuits, size_t minLength);

/*! \brief Check for short circuits
 *  \ingroup algorithm.h
 *
 * \param circuits Reference to circuits
 * \return Index of short circuit or -1 if no short circuit found
 */
int check_short_circuit(std::vector<std::vector<int>> &circuits);

/*! \brief Enable circuits
 *  \ingroup algorithm.h
 *
 * \param circuits Reference to circuits
 */
bool enable_circuits(std::vector<std::vector<int>> &circuits);

#endif