#include <math.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }
#define NUM_KEYS 26

#include "win_console.cpp"

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

struct point {
    int x;
    int y;
};

struct point_list {
    point *values;
    int numValues;
    int capacity;
};

void listPush (point_list *list, point value) {
    ASSERT(list->numValues < list->capacity);

    list->values[list->numValues] = value;
    list->numValues++;
}

point_list pointListInit (memory_arena *memory, int capacity) {
    point_list result = {};
    result.capacity = capacity;
    result.values = (point *)allocateSize(memory, capacity * sizeof(point));
    return result;
}

struct key {
    int x;
    int y;
    char letter;
};

struct search_node {
    int x;
    int y;
    int previousNodeIndex;
};

struct search_node_list {
    search_node *values;
    int numValues;
    int capacity;
};

void listPush (search_node_list *list, search_node value) {
    ASSERT(list->numValues < list->capacity);

    list->values[list->numValues] = value;
    list->numValues++;
}

search_node_list searchNodeListInit (memory_arena *memory, int capacity) {
    search_node_list result = {};
    result.capacity = capacity;
    result.values = (search_node *)allocateSize(memory, capacity * sizeof(search_node));
    return result;
}

int listIndexOf (search_node_list *list, search_node value) {
    int result = -1;
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i].x == value.x && list->values[i].y == value.y) {
            result = i;
            break;
        }
    }
    return result;
}

struct key_search_node {
    bool availableKeys[NUM_KEYS];
    int distance;
    int x;
    int y;
    char letter;
};

struct key_search_node_list {
    key_search_node *values;
    int numValues;
    int capacity;
};

void listPush (key_search_node_list *list, key_search_node value) {
    ASSERT(list->numValues < list->capacity);

    list->values[list->numValues] = value;
    list->numValues++;
}

key_search_node_list keySearchNodeListInit (memory_arena *memory, int capacity) {
    key_search_node_list result = {};
    result.capacity = capacity;
    result.values = (key_search_node *)allocateSize(memory, capacity * sizeof(key_search_node));
    return result;
}

int listIndexOf (key_search_node_list *list, key_search_node value) {
    int result = -1;
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i].x == value.x && list->values[i].y == value.y) {
            result = i;
            break;
        }
    }
    return result;
}

inline void pushNodeIfNotExists (int x, int y, int prevIndex, search_node_list *searchList) {
    search_node nextNode = {};
    nextNode.x = x;
    nextNode.y = y;
    nextNode.previousNodeIndex = prevIndex;
    if (listIndexOf(searchList, nextNode) == -1) {
        listPush(searchList, nextNode);
    }
}

bool canPassThroughDoor (bool *availableKeys, char nodeChar, char currentLetter) {
    if (nodeChar >= 'A' && nodeChar <= 'Z') {
        int index = nodeChar + 32 - 'a';
        return availableKeys[index];
    }
    else if (nodeChar >= 'a' && nodeChar <= 'z') {
        int index = nodeChar - 'a';
        return availableKeys[index] || nodeChar == currentLetter;
    }
    else {
        return true;
    }
}

bool findPath (int startX, int startY, int endX, int endY, char currentLetter, char *map, int mapWidth, int mapHeight, 
               bool *availableKeys, search_node_list *searchList, point_list *path) 
{
    search_node startNode = {};
    startNode.x = startX;
    startNode.y = startY;
    startNode.previousNodeIndex = -1;
    listPush(searchList, startNode);
    int currentNodeIndex = 0;

    while (currentNodeIndex < searchList->numValues) {
        search_node currentNode = searchList->values[currentNodeIndex];

        if (currentNode.x == endX && currentNode.y == endY) {
            point nextPoint = {};
            nextPoint.x = currentNode.x;
            nextPoint.y = currentNode.y;
            listPush(path, nextPoint);

            while (currentNode.previousNodeIndex != -1) {
                currentNode = searchList->values[currentNode.previousNodeIndex];

                nextPoint = {};
                nextPoint.x = currentNode.x;
                nextPoint.y = currentNode.y;
                listPush(path, nextPoint);
            }

            return true;
        }
        else {
            char upNodeChar = map[(currentNode.y - 1) * mapWidth + currentNode.x];
            if (upNodeChar != '#' && canPassThroughDoor(availableKeys, upNodeChar, currentLetter)) {
                pushNodeIfNotExists(currentNode.x, currentNode.y-1, currentNodeIndex, searchList);
            }

            char downNodeChar = map[(currentNode.y + 1) * mapWidth + currentNode.x];
            if (downNodeChar != '#' && canPassThroughDoor(availableKeys, downNodeChar, currentLetter)) {
                pushNodeIfNotExists(currentNode.x, currentNode.y+1, currentNodeIndex, searchList);
            }

            char leftNodeChar = map[currentNode.y * mapWidth + (currentNode.x - 1)];
            if (leftNodeChar != '#' && canPassThroughDoor(availableKeys, leftNodeChar, currentLetter)) {
                pushNodeIfNotExists(currentNode.x-1, currentNode.y, currentNodeIndex, searchList);
            }

            char rightNodeChar = map[currentNode.y * mapWidth + (currentNode.x + 1)];
            if (rightNodeChar != '#' && canPassThroughDoor(availableKeys, rightNodeChar, currentLetter)) {
                pushNodeIfNotExists(currentNode.x+1, currentNode.y, currentNodeIndex, searchList);
            }
        }

        ++currentNodeIndex;
    }
    return false;
}

int keySearchNodeSorter (const void *a, const void *b) {
    key_search_node *nodeA = (key_search_node *)a;
    key_search_node *nodeB = (key_search_node *)b;
    return nodeA->distance - nodeB->distance;
}

int findBestKeyPath (int startX, int startY, char *map, int mapWidth, int mapHeight, bool *availableKeys, search_node_list *searchList, point_list *path, key_search_node_list *keySeachNodeList, key *keys) {
    key_search_node startNode = {};
    startNode.x = startX;
    startNode.y = startY;
    startNode.distance = 0;
    startNode.letter = '{';
    listPush(keySeachNodeList, startNode);

    int cachedDistances[3000] = {};

    int currentNodeIndex = 0;
    int bestDistance = 9999;
    while (currentNodeIndex < keySeachNodeList->numValues) {
        qsort(keySeachNodeList->values + currentNodeIndex, keySeachNodeList->numValues - currentNodeIndex, sizeof(key_search_node), keySearchNodeSorter);
        key_search_node currentNode = keySeachNodeList->values[currentNodeIndex];

        int numAvailableKeys = 0;

        for (int i = 0; i < NUM_KEYS; ++i) {
            if (currentNode.availableKeys[i]) {
                continue;
            }
            key currentKey = keys[i];
            int cachedDistanceIndex = (currentNode.letter - 'a') + (currentKey.letter - 'a') * 100;
            if (cachedDistances[cachedDistanceIndex] != 0) {
                    ++numAvailableKeys;
                key_search_node nextNode = {};
                nextNode.x = currentKey.x;
                nextNode.y = currentKey.y;
                nextNode.distance = currentNode.distance + cachedDistances[cachedDistanceIndex];
                nextNode.letter = currentKey.letter;
                for (int j = 0; j < NUM_KEYS; ++j) {
                    nextNode.availableKeys[j] = currentNode.availableKeys[j];
                }
                nextNode.availableKeys[i] = true;
                listPush(keySeachNodeList, nextNode);
            }
            else {
                path->numValues = 0;
                searchList->numValues = 0;
                bool keyReachable = findPath(currentKey.x, currentKey.y, currentNode.x, currentNode.y, currentKey.letter, map, mapWidth, mapHeight, currentNode.availableKeys, searchList, path);
                if (keyReachable) {
                    ++numAvailableKeys;
                    key_search_node nextNode = {};
                    nextNode.x = currentKey.x;
                    nextNode.y = currentKey.y;
                    nextNode.distance = currentNode.distance + path->numValues - 1;
                nextNode.letter = currentKey.letter;
                    cachedDistances[cachedDistanceIndex] = path->numValues - 1;
                    for (int j = 0; j < NUM_KEYS; ++j) {
                        nextNode.availableKeys[j] = currentNode.availableKeys[j];
                    }
                    nextNode.availableKeys[i] = true;
                    listPush(keySeachNodeList, nextNode);
                }
            }
        }

        if (numAvailableKeys == 0) {
            if (currentNode.distance < bestDistance) {
                bestDistance = currentNode.distance;
            }
        }
        ++currentNodeIndex;

    }
   
    return bestDistance;
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 500 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int mapWidth = 81;
    const int mapHeight = 81;
    char *map = (char *)allocateSize(&memory, mapWidth * mapHeight);
    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            map[i * mapWidth + j] = ' ';
        }
    }

    const int stringLength = 100;
    char *line = (char *)allocateSize(&memory, stringLength);

    int row = 0;
    while(fgets(line, stringLength, stdin)) {
        int col = 0;
        while (line[col] != 0 && line[col] != '\n') {
            map[row * mapWidth + col] = line[col];
            ++col;
        }
        ++row;
    }

    key keys[30];
    int numKeys = 0;
    int startX = 0;
    int startY = 0;
    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            char cell = map[i * mapWidth + j];
            if (cell == '@') {
                startY = i;
                startX = j;
            }
            else if (cell >= 'a' && cell <= 'z') {
                key newKey = {};
                newKey.letter = cell;
                newKey.x = j;
                newKey.y = i;
                int index = cell - 'a';
                keys[index] = newKey;
                ++numKeys;
            }
        }
    }

    point_list path = pointListInit(&memory, 6500);
    search_node_list searchNodes = searchNodeListInit(&memory, 6500);
    bool availableKeys[NUM_KEYS] = {};
    key_search_node_list keySearchNodes = keySearchNodeListInit(&memory, 10000000);
    int bestDistance = findBestKeyPath(startX, startY, map, mapWidth, mapHeight, availableKeys, &searchNodes, &path, &keySearchNodes, keys);
    printf("%d\n", bestDistance);

    //for (int i = 0; i < path.numValues; ++i) {
    //    point nextPoint = path.values[i];
    //    map[nextPoint.y * mapWidth + nextPoint.x] = 'O';
    //}

    //for (int i = 0; i < mapHeight; ++i) {
    //    for (int j = 0; j < mapWidth; ++j) {
    //        char cell = map[i * mapWidth + j];
    //        printf("%c", cell);
    //    }
    //    printf("\n");
    //}

    // init windows console 
    //win_console winConsole;
    //float targetFrameRate = 1000.0f / 60.0f;
    //initWinConsole(&winConsole, targetFrameRate);
    //setWinConsoleOutputActive(&winConsole);

    //int windowWidth = 88;
    //int windowHeight = 50;
    //char *window = (char *)allocateSize(&memory, windowWidth * windowHeight);
    //int requiredWindowBufferSize = initWinConsoleBuffer(&winConsole, windowWidth, windowHeight);
    //void *windowBuffer = allocateSize(&memory, requiredWindowBufferSize);
    //setWindowBufferMemory(&winConsole, windowBuffer);



    //win_console_input input = {};

    //startWinConsoleFrame(&winConsole);

    //writeWinConsoleCharsToWholeScreen(&winConsole, window);
    //renderWinConsole(&winConsole);

    //winConsoleResetInput(&input);
    //waitForEndOfWinConsoleFrame(&winConsole);

//    char dustText[30];
//    _itoa_s((int)dustCollected, dustText, 30, 10);
//    winConsoleWriteText(&winConsole, dustText, 30, 0, 0);
//    renderWinConsole(&winConsole);
//
//    while (true) { }
//
//    setWinConsoleStandardOutputActive(&winConsole);
//    releaseWinConsoleHandles(&winConsole);

    return 0;
}
