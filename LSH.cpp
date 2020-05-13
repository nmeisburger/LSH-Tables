#include "LSH.h"

LSH::LSH() {

    L = NUM_TABLES;
    reservoir_size = RESERVOIR_SIZE;
    range_pow = RANGE_POW;
    range = 1 << range_pow;

    reservoirs = new Reservoir *[L];
    for (int i = 0; i < L; i++) {
        reservoirs[i] = new Reservoir[range]();
    }
}

void LSH::insert(unsigned int num_items, unsigned int *items, unsigned int *hashes) {
#pragma omp parallel for default(none) shared(num_items, hashes, items)
    for (size_t n = 0; n < num_items; n++) {         // Iterate through each item.
        for (size_t table = 0; table < L; table++) { // Iterate through each table for each item.
            // Find the hash index for this item table and insert the item into that reservoir.
            reservoirs[table][hashes[n * L + table]].add(items[n]);
        }
    }
}

void LSH::insert(unsigned int item, unsigned int *hashes) {
    for (size_t table = 0; table < L; table++) { // Iterate through each table.
        // Find the hash index for that table for the itema and insert the item into that reservoir.
        reservoirs[table][hashes[table]].add(item);
    }
}

void LSH::retrieve(unsigned int num_query, unsigned int *hashes, unsigned int *results_buffer) {

#pragma omp parallel for default(none) shared(num_query, hashes, results_buffer)
    for (size_t query = 0; query < num_query; query++) { // Iterate through each query.
        for (size_t table = 0; table < L; table++) {     // Iterate through each table.
            // Calculate the location in the hash array for the given query and table.
            size_t loc = query * L + table;
            // Retrieve the contents of the reservoir and copy them into the results buffer.
            reservoirs[table][hashes[loc]].retrieve(results_buffer + loc * reservoir_size);
        }
    }
}

void LSH::top_k(unsigned int num_query, unsigned int top_k, unsigned int *hashes,
                unsigned int *selection) {

    // Buffer to store reservoirs.
    unsigned int *extracted_reservoirs = new unsigned int[num_query * L * reservoir_size];

    // Extract the reservoirs given by the hash indices.
    this->retrieve(num_query, hashes, extracted_reservoirs);

    // A block is total number items extracted foreach query.
    unsigned int block = L * reservoir_size;

    for (size_t query = 0; query < num_query; query++) {
        unsigned int *start = extracted_reservoirs + query * block;
        // Sort each block so that elements with the same id are next to each other. This makes
        // frequency counting simpler.
        std::sort(start, start + block);

        // Vector to store most frequent elements.
        std::vector<std::pair<unsigned int, unsigned int>> counts;
        unsigned int count = 0;
        unsigned int last = *start;

        // Iterate through each item in the block and prefrom frequency counting.
        for (size_t i = 0; i < block; i++) {
            if (last == start[i]) {
                count++;
            } else {
                if (last != EMPTY) {
                    counts.push_back(std::make_pair(last, count));
                }
                count = 1;
                last = start[i];
            }
        }
        if (last != EMPTY) {
            counts.push_back(std::make_pair(last, count));
        }

        // The counts vector now stores pairs of each element and how many times it occurred.
        // Sort it by count to get the final ordering of the most frequent elements.
        std::sort(counts.begin(), counts.end(),
                  [&counts](std::pair<int, int> a, std::pair<int, int> b) {
                      return a.second > b.second;
                  });

        size_t k;

        // The most frequent elements are the nearest neighbors.
        for (k = 0; k < std::min(top_k, (unsigned int)counts.size()); k++) {
            selection[query * top_k + k] = counts[k].first;
        }
        // If there are fewer unique elements than nearest neighbors, return EMPTY as the value.
        for (; k < top_k; k++) {
            selection[query * top_k + k] = EMPTY;
        }
    }

    delete[] extracted_reservoirs;
}

void LSH::reset() {
    for (size_t t = 0; t < L; t++) {
        for (size_t r = 0; r < range; r++) {
            reservoirs[t][r].reset();
        }
    }
}

void LSH::view() {
    for (size_t t = 0; t < L; t++) {
        printf("LSH Table %lu\n", t);
        for (size_t r = 0; r < range; r++) {
            reservoirs[t][r].view();
        }
        printf("\n");
    }
}

void LSH::add_random_items(unsigned int num_items, bool verbose) {

    unsigned int *items = new unsigned int[num_items];
    unsigned int *hashes = new unsigned int[num_items * L];

    for (size_t i = 0; i < num_items; i++) {
        items[i] = i;
        if (verbose)
            printf("Item: %lu -> { ", i);
        for (size_t h = 0; h < L; h++) {
            hashes[i * L + h] = rand() % range;
            if (verbose)
                printf("%lu ", hashes[i * L + h]);
        }
        if (verbose)
            printf("}\n");
    }

    insert(num_items, items, hashes);

    delete[] items;
    delete[] hashes;
}