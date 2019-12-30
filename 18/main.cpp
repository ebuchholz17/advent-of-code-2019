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

struct key_to_key_path {
    char a;
    char b;
    int distance;
    char requiredKeys[NUM_KEYS];
    int numRequiredKeys;
    char pickedUpKeys[NUM_KEYS];
    int numPickedUpKeys;
};

struct key_search_node {
    bool availableKeys[NUM_KEYS];
    int distance;
    char letters[4];
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
    int index = -1;
    for (int i = 0; i < list->numValues; ++i) {
        key_search_node currentVal = list->values[i];
        bool lettersMatch = true;
        for (int j = 0; j < 4; ++j) {
            if (currentVal.letters[j] != value.letters[j]) {
                lettersMatch = false;
                break;
            }
        }
        if (!lettersMatch) {
            continue;
        }

        bool keysMatch = true;
        for (int j = 0; j < NUM_KEYS; ++j) {
            if (currentVal.availableKeys[j] != value.availableKeys[j]) {
                keysMatch = false;
                break;
            }
        }
        if (keysMatch) {
            index = i;
            break;
        }
    }
    return index;
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

bool currentCharIsWrongKey(char currentChar, char targetLetter) {
    if (currentChar >= 'a' && currentChar <= 'z') {
        if (currentChar == targetLetter) {
            return false;
        }
        else {
            return true;
        }
    }
    else {
        return false;
    }
}

bool findPath (int startX, int startY, int endX, int endY, char letter, char *map, int mapWidth, int mapHeight, 
               search_node_list *searchList, point_list *path, key_to_key_path *keyToKeyPath) 
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

                char currentChar = map[currentNode.y * mapWidth + currentNode.x];
                if (currentChar >= 'A' && currentChar <= 'Z') {
                    char requiredKey = currentChar + 32 - 'a';
                    keyToKeyPath->requiredKeys[keyToKeyPath->numRequiredKeys] = requiredKey;
                    keyToKeyPath->numRequiredKeys++;
                }
                else if (currentChar >= 'a' && currentChar <= 'z') {
                    char pickedUpKey = currentChar - 'a';
                    keyToKeyPath->pickedUpKeys[keyToKeyPath->numPickedUpKeys] = pickedUpKey;
                    keyToKeyPath->numPickedUpKeys++;
                }
            }
            return true;
        }
        else {
            char upNodeChar = map[(currentNode.y - 1) * mapWidth + currentNode.x];
            if (upNodeChar != '#') {
                pushNodeIfNotExists(currentNode.x, currentNode.y-1, currentNodeIndex, searchList);
            }

            char downNodeChar = map[(currentNode.y + 1) * mapWidth + currentNode.x];
            if (downNodeChar != '#') {
                pushNodeIfNotExists(currentNode.x, currentNode.y+1, currentNodeIndex, searchList);
            }

            char leftNodeChar = map[currentNode.y * mapWidth + (currentNode.x - 1)];
            if (leftNodeChar != '#') {
                pushNodeIfNotExists(currentNode.x-1, currentNode.y, currentNodeIndex, searchList);
            }

            char rightNodeChar = map[currentNode.y * mapWidth + (currentNode.x + 1)];
            if (rightNodeChar != '#') {
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
    int diff = nodeA->distance - nodeB->distance;
    if (diff == 0) {
        int numNodeAKeys = 0;
        for (int i = 0 ; i < NUM_KEYS; ++i) {
            if (nodeA->availableKeys[i]) {
                ++numNodeAKeys;
            }
        }
        int numNodeBKeys = 0;
        for (int i = 0 ; i < NUM_KEYS; ++i) {
            if (nodeB->availableKeys[i]) {
                ++numNodeBKeys;
            }
        }
        return numNodeBKeys - numNodeAKeys;
    }
    else {
        return diff;
    }
}


int findBestKeyPath (int *startXs, int *startYs,
                     char *map, int mapWidth, int mapHeight, bool *availableKeys, 
                     search_node_list *searchList, point_list *path, key_search_node_list *keySeachNodeList, 
                     key *keys, key_search_node_list *cachedNodes) 
{
    key_search_node startNode = {};
    startNode.distance = 0;
    startNode.letters[0] = '{';
    startNode.letters[1] = '|';
    startNode.letters[2] = '}';
    startNode.letters[3] = '~';
    listPush(keySeachNodeList, startNode);

    key_to_key_path keyToKeyPaths[4000];
    for (int i = 0; i < NUM_KEYS; ++i) {
        for (int j = 0; j < NUM_KEYS; ++j) {
            if (i == j) { 
                continue;
            }

            key firstKey = keys[i];
            key secondKey = keys[j];
            key_to_key_path keyToKeyPath = {};
            keyToKeyPath.a = firstKey.letter;
            keyToKeyPath.b = secondKey.letter;

            path->numValues = 0;
            searchList->numValues = 0;
            bool foundPath = findPath(firstKey.x, firstKey.y, secondKey.x, secondKey.y, secondKey.letter, map, mapWidth, mapHeight, searchList, path, &keyToKeyPath);
            if (foundPath) {
                keyToKeyPath.distance = path->numValues - 1;
            }
            else {
                keyToKeyPath.distance = -1;
            }

            int pathIndex = (firstKey.letter - 'a') + (secondKey.letter - 'a') * 100;
            keyToKeyPaths[pathIndex] = keyToKeyPath;
        }
    }
    for (int i = 0; i < NUM_KEYS; ++i) {
        for (int j = 0; j < 4; ++j) {
            key currentKey = keys[i];
            key_to_key_path keyToKeyPath = {};
            keyToKeyPath.a = startNode.letters[j];
            keyToKeyPath.b = currentKey.letter;

            path->numValues = 0;
            searchList->numValues = 0;
            bool foundPath = findPath(startXs[j], startYs[j], currentKey.x, currentKey.y, currentKey.letter, map, mapWidth, mapHeight, searchList, path, &keyToKeyPath);
            if (foundPath) {
                keyToKeyPath.distance = path->numValues - 1;
            }
            else {
                keyToKeyPath.distance = -1;
            }

            int pathIndex = (startNode.letters[j] - 'a') + (currentKey.letter - 'a') * 100;
            keyToKeyPaths[pathIndex] = keyToKeyPath;

            keyToKeyPath = {};
            keyToKeyPath.a = currentKey.letter;
            keyToKeyPath.b = startNode.letters[j];

            path->numValues = 0;
            searchList->numValues = 0;
            foundPath = findPath(currentKey.x, currentKey.y, startXs[j], startYs[j], startNode.letters[j], map, mapWidth, mapHeight, searchList, path, &keyToKeyPath);
            if (foundPath) {
                keyToKeyPath.distance = path->numValues - 1;
            }
            else {
                keyToKeyPath.distance = -1;
            }

            pathIndex = (currentKey.letter - 'a') + (startNode.letters[j] - 'a') * 100;
            keyToKeyPaths[pathIndex] = keyToKeyPath;
        }
    }

    int currentNodeIndex = 0;
    int bestDistance = 9999;
    while (currentNodeIndex < keySeachNodeList->numValues) {
        qsort(keySeachNodeList->values + currentNodeIndex, keySeachNodeList->numValues - currentNodeIndex, sizeof(key_search_node), keySearchNodeSorter);
        key_search_node currentNode = keySeachNodeList->values[currentNodeIndex];

        bool haveAllKeys = true;
        for (int i = 0; i < NUM_KEYS; ++i) {
            if (!currentNode.availableKeys[i]) {
                haveAllKeys = false;
                break;
            }
        }
        
        if (haveAllKeys) {
            bestDistance = currentNode.distance;
            break;
        }
        else {
            for (int i = 0; i < NUM_KEYS; ++i) {
                key currentKey = keys[i];
                if (currentNode.availableKeys[currentKey.letter - 'a']) {
                    continue;
                }
                bool botOnLetter = false;
                for(int j = 0; j < 4; ++j) {
                    if (currentNode.letters[j] == currentKey.letter) {
                        botOnLetter = true;
                        break;
                    }
                }
                if (botOnLetter) {
                    continue;
                }

                for (int letterIndex = 0; letterIndex < 4; ++letterIndex) {
                    char currentNodeLetter = currentNode.letters[letterIndex];
                    int pathIndex = (currentNodeLetter - 'a') + (currentKey.letter - 'a') * 100;
                    key_to_key_path pathToKey = keyToKeyPaths[pathIndex];
                    if (pathToKey.distance == -1) {
                        continue;
                    }
                    bool pathToKeyIsClear = true;
                    if (pathToKey.numRequiredKeys > 0) {
                        for (int j = 0; j < pathToKey.numRequiredKeys; ++j) {
                            char requiredKey = pathToKey.requiredKeys[j];
                            if (!currentNode.availableKeys[requiredKey]) {
                                pathToKeyIsClear = false;
                                break;
                            }
                        }
                    }

                    if (pathToKeyIsClear) {
                        key_search_node nextNode = {};
                        nextNode.distance = currentNode.distance + pathToKey.distance;
                        for (int j = 0; j < 4; ++j) {
                            nextNode.letters[j] = currentNode.letters[j];
                        }
                        nextNode.letters[letterIndex] = currentKey.letter;
                        for (int j = 0; j < NUM_KEYS; ++j) {
                            nextNode.availableKeys[j] = currentNode.availableKeys[j];
                        }
                        for (int j = 0; j < pathToKey.numPickedUpKeys; ++j) {
                            char pickedUpKey = pathToKey.pickedUpKeys[j];
                            nextNode.availableKeys[pickedUpKey] = true;
                        }
                        nextNode.availableKeys[currentKey.letter - 'a'] = true;

                        int cachedNodeIndex = listIndexOf(cachedNodes, nextNode);
                        if (cachedNodeIndex == -1) {
                            listPush(keySeachNodeList, nextNode);
                            listPush(cachedNodes, nextNode);
                        }
                        else {
                            key_search_node cachedNode = cachedNodes->values[cachedNodeIndex];
                            if (cachedNode.distance > nextNode.distance) {
                                listPush(keySeachNodeList, nextNode);
                                cachedNode.distance = nextNode.distance;
                                cachedNodes->values[cachedNodeIndex] = cachedNode;
                            }
                        }
                    }
                }
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
    int startXs[4];
    int startYs[4];
    int startIndex = 0;
    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            char cell = map[i * mapWidth + j];
            if (cell == '@') {
                startYs[startIndex] = i;
                startXs[startIndex] = j;
                ++startIndex;
            }
            else if (cell >= 'a' && cell <= 'z') {
                key newKey = {};
                newKey.letter = cell;
                newKey.x = j;
                newKey.y = i;
                int index = cell - 'a';
                keys[numKeys] = newKey;
                ++numKeys;
            }
        }
    }

    point_list path = pointListInit(&memory, 6500);
    search_node_list searchNodes = searchNodeListInit(&memory, 6500);
    bool availableKeys[NUM_KEYS] = {};
    key_search_node_list keySearchNodes = keySearchNodeListInit(&memory, 10000000);
    key_search_node_list cachedNodes = keySearchNodeListInit(&memory, 1000000);
    int bestDistance = findBestKeyPath(startXs, startYs, map, mapWidth, mapHeight, availableKeys, &searchNodes, &path, &keySearchNodes, keys, &cachedNodes);
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
