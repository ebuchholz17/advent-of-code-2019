#include <math.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }
#define MAX_PROGRAM_LENGTH 20000

#include "win_console.cpp"

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

bool list_contains (int64_list *list, int64 value) {
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i] == value) {
            return true;
        }
    }
    return false;
}

int list_index_of (int64_list *list, int64 value) {
    int result = -1;
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i] == value) {
            result = i;
            break;
        }
    }
    return result;
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

enum opcode {
    OPCODE_ADD = 1,
    OPCODE_MULTIPLY = 2,
    OPCODE_INPUT = 3,
    OPCODE_OUTPUT = 4,
    OPCODE_JUMP_IF_TRUE = 5,
    OPCODE_JUMP_IF_FALSE = 6,
    OPCODE_LESS_THAN = 7,
    OPCODE_EQUAL = 8,
    OPCODE_ADJUST_RELATIVE_BASE = 9,
    OPCODE_HALT = 99
};

enum parameter_mode {
    PARAMETER_MODE_POSITION,
    PARAMETER_MODE_IMMEDIATE,
    PARAMETER_MODE_RELATIVE
};

enum program_result {
    REQUEST_INPUT,
    REQUEST_OUTPUT,
    PROGRAM_HALTED
};

struct computer {
    int64 program[MAX_PROGRAM_LENGTH];
    int64 instructionPointer;
    int64 relativeBase;
    int64_list input;
    int64_list output;
};

inline int64 getParameterValue (int64 param, int64 mode, int64 relativeBase, int64 *program) {
    int64 result = 0;
    switch (mode) {
        default: {
            ASSERT(0);
        } break;
        case PARAMETER_MODE_POSITION: {
            result = program[param];
        } break;
        case PARAMETER_MODE_IMMEDIATE: {
            result = param;
        } break;
        case PARAMETER_MODE_RELATIVE: {
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
        case PARAMETER_MODE_POSITION: {
            result = param;
        } break;
        case PARAMETER_MODE_IMMEDIATE: {
            ASSERT(0);
        } break;
        case PARAMETER_MODE_RELATIVE: {
            result = param + relativeBase;
        } break;
    }
    return result;
}

program_result runProgram (computer *comp) {
    int64 *program = comp->program;
    
    while (true) {
        int64 instruction = program[comp->instructionPointer];

        int64 param3Mode = instruction / 10000;
        int64 param2Mode = (instruction / 1000) % 10;
        int64 param1Mode = (instruction / 100) % 10;
        int64 opcode = instruction % 100;
        
        switch (opcode) {
            // addition
            case OPCODE_ADD: {
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
            case OPCODE_MULTIPLY: {
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
            case OPCODE_INPUT: {
                int64 param1 = program[comp->instructionPointer+1];
                int64 param1Value = getParameterWriteValue(param1, param1Mode, comp->relativeBase, program);

                // if input buffer is empty, pause to request input
                if (comp->input.numValues == 0) {
                    return REQUEST_INPUT;
                }
                else{
                    int64 inputValue = list_splice(&comp->input, 0);

                    program[param1Value] = inputValue;
                    comp->instructionPointer += 2;
                }
            } break;
            case OPCODE_OUTPUT: {
                int64 param1 = program[comp->instructionPointer+1];
                int64 param1Value = getParameterValue(param1, param1Mode, comp->relativeBase, program);

                list_push(&comp->output, param1Value);

                comp->instructionPointer += 2;
                return REQUEST_OUTPUT;
            } break;
            case OPCODE_JUMP_IF_TRUE: {
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
            case OPCODE_JUMP_IF_FALSE: {
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
            case OPCODE_LESS_THAN: {
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
            case OPCODE_EQUAL: {
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
            case OPCODE_ADJUST_RELATIVE_BASE: {
                int64 param1 = program[comp->instructionPointer+1];

                int64 param1Value = getParameterValue(param1, param1Mode, comp->relativeBase, program);
                comp->relativeBase += param1Value;

                comp->instructionPointer += 2;
            } break;
            // end of program
            case OPCODE_HALT: {
                goto halt;
            } break;
            default: {
                ASSERT(0);
            } break;
        }
    }

halt:
    return PROGRAM_HALTED;
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 2 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    int64 *program = (int64 *)allocateSize(&memory, MAX_PROGRAM_LENGTH * sizeof(int64));
    int numProgramPositions = 0;
    for (int64 i = 0; i < MAX_PROGRAM_LENGTH; ++i) {
        program[i] = 0;
    }

    const int stringLength = 8000;
    char *line = (char *)allocateSize(&memory, stringLength);
    char word[32];

    fgets(line, stringLength, stdin);

    char *cursor = line;
    while (*cursor != 0) {
        cursor = readUntilCharacter(cursor, word, ',');
        ++cursor;
        program[numProgramPositions] = _atoi64(word);
        ++numProgramPositions;
    }

    computer *comp = (computer *)allocateSize(&memory, sizeof(computer));
    comp->input.capacity = 10;
    comp->input.values = (int64 *)allocateSize(&memory, comp->input.capacity * sizeof(int64));
    comp->input.numValues = 0;

    comp->output.capacity = 10;
    comp->output.values = (int64 *)allocateSize(&memory, comp->output.capacity * sizeof(int64));
    comp->output.numValues = 0;
    comp->instructionPointer = 0;
    comp->relativeBase = 0;

    int64 *programCopy = comp->program;
    for (int64 k = 0; k < numProgramPositions; ++k) {
        programCopy[k] = program[k];
    }

    // init windows console 
    win_console winConsole;
    initWinConsole(&winConsole, 1000.0f / 60.0f);
    setWinConsoleOutputActive(&winConsole);

    int requiredWindowBufferSize = initWinConsoleBuffer(&winConsole, 100, 50);
    void *windowBuffer = allocateSize(&memory, requiredWindowBufferSize);
    setWindowBufferMemory(&winConsole, windowBuffer);

    startWinConsoleFrame(&winConsole);

    win_console_input input = {};

    const int mapWidth = 100;
    const int mapHalfWidth = 50;
    const int mapHeight = 100;
    const int mapHalfHeight = 50;
    char *map = (char *)allocateSize(&memory, mapWidth * mapHeight);
    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            map[i * mapWidth + j] = ' ';
        }
    }
    int xCoord = 0;
    int yCoord = 0;
    map[(yCoord + mapHalfHeight) * mapWidth + (xCoord + mapHalfWidth)] = '.';

    int windowWidth = 100;
    int windowHalfWidth = 50;
    int windowHeight = 50;
    int windowHalfHeight = 25;
    char *window = (char *)allocateSize(&memory, windowWidth * windowHeight);

    bool programRunning = true;
    int lastDirection = 0;
    while (programRunning) {
        getWinConsoleInput(&winConsole, &input);
        int direction = -1;
        if (input.upKey.justPressed) {
            direction = 1;
        }
        if (input.downKey.justPressed) {
            direction = 2;
        }
        if (input.leftKey.justPressed) {
            direction = 3;
        }
        if (input.rightKey.justPressed) {
            direction = 4;
        }

        program_result result = REQUEST_INPUT;

        result = runProgram(comp);
        switch (result) {
            case REQUEST_INPUT: {
                if (direction != -1) {
                    list_push(&comp->input, direction);
                    lastDirection = direction;
                }
            } break;
            case REQUEST_OUTPUT: {
                int64 output = list_splice(&comp->output, 0);
                int newXCoord = xCoord;
                int newYCoord = yCoord;
                switch (lastDirection) {
                case 1:
                    newYCoord -= 1;
                    break;
                case 2:
                    newYCoord += 1;
                    break;
                case 3:
                    newXCoord -= 1;
                    break;
                case 4:
                    newXCoord += 1;
                    break;
                }
                lastDirection = -1;
                if (output == 0) {
                    map[(newYCoord + mapHalfHeight) * mapWidth + (newXCoord + mapHalfWidth)] = '#';
                }
                else if (output == 1) {
                    map[(newYCoord + mapHalfHeight) * mapWidth + (newXCoord + mapHalfWidth)] = '.';
                    xCoord = newXCoord;
                    yCoord = newYCoord;
                }
                else {
                    map[(newYCoord + mapHalfHeight) * mapWidth + (newXCoord + mapHalfWidth)] = '*';
                    xCoord = newXCoord;
                    yCoord = newYCoord;
                }
            } break;
            case PROGRAM_HALTED: {
                programRunning = false;
            } break;
        }

        int xStartCoord = (xCoord + mapHalfWidth) - windowHalfWidth;
        int xEndCoord = (xCoord + mapHalfWidth) + windowHalfWidth - 1;
        int yStartCoord = (yCoord + mapHalfHeight) - windowHalfHeight;
        int yEndCoord = (yCoord + mapHalfHeight) + windowHalfHeight - 1;

        for (int i = 0; i < windowHeight; ++i) {
            for (int j = 0; j < windowWidth; ++j) {
                window[i * windowWidth + j] = map[((yStartCoord+i) * mapWidth) + (xStartCoord+j)];
            }
        }

        writeWinConsoleCharsToWholeScreen(&winConsole, window);
        writeWinConsoleCharAtXY(&winConsole, 'D', windowHalfWidth, windowHalfHeight);

        renderWinConsole(&winConsole);
        waitForEndOfWinConsoleFrame(&winConsole);
        startWinConsoleFrame(&winConsole);
        winConsoleResetInput(&input);
    }

    setWinConsoleStandardOutputActive(&winConsole);
    releaseWinConsoleHandles(&winConsole);

    // part 1 answer by interactively exploring map: 216

    return 0;
}
