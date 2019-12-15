struct point {
    int x;
    int y;

    int color;
};

point Point (int x, int y) {
    point result;
    result.x = x;
    result.y = y;
    result.color = 0;
    return result;
}

struct point_list {
    point *values;
    int numValues;
    int capacity;
};

void list_push (point_list *list, point value) {
    ASSERT(list->numValues < list->capacity);

    list->values[list->numValues] = value;
    list->numValues++;
}

bool list_contains (point_list *list, point value) {
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i].x == value.x && list->values[i].y == value.y) {
            return true;
        }
    }
    return false;
}

int list_index_of (point_list *list, point value) {
    int result = -1;
    for (int i = 0; i < list->numValues; ++i) {
        if (list->values[i].x == value.x && list->values[i].y == value.y) {
            result = i;
            break;
        }
    }
    return result;
}

point list_splice(point_list *list, int index) {
    ASSERT(index < list->numValues);

    point result = list->values[index];
    list->numValues--;
    for (int i = index; i < list->numValues; ++i) {
        list->values[i] = list->values[i+1];
    }

    return result;
}
