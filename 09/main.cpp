#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }
#define MAX_PROGRAM_LENGTH 200000

typedef long long int64;
typedef unsigned long long uint64;

enum opcode {
    ADD = 1,
    MULTIPLY = 2,
    INPUT = 3,
    OUTPUT = 4,
    JUMP_IF_TRUE = 5,
    JUMP_IF_FALSE = 6,
    LESS_THAN = 7,
    EQUAL = 8,
    ADJUST_RELATIVE_BASE = 9,
    HALT = 99
};

enum parameter_mode {
    POSITION,
    IMMEDIATE,
    RELATIVE
};

struct int64_list {
    int64 *values;
    int numValues;
    int capacity;
};

void list_push (int64_list *list, int64 value) {
    ASSERT(list->numValues < list->capacity);

    list->values[list->numValues] = value;
    list->numValues++;
}

int64 list_splice(int64_list *list, int index) {
    ASSERT(index < list->numValues);

    int64 result = list->values[index];
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

int64_list *list_copy (int64_list *source, memory_arena *memory) {
    int64_list *result = (int64_list *)allocateSize(memory, sizeof(int64_list) + source->capacity * sizeof(int64));
    result->values = (int64 *)((char *)result + sizeof(int64_list)); 
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

inline int64 getParameterValue (int64 param, int64 mode, int64 relativeBase, int64 *program) {
    int64 result = 0;
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
        case RELATIVE: {
            result = program[param + relativeBase];
        } break;
    }
    return result;
}

inline int64 getParameterWriteValue (int64 param, int64 mode, int64 relativeBase, int64 *program) {
    int64 result = 0;
    switch (mode) {
        default: {
            ASSERT(0);
        } break;
        case POSITION: {
            result = param;
        } break;
        case IMMEDIATE: {
            ASSERT(0);
        } break;
        case RELATIVE: {
            result = param + relativeBase;
        } break;
    }
    return result;
}

struct computer {
    int64 program[MAX_PROGRAM_LENGTH];
    int64 instructionPointer;
    int64 relativeBase;
    int64_list input;
    int64_list output;
};

bool runProgram (computer *comp) {
    int64 inputPointer = 0;

    int64 *program = comp->program;
    
    while (true) {
        int64 instruction = program[comp->instructionPointer];

        int64 param3Mode = instruction / 10000;
        int64 param2Mode = (instruction / 1000) % 10;
        int64 param1Mode = (instruction / 100) % 10;
        int64 opcode = instruction % 100;
        
        switch (opcode) {
            // addition
            case ADD: {
                int64 param1 = program[comp->instructionPointer+1];
                int64 param2 = program[comp->instructionPointer+2];
                int64 param3 = program[comp->instructionPointer+3];

                int64 param1Value = getParameterValue(param1, param1Mode, comp->relativeBase, program);
                int64 param2Value = getParameterValue(param2, param2Mode, comp->relativeBase, program);
                int64 param3Value = getParameterWriteValue(param3, param3Mode, comp->relativeBase, program);

                int64 result = param1Value + param2Value;

                program[param3Value] = result;
                comp->instructionPointer += 4;
            } break;
            // multiplication
            case MULTIPLY: {
                int64 param1 = program[comp->instructionPointer+1];
                int64 param2 = program[comp->instructionPointer+2];
                int64 param3 = program[comp->instructionPointer+3];

                int64 param1Value = getParameterValue(param1, param1Mode, comp->relativeBase, program);
                int64 param2Value = getParameterValue(param2, param2Mode, comp->relativeBase, program);
                int64 param3Value = getParameterWriteValue(param3, param3Mode, comp->relativeBase, program);

                int64 result = param1Value * param2Value;

                program[param3Value] = result;
                comp->instructionPointer += 4;
            } break;
            case INPUT: {
                int64 param1 = program[comp->instructionPointer+1];
                int64 param1Value = getParameterWriteValue(param1, param1Mode, comp->relativeBase, program);

                ASSERT(inputPointer < comp->input.numValues);
                int64 inputValue = comp->input.values[inputPointer];
                ++inputPointer;

                program[param1Value] = inputValue;
                comp->instructionPointer += 2;
            } break;
            case OUTPUT: {
                int64 param1 = program[comp->instructionPointer+1];
                int64 param1Value = getParameterValue(param1, param1Mode, comp->relativeBase, program);

                list_push(&comp->output, param1Value);

                comp->instructionPointer += 2;
                //return false;
            } break;
            case JUMP_IF_TRUE: {
                int64 param1 = program[comp->instructionPointer+1];
                int64 param2 = program[comp->instructionPointer+2];

                int64 param1Value = getParameterValue(param1, param1Mode, comp->relativeBase, program);
                int64 param2Value = getParameterValue(param2, param2Mode, comp->relativeBase, program);

                if (param1Value != 0) {
                    comp->instructionPointer = param2Value;
                }
                else {
                    comp->instructionPointer += 3;
                }

            } break;
            case JUMP_IF_FALSE: {
                int64 param1 = program[comp->instructionPointer+1];
                int64 param2 = program[comp->instructionPointer+2];

                int64 param1Value = getParameterValue(param1, param1Mode, comp->relativeBase, program);
                int64 param2Value = getParameterValue(param2, param2Mode, comp->relativeBase, program);

                if (param1Value == 0) {
                    comp->instructionPointer = param2Value;
                }
                else {
                    comp->instructionPointer += 3;
                }
            } break;
            case LESS_THAN: {
                int64 param1 = program[comp->instructionPointer+1];
                int64 param2 = program[comp->instructionPointer+2];
                int64 param3 = program[comp->instructionPointer+3];

                int64 param1Value = getParameterValue(param1, param1Mode, comp->relativeBase, program);
                int64 param2Value = getParameterValue(param2, param2Mode, comp->relativeBase, program);
                int64 param3Value = getParameterWriteValue(param3, param3Mode, comp->relativeBase, program);

                if (param1Value < param2Value) {
                    program[param3Value] = 1;
                }
                else {
                    program[param3Value] = 0;
                }
                comp->instructionPointer += 4;
            } break;
            case EQUAL: {
                int64 param1 = program[comp->instructionPointer+1];
                int64 param2 = program[comp->instructionPointer+2];
                int64 param3 = program[comp->instructionPointer+3];

                int64 param1Value = getParameterValue(param1, param1Mode, comp->relativeBase, program);
                int64 param2Value = getParameterValue(param2, param2Mode, comp->relativeBase, program);
                int64 param3Value = getParameterWriteValue(param3, param3Mode, comp->relativeBase, program);

                if (param1Value == param2Value) {
                    program[param3Value] = 1;
                }
                else {
                    program[param3Value] = 0;
                }
                comp->instructionPointer += 4;
            } break;
            case ADJUST_RELATIVE_BASE: {
                int64 param1 = program[comp->instructionPointer+1];

                int64 param1Value = getParameterValue(param1, param1Mode, comp->relativeBase, program);
                comp->relativeBase += param1Value;

                comp->instructionPointer += 2;
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

int main (int argc, char **argv) {
    int64 *program = (int64 *)malloc(MAX_PROGRAM_LENGTH * sizeof(int64));
    int numProgramPositions = 0;
    for (int64 i = 0; i < MAX_PROGRAM_LENGTH; ++i) {
        program[i] = 0;
    }

    const int stringLength = 5000;
    char line[stringLength];

    char word[100];

    fgets(line, stringLength, stdin);

    char *cursor = line;
    while (*cursor != 0) {
        cursor = readUntilCharacter(cursor, word, ',');
        ++cursor;
        program[numProgramPositions] = _atoi64(word);
        ++numProgramPositions;
    }

    computer *comp = (computer *)malloc(sizeof(computer));
    comp->input.capacity = 10000;
    comp->input.values = (int64 *)malloc(comp->input.capacity * sizeof(int64));
    comp->input.numValues = 0;

    comp->output.capacity = 10000;
    comp->output.values = (int64 *)malloc(comp->output.capacity * sizeof(int64));
    comp->output.numValues = 0;
    comp->instructionPointer = 0;
    comp->relativeBase = 0;

    int64 *programCopy = comp->program;
    for (int64 k = 0; k < numProgramPositions; ++k) {
        programCopy[k] = program[k];
    }

    //list_push(&comp->input, 1);
    list_push(&comp->input, 2);

    comp->output.numValues = 0;

    runProgram(comp);
    comp->input.numValues = 0;

    for (int64 i =0; i < comp->output.numValues; ++i) {
        printf("%lld\n", comp->output.values[i]);
    }

    return 0;
}
