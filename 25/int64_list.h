#ifndef INT64_H
#define INT64_H

#define LIST_TYPE int64
#include "list.h"

int listIndexOf (int64_list *list, int64 value) {
    int result = -1;
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i] == value) {
            result = i;
            break;
        }
    }
    return result;
}

#endif
