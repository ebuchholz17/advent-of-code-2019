#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }
#define MAX_SPACE_OBJECTS 363636
#define MAX_ORBITING_OBJECTS 10

struct space_object {
    char name[4];
    // assume no more than 10 orbiting objects
    int numOrbitingObjects;
    int orbitingObjects[MAX_ORBITING_OBJECTS];
    int parentPlantHash;
};

bool spaceObjectExists (space_object *spaceObject) {
    return spaceObject->name[0] != 0;
}

int getLetterValue (char letter) {
    int result = 0;
    if (letter >= 'A' && letter <= 'Z') {
        result = (int)(letter - 'A') + 10;
    }
    else if (letter >= '0' && letter <= '9') {
        result = (int)(letter - '0');
    }
    else {
        ASSERT(0);
    }
    return result;
}

int santaHash;
int getSpaceObjectHash (char *name) {
    int value = 0;

    char letter = name[0];
    int letterValue = getLetterValue(letter);
    letterValue *= 10000;
    value += letterValue;

    letter = name[1];
    letterValue = getLetterValue(letter);
    letterValue *= 100;
    value += letterValue;

    letter = name[2];
    letterValue = getLetterValue(letter);
    value += letterValue;
    
    return value;
}

space_object *getSpaceObject (char *name, space_object *spaceObjects) {
    int index = getSpaceObjectHash(name);
    ASSERT(index >= 0 && index < MAX_SPACE_OBJECTS);
    return &spaceObjects[index];
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

void traverseTree (space_object *so, space_object *spaceObjects, int depth, int *total) {
    *total = *total + depth;

    for (int i = 0; i < so->numOrbitingObjects; ++i) {
        int objectIndex = so->orbitingObjects[i];
        space_object *next = &spaceObjects[objectIndex];
        traverseTree(next, spaceObjects, depth + 1, total);
    }
}

int findSanta (int soHash, int prevSpaceHash, int depth, space_object *spaceObjects) {
    if (soHash == santaHash) {
        return depth-1;
    }
    else {
        space_object *so = &spaceObjects[soHash];
        int parentHash = so->parentPlantHash;

        int santaDepth = -1;
        if (parentHash != prevSpaceHash && parentHash != -1) {
            santaDepth = findSanta(parentHash, soHash, depth + 1, spaceObjects);
            if (santaDepth != -1) {
                return santaDepth;
            } 
        }

        for (int i = 0; i < so->numOrbitingObjects; ++i) {
            int childHash = so->orbitingObjects[i];
            if (childHash != prevSpaceHash) {
                santaDepth = findSanta(childHash, soHash, depth + 1, spaceObjects);
                if (santaDepth != -1) {
                    return santaDepth;
                } 
            }
        }
        return -1;
    }
}

int main (int argc, char **argv) {
    space_object *spaceObjects = (space_object *)malloc(MAX_SPACE_OBJECTS * sizeof(space_object));
    for (int i = 0; i < MAX_SPACE_OBJECTS; ++i) {
        space_object *so = &spaceObjects[i];
        *so = {};
        so->parentPlantHash = -1;
    }
    space_object *root = 0;
    int rootHash = getSpaceObjectHash("COM");

    const int stringLength = 10;
    char line[stringLength];

    char word[10];

    while (fgets(line, stringLength, stdin)) {
        char *cursor = line;
        cursor = readUntilCharacter(cursor, word, ')');
        ++cursor;

        int firstSOHash = getSpaceObjectHash(word);
        space_object *firstSO = &spaceObjects[firstSOHash];
        if (!spaceObjectExists(firstSO)) {
            for (int i = 0; i <= 3; ++i) {
                firstSO->name[i] = word[i];
            }
        }

        cursor = readUntilCharacter(cursor, word, '\n');
        ++cursor;

        int secondSOHash = getSpaceObjectHash(word);
        space_object *secondSO = &spaceObjects[secondSOHash];
        if (!spaceObjectExists(secondSO)) {
            for (int i = 0; i <= 3; ++i) {
                secondSO->name[i] = word[i];
            }
        }

        ASSERT(firstSO->numOrbitingObjects < MAX_ORBITING_OBJECTS);

        firstSO->orbitingObjects[firstSO->numOrbitingObjects] = secondSOHash;
        ++firstSO->numOrbitingObjects;
        secondSO->parentPlantHash = firstSOHash;

        if (firstSOHash == rootHash) {
            root = firstSO;
        }
    }

    santaHash = getSpaceObjectHash("SAN");
    int youHash = getSpaceObjectHash("YOU");
    space_object *you = &spaceObjects[youHash];
    int pathToSantaLength = findSanta(you->parentPlantHash, youHash, 0, spaceObjects);
    printf("%d\n", pathToSantaLength);

    //int totalOrbits = 0;
    //traverseTree(root, spaceObjects, 0, &totalOrbits);
    //printf("%d\n", totalOrbits);

    return 0;
}
