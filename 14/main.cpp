#include <math.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

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

char *readUntilAnyOfCharacters (char *currentLetter, char *currentWord, char character1, char character2) {
    int letterIndex = 0;
    while (*currentLetter != character1 && *currentLetter != character2 && 
           *currentLetter != '\n' && currentLetter != 0) 
    {
        currentWord[letterIndex] = *currentLetter;
        letterIndex++;
        currentLetter++;
    }
    currentWord[letterIndex] = 0;
    return currentLetter;
}

struct reaction_component {
    char name[8];
    int amountNeeded;
};

struct chemical {
    reaction_component recipe[10];
    char name[8];
    int64 amountAvailable;
    int64 amountYielded;
    int numComponentsInReaction;
};

struct chemical_list {
    chemical *values;
    int numValues;
    int capacity;
};

void listPush (chemical_list *list, chemical value) {
    ASSERT(list->numValues < list->capacity);

    list->values[list->numValues] = value;
    list->numValues++;
}

bool stringsAreEqual (char *a, char *b) {
    int i, j;
    for (i = 0, j = 0; a[i] && b[j]; ++i, ++j) {
        if (a[i] != b[j]) { 
            return false;
        }
    }
    return a[i] == 0 && b[j] == 0;
}

int listIndexOf (chemical_list *list, chemical value) {
    int result = -1;
    char *chemicalName = value.name;
    for (int i = 0; i < list->numValues; ++i) {
        if (stringsAreEqual(list->values[i].name, value.name)) {
            result = i;
            break;
        }
    }
    return result;
}

bool list_contains (chemical_list *list, chemical value) {
    return listIndexOf(list, value) != -1;
}

chemical listSplice(chemical_list *list, int index) {
    ASSERT(index < list->numValues);

    chemical result = list->values[index];
    list->numValues--;
    for (int i = index; i < list->numValues; ++i) {
        list->values[i] = list->values[i+1];
    }

    return result;
}

chemical_list *listCopy (chemical_list *source, memory_arena *memory) {
    chemical_list *result = (chemical_list *)allocateSize(memory, sizeof(chemical_list));
    result->values = (chemical *)allocateSize(memory, sizeof(chemical) * source->capacity);
    result->numValues = source->numValues;
    result->capacity = source->capacity;
    for (int i = 0; i < source->numValues; ++i) {
        result->values[i] = source->values[i];
    }
    return result;
}

chemical *listGetByName(chemical_list *list, char *name) {
    chemical *result = 0;
    for (int i = 0; i < list->numValues; ++i) {
        if (stringsAreEqual(list->values[i].name, name)) {
            result = &list->values[i];
            break;
        }
    }
    return result;
}

void copyString (char *source, char *destination) {
    int index = 0;
    while (source[index] != 0) {
        destination[index] = source[index];
        ++index;
    }
    destination[index] = 0;
}

bool produceChemical (char *chemicalName, int64 numToProduce, 
                      chemical_list *chemicals, int64 *totalOreAvailable, int64 *totalOreExpended) 
{
    bool outOfOre = false;
    if (stringsAreEqual(chemicalName, "ORE")) {
        *totalOreExpended = *totalOreExpended + numToProduce;
        if (*totalOreExpended > *totalOreAvailable) {
            return true;
        }
        return false;
    }
    else {
        chemical *chem = listGetByName(chemicals, chemicalName);
        if (chem->amountAvailable >= numToProduce) {
            chem->amountAvailable -= numToProduce;
        }
        else {
            int64 multiplesNeeded = numToProduce / chem->amountYielded;
            if (numToProduce % chem->amountYielded > 0) {
                multiplesNeeded += 1;
            }
            int64 multiplesHad = chem->amountAvailable / chem->amountYielded;
            multiplesNeeded -= multiplesHad;
            for (int i = 0; i < chem->numComponentsInReaction; ++i) {
                reaction_component *comp = &chem->recipe[i];
                bool notEnoughOre = produceChemical(comp->name, multiplesNeeded * comp->amountNeeded, chemicals, totalOreAvailable, totalOreExpended);
                if (notEnoughOre) {
                    outOfOre = true;
                    break;
                }
            }
            if (!outOfOre) {
                chem->amountAvailable += multiplesNeeded * chem->amountYielded;
                chem->amountAvailable -= numToProduce;
            }
        }
    }
    return outOfOre;
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int stringLength = 100;
    char line[stringLength];
    char word[10];

    chemical_list chemicals = {};
    chemicals.capacity = 100;
    chemicals.values = (chemical *)allocateSize(&memory, chemicals.capacity * sizeof(chemical));

    while (fgets(line, stringLength, stdin)) {
        char *cursor = line;
        chemical chem = {};

        reaction_component comp = {};
        cursor = readUntilCharacter(cursor, word, ' ');
        ++cursor;
        comp.amountNeeded = atoi(word);

        cursor = readUntilAnyOfCharacters(cursor, word, ' ', ',');
        copyString(word, comp.name);

        chem.recipe[chem.numComponentsInReaction] = comp;
        chem.numComponentsInReaction++;

        while (*cursor == ',') {
            cursor += 2;
            cursor = readUntilCharacter(cursor, word, ' ');
            ++cursor;
            comp.amountNeeded = atoi(word);

            cursor = readUntilAnyOfCharacters(cursor, word, ' ', ',');
            copyString(word, comp.name);

            chem.recipe[chem.numComponentsInReaction] = comp;
            chem.numComponentsInReaction++;
        }

        cursor += 4;
        cursor = readUntilCharacter(cursor, word, ' ');
        ++cursor;
        chem.amountYielded = atoi(word);

        cursor = readUntilCharacter(cursor, word, '\n');
        copyString(word, chem.name);

        listPush(&chemicals, chem);
    }

    int64 totalOreAvailable = 1000000000000L;
    int64 totalOreExpended = 0;
    int64 amountOfFuelProduced = 0;

    chemical_list chemicalsCopy = {};
    chemicalsCopy.capacity = 100;
    chemicalsCopy.values = (chemical *)allocateSize(&memory, chemicalsCopy.capacity * sizeof(chemical));

    bool outOfOre = false;
    while (!outOfOre) {
        for (int i = 0; i < chemicals.numValues; ++i) {
            chemicalsCopy.values[i] = chemicals.values[i];
        }
        totalOreExpended = 0;
        outOfOre = produceChemical("FUEL", 1000, &chemicals, &totalOreAvailable, &totalOreExpended);
        if (outOfOre) {
            outOfOre = false;
            for (int i = 0; i < chemicals.numValues; ++i) {
                chemicals.values[i] = chemicalsCopy.values[i];
            }
            while (!outOfOre) {
                totalOreExpended = 0;
                outOfOre = produceChemical("FUEL", 1, &chemicals, &totalOreAvailable, &totalOreExpended);
                if (outOfOre) {
                    break;
                }
                else {
                    amountOfFuelProduced += 1;
                    totalOreAvailable = totalOreAvailable - totalOreExpended;
                }
            }
        }
        else {
            amountOfFuelProduced += 1000;
            totalOreAvailable = totalOreAvailable - totalOreExpended;
        }
        //printf("%lld\n", amountOfFuelProduced);
    }
    printf("%lld\n", totalOreExpended);
    printf("%lld\n", amountOfFuelProduced);

    return 0;
}
