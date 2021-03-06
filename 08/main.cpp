#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }
#define MAX_PROGRAM_LENGTH 2000

#define IMAGE_WIDTH 25
#define IMAGE_HEIGHT 6

struct memory_arena {
    void *base;
    int size;
    int capacity;
};

void *allocateSize (memory_arena *memory, int size) {
    ASSERT(memory->size + size <= memory->capacity);

    void *result = (char *)memory->base + memory->size;
    memory->size += size;
    return result;
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1024 * 1024;
    memory.base = malloc(memory.capacity);

    int stringLength = 20000;
    char *line = (char *)allocateSize(&memory, stringLength);

    fgets(line, stringLength, stdin);

    int imageLength = IMAGE_WIDTH * IMAGE_HEIGHT;

    const int numLayers = 100;
    int *imageData = (int *)allocateSize(&memory, imageLength * numLayers * sizeof(int));
    int *layers[numLayers];
    for (int i = 0; i < numLayers; ++i) {
        layers[i] = &imageData[i * imageLength];
    }

    int index = 0;
    while (line[index] != 0 && line[index] != '\n') {
        char character = line[index];
        int value = character - '0';
        int *layer = layers[index / imageLength];
        layer[index % imageLength] = value;
        ++index;
    }

    int *finalImage = (int *)allocateSize(&memory, imageLength);

    for (int i = 0; i < IMAGE_HEIGHT; ++i) {
        for (int j = 0; j < IMAGE_WIDTH; ++j) {
            for (int layerIndex = 0; layerIndex < numLayers; ++layerIndex) {
                int *layer = layers[layerIndex];
                int value = layer[i * IMAGE_WIDTH + j];
                if (value != 2) {
                    finalImage[i * IMAGE_WIDTH + j] = value;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < IMAGE_HEIGHT; ++i) {
        for (int j = 0; j < IMAGE_WIDTH; ++j) {
            printf("%d", finalImage[i * IMAGE_WIDTH + j]);
        }
        printf("\n");
    }

    //int fewestZeroes = 150;
    //int layerWithFewestZeroes = -1;
    //for (int i = 0; i < 100; ++i) {
    //    int numZeroes = 0;
    //    int *layer = layers[i];
    //    for (int j = 0; j < imageLength; ++j) {
    //        int value = layer[j];
    //        if (value == 0) {
    //            ++numZeroes;
    //        }
    //    }
    //    if (numZeroes < fewestZeroes) {
    //        fewestZeroes = numZeroes;
    //        layerWithFewestZeroes = i;
    //    }
    //}

    //int *fewestZeroesLayer = layers[layerWithFewestZeroes];
    //int numOnes = 0;
    //int numTwos = 0;
    //for (int i = 0; i < imageLength; ++i) {
    //    int value = fewestZeroesLayer[i];
    //    if (value == 1) {
    //        ++numOnes;
    //    }
    //    else if (value == 2) {
    //        ++numTwos;
    //    }
    //}

    //int result = numOnes * numTwos;
    //printf("%d\n", result);

    return 0;
}
