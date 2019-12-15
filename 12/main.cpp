#include <math.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <stdarg.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

typedef long long int64;
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

char *readUntilCharacter (char *currentLetter, char *currentWord, char character) {
    int letterIndex = 0;
    while (*currentLetter != character && *currentLetter != '\n' && currentLetter != 0) {
        currentWord[letterIndex] = *currentLetter;
        letterIndex++;
        currentLetter++;
    }
    currentWord[letterIndex] = 0;
    return currentLetter;
}

void parseMoon (char *line, char *format, int *x, int* y, int *z) {
    char *cursor = line;
    int lineIndex = 0;
    int formatIndex = 0;
    int *values[3];
    values[0] = x;
    values[1] = y;
    values[2] = z;
    int currentValue = 0;
    while (line[lineIndex] != 0 && line[lineIndex] != '\n' && format[formatIndex] != 0) {
        char lineChar = line[lineIndex];
        char formatChar = format[formatIndex];

        if (formatChar == lineChar) {
            ++lineIndex;
            ++formatIndex;
        }
        if (formatChar == '%') {
            char word[10];
            int wordIndex = 0;
            ++formatIndex;
            do {
                word[wordIndex] = line[lineIndex];
                ++wordIndex;
                ++lineIndex;
            } while (format[formatIndex] != line[lineIndex] && line[lineIndex] != 0 && line[lineIndex] != '\n');
            word[wordIndex] = 0;
            wordIndex = 0;

            int value = atoi(word);
            *(values[currentValue]) = value;
            ++currentValue;
        }
    }

}

struct vector3 {
    int x;
    int y;
    int z;
};

struct moon {
    vector3 pos;
    vector3 velocity;
};

struct moon_list {
    moon *values;
    int numValues;
    int capacity;
};

void list_push (moon_list *list, moon value) {
    ASSERT(list->numValues < list->capacity);

    list->values[list->numValues] = value;
    list->numValues++;
}

//bool list_contains (moon_list *list, moon value) {
//    for (int i = 0; i < list->numValues; ++i) {
//        if (list->values[i] == value) {
//            return true;
//        }
//    }
//    return false;
//}
//
//int list_index_of (moon_list *list, moon value) {
//    int result = -1;
//    for (int i = 0; i < list->numValues; ++i) {
//        if (list->values[i] == value) {
//            result = i;
//            break;
//        }
//    }
//    return result;
//}

moon list_splice(moon_list *list, int index) {
    ASSERT(index < list->numValues);

    moon result = list->values[index];
    list->numValues--;
    for (int i = index; i < list->numValues; ++i) {
        list->values[i] = list->values[i+1];
    }

    return result;
}

moon_list *list_copy (moon_list *source, memory_arena *memory) {
    moon_list *result = (moon_list *)allocateSize(memory, sizeof(moon_list) + source->capacity * sizeof(moon));
    result->values = (moon *)((char *)result + sizeof(moon_list)); 
    result->numValues = source->numValues;
    result->capacity = source->capacity;
    for (int i = 0; i < source->numValues; ++i) {
        result->values[i] = source->values[i];
    }
    return result;
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 10 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int stringLength = 30;
    char line[stringLength];

    moon_list moons = {};
    moons.capacity = 10;
    moons.values = (moon *)allocateSize(&memory, moons.capacity * sizeof(moon));

    while (fgets(line, stringLength, stdin)) {
        moon newMoon = {};
        parseMoon(line, "<x=%, y=%, z=%>", &newMoon.pos.x, &newMoon.pos.y, &newMoon.pos.z);
        list_push(&moons, newMoon);
    }

    int64 timeStep = 0;
    while (timeStep < 1000000L) {
        for (int i = 0; i < moons.numValues; ++i) {
            for (int j = 0; j < moons.numValues; ++j) {
                if (i == j) {
                    continue;
                }
                moon *firstMoon = &moons.values[i];
                moon *secondMoon = &moons.values[j];
                if (firstMoon->pos.x < secondMoon->pos.x) {
                    firstMoon->velocity.x += 1;
                }
                else if (firstMoon->pos.x > secondMoon->pos.x) {
                    firstMoon->velocity.x -= 1;
                }
                if (firstMoon->pos.y < secondMoon->pos.y) {
                    firstMoon->velocity.y += 1;
                }
                else if (firstMoon->pos.y > secondMoon->pos.y) {
                    firstMoon->velocity.y -= 1;
                }
                if (firstMoon->pos.z < secondMoon->pos.z) {
                    firstMoon->velocity.z += 1;
                }
                else if (firstMoon->pos.z > secondMoon->pos.z) {
                    firstMoon->velocity.z -= 1;
                }
            }
        }
        for (int i = 0; i < moons.numValues; ++i) {
            moon *currentMoon = &moons.values[i];
            currentMoon->pos.x += currentMoon->velocity.x;
            currentMoon->pos.y += currentMoon->velocity.y;
            currentMoon->pos.z += currentMoon->velocity.z;
        }
        int energySum = 0;
        for (int i = 0; i < moons.numValues; ++i) {
            moon *currentMoon = &moons.values[i];

            int peX = currentMoon->pos.x < 0 ? -currentMoon->pos.x : currentMoon->pos.x;
            int peY = currentMoon->pos.y < 0 ? -currentMoon->pos.y : currentMoon->pos.y;
            int peZ = currentMoon->pos.z < 0 ? -currentMoon->pos.z : currentMoon->pos.z;
            int peTotal = peX + peY + peZ;

            int keX = currentMoon->velocity.x < 0 ? -currentMoon->velocity.x : currentMoon->velocity.x;
            int keY = currentMoon->velocity.y < 0 ? -currentMoon->velocity.y : currentMoon->velocity.y;
            int keZ = currentMoon->velocity.z < 0 ? -currentMoon->velocity.z : currentMoon->velocity.z;
            int keTotal = keX + keY + keZ;

            int energyTotal = peTotal * keTotal;
            energySum += energyTotal;
                printf("pos=<x=%d, y=%d, z=%d>, vel=<x=%d, y=%d, z=%d>\n", 
                       currentMoon->pos.x,
                       currentMoon->pos.y,
                       currentMoon->pos.z,
                       currentMoon->velocity.x,
                       currentMoon->velocity.y,
                       currentMoon->velocity.z);
        }
        printf("\n"); 
        timeStep++;
    }
    for (int i = 0; i < moons.numValues; ++i) {
        moon *currentMoon = &moons.values[i];
        printf("pos=<x=%d, y=%d, z=%d>, vel=<x=%d, y=%d, z=%d>\n", 
               currentMoon->pos.x,
               currentMoon->pos.y,
               currentMoon->pos.z,
               currentMoon->velocity.x,
               currentMoon->velocity.y,
               currentMoon->velocity.z);
    }
    int energySum = 0;
    for (int i = 0; i < moons.numValues; ++i) {
        moon *currentMoon = &moons.values[i];

        int peX = currentMoon->pos.x < 0 ? -currentMoon->pos.x : currentMoon->pos.x;
        int peY = currentMoon->pos.y < 0 ? -currentMoon->pos.y : currentMoon->pos.y;
        int peZ = currentMoon->pos.z < 0 ? -currentMoon->pos.z : currentMoon->pos.z;
        int peTotal = peX + peY + peZ;

        int keX = currentMoon->velocity.x < 0 ? -currentMoon->velocity.x : currentMoon->velocity.x;
        int keY = currentMoon->velocity.y < 0 ? -currentMoon->velocity.y : currentMoon->velocity.y;
        int keZ = currentMoon->velocity.z < 0 ? -currentMoon->velocity.z : currentMoon->velocity.z;
        int keTotal = keX + keY + keZ;

        int energyTotal = peTotal * keTotal;
        printf("%d + %d + %d = %d, %d + %d + %d = %d, %d * %d = %d\n",
               peX, peY, peZ, peTotal, 
               keX, keY, keZ, keTotal, 
               peTotal, keTotal, energyTotal);
        energySum += energyTotal;
    }
    printf("%d\n", energySum);

    // from looking at output, and finding maxima:
    // x period: 186028
    // y period: 231614
    // z period: 60424
    // lcm and answer: 325433763467176

    return 0;
}
