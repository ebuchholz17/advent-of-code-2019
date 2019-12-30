#ifndef POINT_H
#define POINT_H

struct point {
    int x;
    int y;
    int steps;
    int level;
};

#define LIST_TYPE point
#include "list.h"

int listIndexOf (point_list *list, point value) {
    int result = -1;
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i].x == value.x && list->values[i].y == value.y && list->values[i].level == value.level) { 
            result = i;
            break;
        }
    }
    return result;
}

struct point_hash_map {
    point *values;
    int capacity;
};



#endif
