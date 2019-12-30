#include "advent.h"
#include "int64_list.h"

#define MAX_PROGRAM_LENGTH 20000

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
                    int64 inputValue = listSplice(&comp->input, 0);

                    program[param1Value] = inputValue;
                    comp->instructionPointer += 2;
                }
            } break;
            case OPCODE_OUTPUT: {
                int64 param1 = program[comp->instructionPointer+1];
                int64 param1Value = getParameterValue(param1, param1Mode, comp->relativeBase, program);

                listPush(&comp->output, param1Value);

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

void pushStringIntoInput (int64_list *input, char *string) {
    char *currentLetter = string;
    while (*currentLetter != 0) {
        listPush(input, *currentLetter);
        ++currentLetter;
    }
    listPush(input, '\n');
}

struct packet {
    int64 x;
    int64 y;
};
#define LIST_TYPE packet
#include "list.h"

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 100 * 1024 * 1024;
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

    int numComputers = 50;
    computer *computers = (computer *)allocateSize(&memory, sizeof(computer) * numComputers);
    for (int i = 0; i < numComputers; ++i) {
        computer *comp = &computers[i];
        comp->input.capacity = 200;
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
    }

    // assign ids
    for (int i = 0; i < numComputers; ++i) {
        computer *comp = &computers[i];
        bool programRunning = true;
        while (programRunning) {
            program_result result = runProgram(comp);
            switch (result) {
                case REQUEST_INPUT: {
                    listPush(&comp->input, i);
                    programRunning = false;
                } break;
                case REQUEST_OUTPUT: {
                    ASSERT(0);
                } break;
                case PROGRAM_HALTED: {
                    ASSERT(0);
                } break;
            }
        }
    }

    packet_list *packetQueues = (packet_list *)allocateSize(&memory, numComputers * sizeof(packet_list));
    for (int i = 0; i < numComputers; ++i) {
        packet_list *packetQueue = &packetQueues[i];
        *packetQueue = packetListInit(&memory, 100);
    }

    packet natPacket = {};
    int64 lastY = -17;
    while (true) {
        bool idle = true;

        for (int i = 0; i < numComputers; ++i) {
            computer *comp = &computers[i];
            bool programRunning = true;

            bool receivingInput = false;
            packet inputPacket = {};

            bool receivingOutput = false;
            int64 targetComputer = -1;
            packet outputPacket = {};
            bool processedX = false;

            while (programRunning) {
                program_result result = runProgram(comp);
                switch (result) {
                    case REQUEST_INPUT: {
                        if (!receivingInput) {
                            packet_list *packetQueue = &packetQueues[i];
                            if (packetQueue->numValues == 0) {
                                listPush(&comp->input, -1);
                                programRunning = false;
                            }
                            else {
                                idle = false;
                                receivingInput = true;
                                inputPacket = listSplice(packetQueue, 0);
                                listPush(&comp->input, inputPacket.x);
                            }
                        }
                        else {
                            listPush(&comp->input, inputPacket.y);
                            programRunning = false;
                        }
                    } break;
                    case REQUEST_OUTPUT: {
                        idle = false;
                        if (!receivingOutput) {
                            receivingOutput = true;

                            int64 output = listSplice(&comp->output, 0);
                            targetComputer = output;
                        }
                        else {
                            if (!processedX) {
                                processedX = true;
                                int64 output = listSplice(&comp->output, 0);
                                outputPacket.x = output;
                            }
                            else {
                                int64 output = listSplice(&comp->output, 0);
                                outputPacket.y = output;
                                if (targetComputer == 255) {
                                    natPacket = outputPacket;
                                    //printf("%lld\n", output);
                                    //goto done;
                                }
                                else {
                                    packet_list *packetQueue = &packetQueues[targetComputer];
                                    listPush(packetQueue, outputPacket);
                                }
                                programRunning = false;
                            }
                        }
                    } break;
                    case PROGRAM_HALTED: {
                        ASSERT(0);
                    } break;
                }
            }
        }

        if (idle) {
            packet_list *packetQueue = &packetQueues[0];
            listPush(packetQueue, natPacket);
            if (lastY == natPacket.y) {
                printf("%lld\n", lastY);
                break;
            }
            else {
                lastY = natPacket.y;
            }
        }
    }
//done:


    return 0;
}
