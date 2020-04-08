#include "LSH.h"
#include <iostream>

int main() {

    LSH *lsh = new LSH();

    unsigned int i[] = {1, 1, 1, 2, 2, 2, 3, 3, 3};

    unsigned int h[] = {0, 1, 2, 3, 0, 1, 2, 3, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2,
                        1, 0, 1, 3, 2, 2, 1, 2, 3, 3, 3, 3, 2, 1, 1, 1, 2, 3};

    lsh->insert(9, i, h);

    lsh->view();

    unsigned int q[] = {0, 1, 2, 3, 1, 2, 0, 1};

    unsigned int r[10];

    lsh->top_k(2, 5, q, r);

    for (int i; i < 10; i++) {
        std::cout << r[i] << std::endl;
    }

    return 0;
}