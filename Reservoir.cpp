#include "Reservoir.h"

Reservoir::Reservoir() {
    size = RESERVOIR_SIZE;
    count = 0;
    lock = new omp_lock_t();

    omp_init_lock(lock);

    reservoir = new unsigned int[size];

    for (size_t i = 0; i < size; i++) {
        reservoir[i] = EMPTY;
    }
}

void Reservoir::add(unsigned int item) {
    omp_set_lock(lock); // Acquire lock on reservoir.

    if (count < size) {
        // If the reservoir is not full, then insert the element.
        reservoir[count] = item;
        count++;
    } else {
        // Otherwise sample an element between 0 and count (count > size in this case). If the
        // sampled location is less than the size, the replace the item at that location in the
        // reservoir.
        unsigned int loc = rand() % count;
        if (loc < size) {
            reservoir[loc] = item;
        }
        count++;
    }
    omp_unset_lock(lock); // Release lock on reservoir.
}

void Reservoir::retrieve(unsigned int *buffer) {
    omp_set_lock(lock);
    std::copy(reservoir, reservoir + size, buffer);
    omp_unset_lock(lock);
}

unsigned int Reservoir::get_size() { return size; }

unsigned int Reservoir::get_count() { return count; }

void Reservoir::reset() {
    for (size_t i = 0; i < size; i++) {
        reservoir[i] = EMPTY;
    }
    count = 0;
}

void Reservoir::view() {
    printf("Reservoir [%d/%d] ", count, size);
    for (size_t i = 0; i < std::min(count, size); i++) {
        printf("%u ", reservoir[i]);
    }
    printf("\n");
}

Reservoir::~Reservoir() {
    omp_destroy_lock(lock);
    delete[] reservoir;
}
