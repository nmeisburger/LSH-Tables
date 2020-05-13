#pragma once

#include "Reservoir.h"
#include <iostream>
#include <omp.h>
#include <vector>

class LSH {
  private:
    // The number of hash tables.
    unsigned int L;
    // The size of each reservoir.
    unsigned int reservoir_size;
    // The log base 2 of the number of reservoirs per table.
    unsigned int range_pow;
    // The number of reservoirs per table (range of the hash functions).
    unsigned int range;
    // The reservoirs. This is represented as an array, which contains array of of reservoirs. Each
    // array of reservoirs is a table, and the outer array represents the collection of tables.
    Reservoir **reservoirs;

  public:
    // Constructor.
    LSH();

    /**
     * This function inserts a batch of vectors into the tables.
     *
     * @param num_items: the number of items in the batch.
     *
     * @param items: a pointer to an array of item ids. These ids are stored in the hash tables and
     * are used to identify the vectors returned from queries. The use of ids instead of full
     * vectors reduces the memory requirements of the system as they are easier to store. The length
     * of this array should be equivalent to num_items.
     *
     * @param hashes: a pointer to an array of hash indicies for the vectors. The length of this
     * array is L * num_items, where L is the number of hash tables. The i-th block of L hash
     * indices corresponds to the hash indices of the i-th vector foreach hash table.
     */
    void insert(unsigned int num_items, unsigned int *items, unsigned int *hashes);

    /**
     * This function inserts a single vector into the tables.
     *
     * @param item: an unsigned int id for the vector. This id is stored in the hash tables and
     * is used to identify the vector when returned from queries. The use of ids instead of full
     * vectors reduces the memory requirements of the system as they are easier to store.
     *
     * @param hashes: a pointer to an array of hash indicies for the vector. The length of this
     * array is L, where L is the number of hash tables. The i-th hash indices corresponds to the
     * i-th table.
     */
    void insert(unsigned int item, unsigned int *hashes);

    /**
     * This function copys the reservoirs corresponding to a set of hash indices into a buffer.
     *
     * @param num_query: the number of queries.
     *
     * @param hashes: an pointer to an array of hash indices foreach query. The length of this array
     * is L * num_query, where L is the number of hash tables. The i-th block of L hash
     * indices corresponds to the hash indices of the i-th vector foreach hash table.
     *
     * @param results_buffer: a pointer to an array for storing the contents of the reservoirs. This
     * function selects the reservoir given by each hash index and copies its contents into the
     * buffer. The length of the results buffer should be num_query * L * reservoir_size because
     * each query will have L hash indices which will each correspond to a reservoir. For the i-th
     * query, the start of the reservoir for the j-th table for that query is given by
     * results_buffer + i * L * reservoir_size + j * reservoir_size.
     */
    void retrieve(unsigned int num_query, unsigned int *hashes, unsigned int *results_buffer);

    /**
     * This function finds an estimation of the nearest neighbors for a set of queries and copies
     * them into a buffer. If there are fewer unique items in the reservoirs than the value of
     * top_k for a query, then the selection array is filled in with the maximum value of an
     * unsigned int for the missing nearest neighbors.
     *
     * @param num_query: the number of queries.
     *
     * @param top_k: the number of nearest neighbors to find foreach query.
     *
     * @param hashes: an pointer to an array of hash indices foreach query. The length of this array
     * is L * num_query, where L is the number of hash tables. The i-th block of L hash
     * indices corresponds to the hash indices of the i-th vector foreach hash table.
     *
     * @param selection: a pointer to an array for storing top_k nearest neighbors foreach query.
     * This array should have a length of num_query * top_k. The j-th nearest neighbor for the i-th
     * query is given by selection[i * top_k + j].
     */
    void top_k(unsigned int num_query, unsigned int top_k, unsigned int *hashes,
               unsigned int *selection);

    // Clears all reservoirs in hash table.
    void reset();

    // Prints hash table.
    void view();

    // For debugging.
    void add_random_items(unsigned int num_items, bool verbose);

    // Destructor.
    ~LSH();
};