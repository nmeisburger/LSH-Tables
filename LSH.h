#pragma once

#include "Reservoir.h"
#include <iostream>
#include <omp.h>
#include <vector>

class LSH {
  private:
    unsigned int L;
    unsigned int reservoir_size;
    unsigned int range_pow;
    unsigned int range;
    Reservoir **reservoirs;

  public:
    LSH();

    void insert(unsigned int num_items, unsigned int *items, unsigned int *hashes);

    void insert(unsigned int item, unsigned int *hashes);

    void retrieve(unsigned int num_query, unsigned int *hashes, unsigned int *results_buffer);

    void top_k(unsigned int num_query, unsigned int top_k, unsigned int *hashes,
               unsigned int *selection);

    void reset();

    void view();

    void add_random_items(unsigned int num_items, bool verbose);

    ~LSH();
};