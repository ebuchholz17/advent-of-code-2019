#include "advent.h"
#include "bigint.h"

//#define NUM_CARDS 10007L
#define NUM_CARDS 119315717514047L

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

enum shuffle_type {
    SHUFFLE_TYPE_DEAL_NEW_STACK,
    SHUFFLE_TYPE_DEAL_WITH_INC,
    SHUFFLE_TYPE_CUT
};

struct shuffle_step {
    shuffle_type type;
    int64 amount;
};
#define LIST_TYPE shuffle_step
#include "list.h"

struct tuple {
    int64 a;
    int64 b;
};
#define LIST_TYPE tuple
#include "list.h"

bool stringsAreEqual (char *a, char *b) {
    int i, j;
    for (i = 0, j = 0; a[i] && b[j]; ++i, ++j) {
        if (a[i] != b[j]) { 
            return false;
        }
    }
    return a[i] == 0 && b[j] == 0;
}

void dealIntoNewStack (int64 *deck, int64 *tempDeck) {
    for (int64 i = 0; i < NUM_CARDS; ++i) {
        tempDeck[(NUM_CARDS - 1) - i] = deck[i];
    }
}

void dealWithIncrement (int64 *deck, int64 *tempDeck, int64 increment) {
    int64 index = 0;
    for (int64 i = 0; i < NUM_CARDS; ++i) {
        tempDeck[index] = deck[i];
        index += increment;
        index = index % NUM_CARDS;
    }
}

void cutDeck (int64 *deck, int64 *tempDeck, int64 location) {
    for (int64 i = 0; i < NUM_CARDS; ++i) {
        int64 index = i + location;
        if (index < 0) {
            index += NUM_CARDS;
        }
        index = index % NUM_CARDS;
        tempDeck[i] = deck[index];
    }
}

tuple shuffleStepToTuple (shuffle_step step) {
    tuple result = {};
    switch (step.type) {
        case SHUFFLE_TYPE_DEAL_NEW_STACK: {
            result.a = -1;
            result.b = -1;
        } break;
        case SHUFFLE_TYPE_DEAL_WITH_INC: {
            result.a = step.amount;
            result.b = 0;
        } break;
        case SHUFFLE_TYPE_CUT: {
            result.a = 1;
            result.b = -step.amount;
        } break;
    }
    return result;
}

int64 mod(int64 x, int64 m) {
    int64 result = x % m;
    if (result < 0) {
        result += m;
    }
    return result;
}


int64 mulmod(int64 a, int64 b, int64 m) { 
    int64 res = 0; 
    a = mod(a, m); 
    while (b > 0) { 
        if (mod(b, 2) == 1) {
            res = mod((res + a), m); 
        }
  
        a = mod((a * 2), m); 
  
        b /= 2; 
    } 
  
    // Return result 
    return mod(res, m); 
} 

int64 mul_mod(int64 x, int64 y, int64 m) {
    if (y > 0) {
        return mulmod(x, y, m);
    }
    else {
        int64 result = 0;
        int64 sign = y < 0 ? -1 : 1;
        if (sign == -1) {
            for (int64 i = 0; i < -y; ++i) {
                result = mod(result - x, m);
            }
        }
        else {
            for (int64 i = 0; i < y; ++i) {
                result = mod(result + x, m);
            }
        }
        return result;
    }
}

int64 pow_mod(int64 x, int64 n, int64 m) {
    if (n == 0) {
        return 1; 
    } 
    int64 y = 1;
    while (n > 1) {
        if (n % 2 == 0) {
            x = mulmod(x, x, m);
            n = n / 2;
        }
        else {
            y = mulmod(x , y, m);
            x = mulmod(x , x, m);
            n = (n - 1) / 2;
        }
    }
    return mulmod(x , y, m);
}

// a⋅b−1 mod m
// a−1=am−2 mod m when m is prime
int64 div_mod (int64 x, int64 y, int64 m) {
    int64 b = pow_mod(y, m - 2, m);
    return mul_mod(x, b, m);
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    //int64 *deck = (int64 *)allocateSize(&memory, sizeof(int64) * NUM_CARDS);
    //int64 *tempDeck = (int64 *)allocateSize(&memory, sizeof(int64) * NUM_CARDS);
    //for (int64 i = 0; i < NUM_CARDS; ++i) {
    //    deck[i] = i;
    //    tempDeck[i] = i;
    //}

    const int stringLength = 30;
    char *line = (char *)allocateSize(&memory, stringLength);
    char word[32];

    shuffle_step_list steps = shuffle_stepListInit(&memory, 200);
    while (fgets(line, stringLength, stdin)) {
        char *cursor = line;
        cursor = readUntilCharacter(cursor, word, ' ');
        ++cursor;

        shuffle_step step = {};
        if (stringsAreEqual(word, "cut")) {
            step.type = SHUFFLE_TYPE_CUT;
            cursor = readUntilCharacter(cursor, word, '\n');
            step.amount = atoi(word);
        }
        else {
            cursor = readUntilCharacter(cursor, word, ' ');
            ++cursor;
            if (stringsAreEqual(word, "into")) {
                step.type = SHUFFLE_TYPE_DEAL_NEW_STACK;
                step.amount = 0;
            }
            else {
                step.type = SHUFFLE_TYPE_DEAL_WITH_INC;
                cursor = readUntilCharacter(cursor, word, ' ');
                ++cursor;
                cursor = readUntilCharacter(cursor, word, '\n');
                step.amount = atoi(word);
            }
        }
        listPush(&steps, step);
    }

    // solved thanks to: https://codeforces.com/blog/entry/72593
    tuple_list tuples = tupleListInit(&memory, 200);
    for (int i = 0; i < steps.numValues; ++i) {
        tuple stepTuple = shuffleStepToTuple(steps.values[i]);
        listPush(&tuples, stepTuple);
    }

    // compose all the functions to a single function
    tuple composedTuple = tuples.values[0];
    for (int i = 1; i < tuples.numValues; ++i) {
        tuple stepTuple = tuples.values[i];
        composedTuple.a = mul_mod(composedTuple.a, stepTuple.a, NUM_CARDS);
        composedTuple.b = mul_mod(composedTuple.b, stepTuple.a, NUM_CARDS);
        composedTuple.b = mod(composedTuple.b + stepTuple.b, NUM_CARDS);
    }

    // use with numcards = 10007 to get original answer
    int64 cardPos = mul_mod(composedTuple.a, 2019, NUM_CARDS);
    cardPos = mod(cardPos + composedTuple.b, NUM_CARDS);

    int64 k = 101741582076661L;
    
    // compose k times via: Fk(x)=a^k * x + b(1−a^k)/(1−a)  mod m
    tuple repeatedTuple = {};
    int64 ak = pow_mod(composedTuple.a, k, NUM_CARDS);
    repeatedTuple.a = ak;

    int64 numerator = mul_mod(composedTuple.b, mod(1 - ak, NUM_CARDS), NUM_CARDS);
    int64 denom = mod(1 - composedTuple.a, NUM_CARDS);
    repeatedTuple.b = div_mod(numerator, denom, NUM_CARDS);

    // invert via: F−k(x)=x−B/A  mod m  where x is 2020, the card pos we want
    int64 invertedNumerator = mod(2020L - repeatedTuple.b, NUM_CARDS);
    int64 answer = div_mod(invertedNumerator, repeatedTuple.a, NUM_CARDS);

    printf("%lld\n", answer);
    return 0;
}
