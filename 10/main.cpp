#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }
#define MAX_PROGRAM_LENGTH 200000

typedef unsigned int uint;
typedef unsigned long long uint64;

struct point {
    int x;
    int y;
};

point Point (int x, int y) {
    point result;
    result.x = x;
    result.y = y;
    return result;
}

struct point_list {
    point *values;
    int numValues;
    int capacity;
};

void list_push (point_list *list, point value) {
    ASSERT(list->numValues < list->capacity);

    list->values[list->numValues] = value;
    list->numValues++;
}

bool list_contains (point_list *list, point value) {
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i].x == value.x && list->values[i].y == value.y) {
            return true;
        }
    }
    return false;
}

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

int gcd (int a, int b) { 
    if (b == 0) {
        return a; 
    }
    return gcd(b, a % b);  
} 

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int stringLength = 50;
    char line[stringLength];

    point_list asteroids = {};
    asteroids.capacity = 1000;
    asteroids.values = (point *)allocateSize(&memory, asteroids.capacity * sizeof(point));

    char *field = (char *)allocateSize(&memory, sizeof(char) * 2000);
    int fieldWidth = 0;
    int fieldHeight = 0;
    while (fgets(line, stringLength, stdin)) {
        int x = 0;
        while (line[x] != 0 && line[x] != '\n') {
            field[fieldHeight * fieldWidth + x] = line[x];
            if (line[x] == '#') {
                point p = Point(x, fieldHeight);
                list_push(&asteroids, p);
            }
            ++x;
        }
        fieldWidth = x;
        ++fieldHeight;
    }

    for (int i = 0; i < asteroids.numValues; ++i) {

    }

    return 0;
}
