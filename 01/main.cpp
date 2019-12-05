#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

int main (int argc, char **argv) {
    int masses[100];
    int numMasses = 0;

    const int stringLength = 10;
    char line[stringLength];

    while (fgets(line, stringLength, stdin)) {
        int num = atoi(line);
        masses[numMasses] = num;
        ++numMasses;
    }

    int sum = 0;
    for (int i = 0; i < numMasses; ++i) {
        int remainder = masses[i];
        int total = 0;
        while (true) {
            remainder /= 3;
            remainder -= 2;
            if (remainder < 0) {
                masses[i] = total;
                break;
            }
            else {
                total += remainder;
            }
        }
    }

    for (int i = 0; i < numMasses; ++i) {
        sum += masses[i];
    }
    printf("%d\n", sum);

    return 0;
}
