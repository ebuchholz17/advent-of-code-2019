#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }
#define MAX_PROGRAM_LENGTH 2000

enum opcode {
    ADD = 1,
    MULTIPLY = 2,
    INPUT = 3,
    OUTPUT = 4,
    JUMP_IF_TRUE = 5,
    JUMP_IF_FALSE = 6,
    LESS_THAN = 7,
    EQUAL = 8,
    HALT = 99
};

enum parameter_mode {
    POSITION,
    IMMEDIATE
};

struct int_list {
    int *values;
    int numValues;
    int capacity;
};

void list_push (int_list *list, int value) {
    ASSERT(list->numValues < list->capacity);

    list->values[list->numValues] = value;
    list->numValues++;
}

int list_splice(int_list *list, int index) {
    ASSERT(index < list->numValues);

    int result = list->values[index];
    list->numValues--;
    for (int i = index; i < list->numValues; ++i) {
        list->values[i] = list->values[i+1];
    }

    return result;
}

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

int_list *list_copy (int_list *source, memory_arena *memory) {
    int_list *result = (int_list *)allocateSize(memory, sizeof(int_list) + source->capacity * sizeof(int));
    result->values = (int *)((char *)result + sizeof(int_list)); 
    result->numValues = source->numValues;
    result->capacity = source->capacity;
    for (int i = 0; i < source->numValues; ++i) {
        result->values[i] = source->values[i];
    }
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

inline int getParameterValue (int param, int mode, int *program) {
    int result = 0;
    switch (mode) {
        default: {
            ASSERT(0);
        } break;
        case POSITION: {
            result = program[param];
        } break;
        case IMMEDIATE: {
            result = param;
        } break;
    }
    return result;
}

struct computer {
    int program[MAX_PROGRAM_LENGTH];
    int instructionPointer;
    int_list input;
    int_list output;
};

bool runProgram (computer *comp) {
    int inputPointer = 0;

    int *program = comp->program;
    
    while (true) {
        int instruction = program[comp->instructionPointer];

        int param3Mode = instruction / 10000;
        int param2Mode = (instruction / 1000) % 10;
        int param1Mode = (instruction / 100) % 10;
        int opcode = instruction % 100;
        
        switch (opcode) {
            // addition
            case ADD: {
                int param1 = program[comp->instructionPointer+1];
                int param2 = program[comp->instructionPointer+2];
                int param3 = program[comp->instructionPointer+3];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);

                int result = param1Value + param2Value;

                program[param3] = result;
                comp->instructionPointer += 4;
            } break;
            // multiplication
            case MULTIPLY: {
                int param1 = program[comp->instructionPointer+1];
                int param2 = program[comp->instructionPointer+2];
                int param3 = program[comp->instructionPointer+3];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);

                int result = param1Value * param2Value;

                program[param3] = result;
                comp->instructionPointer += 4;
            } break;
            case INPUT: {
                int param1 = program[comp->instructionPointer+1];

                ASSERT(inputPointer < comp->input.numValues);
                int inputValue = comp->input.values[inputPointer];
                ++inputPointer;

                program[param1] = inputValue;
                comp->instructionPointer += 2;
            } break;
            case OUTPUT: {
                int param1 = program[comp->instructionPointer+1];
                int param1Value = getParameterValue(param1, param1Mode, program);

                list_push(&comp->output, param1Value);

                comp->instructionPointer += 2;
                return false;
            } break;
            case JUMP_IF_TRUE: {
                int param1 = program[comp->instructionPointer+1];
                int param2 = program[comp->instructionPointer+2];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);

                if (param1Value != 0) {
                    comp->instructionPointer = param2Value;
                }
                else {
                    comp->instructionPointer += 3;
                }

            } break;
            case JUMP_IF_FALSE: {
                int param1 = program[comp->instructionPointer+1];
                int param2 = program[comp->instructionPointer+2];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);

                if (param1Value == 0) {
                    comp->instructionPointer = param2Value;
                }
                else {
                    comp->instructionPointer += 3;
                }
            } break;
            case LESS_THAN: {
                int param1 = program[comp->instructionPointer+1];
                int param2 = program[comp->instructionPointer+2];
                int param3 = program[comp->instructionPointer+3];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);
                //int param3Value = getParameterValue(param3, param3Mode, program);

                if (param1Value < param2Value) {
                    program[param3] = 1;
                }
                else {
                    program[param3] = 0;
                }
                comp->instructionPointer += 4;
            } break;
            case EQUAL: {
                int param1 = program[comp->instructionPointer+1];
                int param2 = program[comp->instructionPointer+2];
                int param3 = program[comp->instructionPointer+3];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);
                //int param3Value = getParameterValue(param3, param3Mode, program);

                if (param1Value == param2Value) {
                    program[param3] = 1;
                }
                else {
                    program[param3] = 0;
                }
                comp->instructionPointer += 4;
            } break;
            // end of program
            case HALT: {
                goto halt;
            } break;
            default: {
                ASSERT(0);
            } break;
        }
    }

halt:
    return true;
}

void calculateCombinations(int_list *possibleValues, int_list *currentValues, 
                           memory_arena *memory, int_list **combinations, int *numCombinations) 
{
    if (currentValues->numValues == 5) {
        ASSERT(*numCombinations < 200);
        combinations[*numCombinations] = currentValues;
        (*numCombinations)++;
    }
    else {
        for (int i = 0; i < possibleValues->numValues; ++i) {
            int_list *newPossibleValues = list_copy(possibleValues, memory);
            int nextNum = list_splice(newPossibleValues, i);

            int_list *newCurrentValues = list_copy(currentValues, memory);
            list_push(newCurrentValues, nextNum);

            calculateCombinations(newPossibleValues, newCurrentValues, memory, combinations, numCombinations);
        }
    }
}

int main (int argc, char **argv) {
    int program[MAX_PROGRAM_LENGTH];
    int numProgramPositions = 0;

    const int stringLength = 5000;
    char line[stringLength];

    char word[10];

    fgets(line, stringLength, stdin);

    char *cursor = line;
    while (*cursor != 0) {
        cursor = readUntilCharacter(cursor, word, ',');
        ++cursor;
        program[numProgramPositions] = atoi(word);
        ++numProgramPositions;
    }

    memory_arena listMemory = {};
    listMemory.capacity = (sizeof(int_list) + 5 * sizeof(int)) * 1000 + (sizeof(int) * 10);
    listMemory.base = malloc(listMemory.capacity);

    int_list possibleValues = {};
    possibleValues.capacity = 5;
    possibleValues.values = (int *)allocateSize(&listMemory, sizeof(int) * possibleValues.capacity);
    list_push(&possibleValues, 5);
    list_push(&possibleValues, 6);
    list_push(&possibleValues, 7);
    list_push(&possibleValues, 8);
    list_push(&possibleValues, 9);

    int_list currentValues = {};
    currentValues.capacity = 5;
    currentValues.values = (int *)allocateSize(&listMemory, sizeof(int) * possibleValues.capacity);

    int_list **combinations = (int_list **)malloc(sizeof(int_list *) * 200);
    int numCombinations = 0;

    calculateCombinations(&possibleValues, &currentValues, &listMemory, combinations, &numCombinations);

    computer computers[5];
    for (int i = 0; i < 5; ++i) {
        computers[i] = {};
        computers[i].input.capacity = 10;
        computers[i].input.values = (int *)malloc(computers[i].input.capacity * sizeof(int));
        computers[i].input.numValues = 0;

        computers[i].output.capacity = 10;
        computers[i].output.values = (int *)malloc(computers[i].output.capacity * sizeof(int));
        computers[i].output.numValues = 0;
    }

    int largestSignal = 0;
    for (int i = 0; i < numCombinations; ++i) {
        int signal = 0;

        list_push(&computers[0].input, combinations[i]->values[0]);
        list_push(&computers[1].input, combinations[i]->values[1]);
        list_push(&computers[2].input, combinations[i]->values[2]);
        list_push(&computers[3].input, combinations[i]->values[3]);
        list_push(&computers[4].input, combinations[i]->values[4]);

        list_push(&computers[0].input, signal);
        
        for (int j = 0; j < 5; ++j) {
            int *programCopy = computers[j].program;
            for (int k = 0; k < numProgramPositions; ++k) {
                programCopy[k] = program[k];
            }
            computers[j].instructionPointer = 0;
        }

        bool halted = false;
        while (true) {
            for (int j = 0; j < 5; ++j) {
                computer *comp = &computers[j];
                list_push(&comp->input, signal);

                comp->output.numValues = 0;

                halted = runProgram(comp);
                comp->input.numValues = 0;

                signal = comp->output.values[0];
            }
            if (halted) {
                break;
            }
        }
        printf("%d\n", signal);
        if (signal > largestSignal) {
            largestSignal = signal;
        }
    }

    printf("%d\n", largestSignal);

    return 0;
}
