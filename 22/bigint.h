#ifndef BIGINT_H
#define BIGINT_H

#define BIGINT_SIZE 100

struct bigint {
    char digits[BIGINT_SIZE];
};

bigint bigintFromInt (int num) {
    bigint result = {};
    int index = BIGINT_SIZE - 1;
    while (num > 0) {
        char digit = num % 10;
        result.digits[index] = digit;
        num /= 10;
        --index;
    }
    return result;
}

bigint bigintAdd (bigint a, bigint b) {
    bigint result = {};
    char carry = 0;
    for (int i = BIGINT_SIZE - 1; i >= 0; --i) {
        char sum = a.digits[i] + b.digits[i] + carry;
        if (sum > 10) {
            sum -= 10;
            carry = 1;
        }
        else {
            carry = 0;
        }
        result.digits[i] = sum;
    }
    return result;
}

bigint bigintMultiply (bigint a, bigint b) {
    bigint result = {};
    char carry = 0;
    for (int i = BIGINT_SIZE - 1; i >= 0; --i) {
        char sum = a.digits[i] + b.digits[i] + carry;
        if (sum > 10) {
            sum -= 10;
            carry = 1;
        }
        else {
            carry = 0;
        }
        result.digits[i] = sum;
    }
    return result;
}

//bigint bigintSubtract (bigint a, bigint b) {
//    bigint result = {};
//    bool borrow = false;
//    for (int i = BIGINT_SIZE - 1; i >= 0; --i) {
//        char num = a.digits[i] + b.digits[i];
//        if (num > 10) {
//            num -= 10;
//            carry = 1;
//        }
//        else {
//            carry = 0;
//        }
//        result.digits[i] = num;
//    }
//    return result;
//}

void bigintToString (bigint a, char *string) {
    char *letter = string;
    bool leadingZeroes = true;
    for (int i = 0; i < BIGINT_SIZE; ++i) {
        if (leadingZeroes && a.digits[i] == 0) {
            continue;
        }
        else {
            leadingZeroes = false;
        }
        *letter = a.digits[i] + '0';
        ++letter;
    }
    *letter = 0;
}

#endif
