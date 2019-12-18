#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

typedef unsigned long long uint64;

struct memory_arena {
    void *base;
    uint64 size;
    uint64 capacity;
};

void *allocateSize (memory_arena *memory, uint64 size) {
    ASSERT(memory->size + size <= memory->capacity);

    void *result = (char *)memory->base + memory->size;
    memory->size += size;
    return result;
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 100 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    int *numbers = (int *)allocateSize(&memory, 10000000 * sizeof(int));
    int numNumbers = 0;
    
    const int stringLength = 1000;
    char *line = (char *)allocateSize(&memory, stringLength);
    fgets(line, stringLength, stdin);

    for (int i = 0; i < 1; ++i) {
        int inputIndex = 0;
        while (line[inputIndex] != 0) {
            numbers[numNumbers] = line[inputIndex] - '0';
            //numbers[inputIndex] = line[inputIndex] - '0';
            ++inputIndex;
            ++numNumbers;
        }
        numNumbers -= 1;
    }

    int pattern[] = { 0, 1, 0, -1 };

    int *numbersCopy = (int *)allocateSize(&memory, 10000000 * sizeof(int));
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < numNumbers; ++j) {
            int sum = 0;
            int patternIndex = 0;
            int repeatCount = j+1;
            int repeatIndex = 0;
            for (int k = 0; k < numNumbers; ++k) {
                repeatIndex = (repeatIndex + 1) % repeatCount;
                if (repeatIndex == 0) {
                    patternIndex = (patternIndex + 1) % 4;
                }
                int multiplyValue = pattern[patternIndex];
                int newValue = numbers[k] * multiplyValue;
                sum += newValue;
            }
            if (sum < 0) {
                sum = -sum;
            }
            sum = sum % 10;
            numbersCopy[j] = sum;
        }
        for (int j = 0; j < numNumbers; ++j) {
            numbers[j] = numbersCopy[j];
        }
        for (int j = 0; j < numNumbers; ++j) {
            printf("%d", numbers[j]);
        }
        printf("\n");
    }

    for (int i = 0; i < numNumbers; ++i) {
        printf("%d", numbers[i]);
    }
    printf("\n");

    return 0;
}
