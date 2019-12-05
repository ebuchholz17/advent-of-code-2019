#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

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

int runProgram (int *program, int_list *input, int_list *output) {
    int instructionPointer = 0;
    int inputPointer = 0;
    
    while (true) {
        int instruction = program[instructionPointer];

        int param3Mode = instruction / 10000;
        int param2Mode = (instruction / 1000) % 10;
        int param1Mode = (instruction / 100) % 10;
        int opcode = instruction % 100;
        
        switch (opcode) {
            // addition
            case ADD: {
                int param1 = program[instructionPointer+1];
                int param2 = program[instructionPointer+2];
                int param3 = program[instructionPointer+3];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);

                int result = param1Value + param2Value;

                program[param3] = result;
                instructionPointer += 4;
            } break;
            // multiplication
            case MULTIPLY: {
                int param1 = program[instructionPointer+1];
                int param2 = program[instructionPointer+2];
                int param3 = program[instructionPointer+3];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);

                int result = param1Value * param2Value;

                program[param3] = result;
                instructionPointer += 4;
            } break;
            case INPUT: {
                int param1 = program[instructionPointer+1];

                ASSERT(inputPointer < input->numValues);
                int inputValue = input->values[inputPointer];
                ++inputPointer;

                program[param1] = inputValue;
                instructionPointer += 2;
            } break;
            case OUTPUT: {
                int param1 = program[instructionPointer+1];
                int param1Value = getParameterValue(param1, param1Mode, program);

                list_push(output, param1Value);

                instructionPointer += 2;
            } break;
            case JUMP_IF_TRUE: {
                int param1 = program[instructionPointer+1];
                int param2 = program[instructionPointer+2];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);

                if (param1Value != 0) {
                    instructionPointer = param2Value;
                }
                else {
                    instructionPointer += 3;
                }

            } break;
            case JUMP_IF_FALSE: {
                int param1 = program[instructionPointer+1];
                int param2 = program[instructionPointer+2];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);

                if (param1Value == 0) {
                    instructionPointer = param2Value;
                }
                else {
                    instructionPointer += 3;
                }
            } break;
            case LESS_THAN: {
                int param1 = program[instructionPointer+1];
                int param2 = program[instructionPointer+2];
                int param3 = program[instructionPointer+3];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);
                //int param3Value = getParameterValue(param3, param3Mode, program);

                if (param1Value < param2Value) {
                    program[param3] = 1;
                }
                else {
                    program[param3] = 0;
                }
                instructionPointer += 4;
            } break;
            case EQUAL: {
                int param1 = program[instructionPointer+1];
                int param2 = program[instructionPointer+2];
                int param3 = program[instructionPointer+3];

                int param1Value = getParameterValue(param1, param1Mode, program);
                int param2Value = getParameterValue(param2, param2Mode, program);
                //int param3Value = getParameterValue(param3, param3Mode, program);

                if (param1Value == param2Value) {
                    program[param3] = 1;
                }
                else {
                    program[param3] = 0;
                }
                instructionPointer += 4;
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
    return program[0];
}

int main (int argc, char **argv) {
    const int maxProgramLength = 2000;
    int program[maxProgramLength];
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

    int_list input;
    input.capacity = 1000;
    input.values = (int *)malloc(input.capacity * sizeof(int));
    input.numValues = 0;

    list_push(&input, 5);

    int_list output;
    output.capacity = 1000;
    output.values = (int *)malloc(output.capacity * sizeof(int));
    output.numValues = 0;

    runProgram(program, &input, &output);

    for (int i = 0; i < output.numValues; ++i) {
        printf("%d\n", output.values[i]);
    }

    return 0;
}
