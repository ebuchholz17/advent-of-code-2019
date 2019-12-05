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

int main (int argc, char **argv) {
    const int stringLength = 2000;
    char inputLine[stringLength];

    char word[10];

    // first wire
    fgets(inputLine, stringLength, stdin);
    char *cursor = inputLine;

    cursor = readUntilCharacter(cursor, word, '-');
    ++cursor;

    int lowerBound = atoi(word);
    int upperBound = atoi(cursor);

    int numValidPWs = 0;
    for (int pw = lowerBound; pw <= upperBound; ++pw) {
        int digits[10];
        int currentDigit = 0;
        int nextPW = pw;
        while (nextPW > 0) {
            int digit = nextPW % 10;
            digits[currentDigit] = digit;
            ++currentDigit;
            nextPW /= 10;
        }
        --currentDigit;
        int lastDigit = -1;
        bool foundDouble = false;
        bool digitsCurrentlyMatch = false;
        bool validPW = true;
        bool doubleCurrentlyValid = false;
        while (currentDigit >= 0) {
            int digit = digits[currentDigit];
            if (digit == lastDigit) {
                if (digitsCurrentlyMatch) {
                    doubleCurrentlyValid = false;
                }
                else {
                    doubleCurrentlyValid = true;
                    digitsCurrentlyMatch = true;
                }
            }
            else {
                if (digitsCurrentlyMatch) {
                    if (doubleCurrentlyValid) {
                        foundDouble = true;
                    }
                }
                digitsCurrentlyMatch = false;
                doubleCurrentlyValid = false;
            }
            if (digit < lastDigit) {
                validPW = false;
                break;
            }
            lastDigit = digit;
            --currentDigit;
        }
        if (digitsCurrentlyMatch) {
            if (doubleCurrentlyValid) {
                foundDouble = true;
            }
        }
        if (!foundDouble) {
            validPW = false;
        }
        if (validPW) {
            printf("%d\n", pw);
            ++numValidPWs;
        }
    }

    printf("%d\n", numValidPWs);

    return 0;
}
