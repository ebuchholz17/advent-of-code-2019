#ifndef PORTAL_LIST_H
#define PORTAL_LIST_H

struct portal {
    char name[3];
    point pos0;
    point pos1;
    bool pos0IsOuter;
};

#define LIST_TYPE portal
#include "list.h"

int listIndexByPos (portal_list *list, point pos, bool *firstPointMatch) {
    int result = -1;
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i].pos0.x == pos.x && list->values[i].pos0.y == pos.y) { 
            result = i;
            *firstPointMatch = true;
            break;
        }
        else if (list->values[i].pos1.x == pos.x && list->values[i].pos1.y == pos.y) { 
            result = i;
            *firstPointMatch = false;
            break;
        }
    }
    return result;
}

int listIndexByName (portal_list *list, char *name) {
    int result = -1;
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i].name[0] == name[0] && list->values[i].name[1] == name[1]) {
            result = i;
            break;
        }
    }
    return result;
}

#endif
