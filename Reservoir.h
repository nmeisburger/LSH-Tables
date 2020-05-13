#pragma once

#include "config.h"
#include <algorithm>
#include <iostream>
#include <omp.h>

#define EMPTY -1

class Reservoir {
  private:
    // The size of the reservoir.
    unsigned int size;
    // The number of attempted insertions.
    unsigned int count;
    // Lock for thread safety.
    omp_lock_t *lock;
    // The array to store the contents of the reservoir.
    unsigned int *reservoir;

  public:
    // Constructor.
    Reservoir(unsigned int size);

    // Constructor.
    Reservoir();

    // Adds an item to the reservoir.
    void add(unsigned int item);

    // Copies the contents of the reservoir into the specified buffer.
    void retrieve(unsigned int *buffer);

    // Gets the size of the reservoir.
    unsigned int get_size();

    // Gets the count of the reservoir.
    unsigned int get_count();

    // Clears the reservoir and resets the count.
    void reset();

    // Prints the contents of the reservoir.
    void view();

    // Destructor.
    ~Reservoir();
};
