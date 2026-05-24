#include "utils.h"
#include <stdlib.h>

void GenerateArray(int *array, int size, int seed) {
    srand(seed);
    for (int i = 0; i < size; i++) {
        array[i] = rand() % 1000;  // числа от 0 до 999
    }
}
