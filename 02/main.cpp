#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

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

int runProgram (int *program, int noun, int verb) {
    int instructionIndex = 0;
    
    program[1] = noun;
    program[2] = verb;

    while (true) {
        int currentInstruction = program[instructionIndex];
        switch (currentInstruction) {
            // addition
            case 1: {
                int a = program[instructionIndex+1];
                int b = program[instructionIndex+2];
                int location = program[instructionIndex+3];
                int result = program[a] + program[b];
                program[location] = result;
            } break;
            // multiplication
            case 2: {
                int a = program[instructionIndex+1];
                int b = program[instructionIndex+2];
                int location = program[instructionIndex+3];
                int result = program[a] * program[b];
                program[location] = result;
            } break;
            // end of program
            case 99: {
                goto done;
            } break;
            default: {
                ASSERT(0);
            } break;
        }
        instructionIndex += 4;
    }

done:
    return program[0];
}

int main (int argc, char **argv) {
    const int maxProgramLength = 2000;
    int program[maxProgramLength];
    int numProgramPositions = 0;

    const int stringLength = 1000;
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

    int noun;
    int verb;
    for (noun = 0; noun < 100; ++noun) {
        for (verb = 0; verb < 100; ++verb) {
            int programCopy[maxProgramLength];
            for (int i = 0; i < numProgramPositions; ++i) {
                programCopy[i] = program[i];
            }
            int result = runProgram(programCopy, noun, verb);
            if (result == 19690720) {
                goto searchDone;
            }
        }
    }
searchDone:

    printf("%d\n", 100 * noun + verb);

    return 0;
}
