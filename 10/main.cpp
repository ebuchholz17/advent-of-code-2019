#include <math.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }
#define MAX_PROGRAM_LENGTH 200000
#define PI 3.14159265f

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

int list_index_of (point_list *list, point value) {
    int result = -1;
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i].x == value.x && list->values[i].y == value.y) {
            result = i;
            break;
        }
    }
    return result;
}

point list_splice(point_list *list, int index) {
    ASSERT(index < list->numValues);

    point result = list->values[index];
    list->numValues--;
    for (int i = index; i < list->numValues; ++i) {
        list->values[i] = list->values[i+1];
    }

    return result;
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

void sortPointsByAngle (point_list *asteroids, point laserLocation) {
    for (int i = 0; i < asteroids->numValues; ++i) {
        for (int j = i; j > 0; --j) {
            point p0 = asteroids->values[j-1];
            point p1 = asteroids->values[j];
            if (p0.x == 0 && p0.y == 28) {
                int t = 0;
                if (p1.x == 32 && p1.y == 28) {
                    t = 0;
                }
            }
            if (p1.x == 32 && p1.y == 28) {
                int t = 0;
            }

            float p0XDiff = (float)(p0.x - laserLocation.x);
            float p0YDiff = (float)(p0.y - laserLocation.y);
            float p1XDiff = (float)(p1.x - laserLocation.x);
            float p1YDiff = (float)(p1.y - laserLocation.y);
            float firstAngle = atan2f(p0YDiff, p0XDiff);
            float secondAngle = atan2f(p1YDiff, p1XDiff);

            if (firstAngle < 0.0f) { firstAngle += 2.0f * PI; }
            if (secondAngle < 0.0f) { secondAngle += 2.0f * PI; }
            if (firstAngle >= (3.0f * PI) / 2.0f) { firstAngle -= 2.0f * PI; }
            if (secondAngle >= (3.0f * PI) / 2.0f) { secondAngle -= 2.0f * PI; }

            if (secondAngle == firstAngle) {
                float p0dist = sqrtf(p0XDiff * p0XDiff + p0YDiff * p0YDiff);
                float p1dist = sqrtf(p1XDiff * p1XDiff + p1YDiff * p1YDiff);
                if (p1dist < p0dist) {
                    asteroids->values[j-1] = p1;
                    asteroids->values[j] = p0;
                }
            }
            else if (secondAngle < firstAngle) {
                asteroids->values[j-1] = p1;
                asteroids->values[j] = p0;
            }
        }
    }
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

    point_list checkedAsteroids = {};
    checkedAsteroids.capacity = 1000;
    checkedAsteroids.values = (point *)allocateSize(&memory, checkedAsteroids.capacity * sizeof(point));

    int *numVisibleAsteroids = (int *)allocateSize(&memory, asteroids.numValues * sizeof(int));

    for (int i = 0; i < asteroids.numValues; ++i) {
        point asteroid = asteroids.values[i];
        checkedAsteroids.numValues = 0;
        numVisibleAsteroids[i] = 0;
        for (int j = 0; j < asteroids.numValues; ++j) {
            if (i == j) { continue; }
            point asteroidToCheck = asteroids.values[j];
            int xDiff = asteroidToCheck.x - asteroid.x;
            int yDiff = asteroidToCheck.y - asteroid.y;
            int xDiffAbs = xDiff < 0 ? -xDiff : xDiff;
            int yDiffAbs = yDiff < 0 ? -yDiff : yDiff;
            int asteroidGCD = gcd(xDiffAbs, yDiffAbs);
            if (asteroidGCD != 0) {
                xDiff /= asteroidGCD;
                yDiff /= asteroidGCD;
            }
            point checkedAsteroid = Point(xDiff, yDiff);
            if (!list_contains(&checkedAsteroids, checkedAsteroid)) {
                list_push(&checkedAsteroids, checkedAsteroid);
                ++numVisibleAsteroids[i];
            }
        }
    }

    int mostVisibleAsteroids = 0;
    int bestAsteroidIndex = -1;
    for (int i = 0; i < asteroids.numValues; ++i) {
        if (numVisibleAsteroids[i] > mostVisibleAsteroids) {
            mostVisibleAsteroids = numVisibleAsteroids[i];
            bestAsteroidIndex = i;
        }
    }

    printf("%d\n", mostVisibleAsteroids);

    point laserLocation = asteroids.values[bestAsteroidIndex];
    list_splice(&asteroids, bestAsteroidIndex);

    sortPointsByAngle(&asteroids, laserLocation);
    int numAsteroidsDestroyed = 0;
    int currentAsteroid = 0;
    point lastDirection = Point(-1, -1);
    point lastDestroyedAsteroid = Point(-1, -1);
    while (numAsteroidsDestroyed < 200) {
        point asteroid = asteroids.values[currentAsteroid];
        int xDiff = asteroid.x - laserLocation.x;
        int yDiff = asteroid.y - laserLocation.y;
        int xDiffAbs = xDiff < 0 ? -xDiff : xDiff;
        int yDiffAbs = yDiff < 0 ? -yDiff : yDiff;
        int asteroidGCD = gcd(xDiffAbs, yDiffAbs);
        if (asteroidGCD != 0) {
            xDiff /= asteroidGCD;
            yDiff /= asteroidGCD;
        }
        if (xDiff == lastDirection.x && yDiff == lastDirection.y) {
            currentAsteroid = (currentAsteroid + 1) % asteroids.numValues;
        }
        else {
            list_splice(&asteroids, currentAsteroid);
            ++numAsteroidsDestroyed;
            lastDestroyedAsteroid = asteroid;
        }
        lastDirection.x = xDiff;
        lastDirection.y = yDiff;
    }

    printf("%d\n", lastDestroyedAsteroid.x * 100 + lastDestroyedAsteroid.y);

    return 0;
}
