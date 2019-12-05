#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

struct point {
    int x;
    int y;
};

point Point (int x, int y) {
    point result;
    result.x = x;
    result.y = y;
    return result;
}

struct line_intersection {
    point intersection;
    int numSteps;
};

// TODO(ebuchholz): keep "running length" of wire "steps" is running length plus length along current line the
// intersection occurs
// TODO(ebuchholz): if a wire intersects itself, bisect the line: first halves of the lines are shortened, second halves
// get a new running length , whichever of the previous lines is shorter
struct line {
    point p0;
    point p1;
    int runningLength;
};

line Line (int x0, int y0, int x1, int y1) {
    line result;
    result.p0.x = x0;
    result.p0.y = y0;
    result.p1.x = x1;
    result.p1.y = y1;
    return result;
}

line Line (point p0, point p1) {
    line result;
    result.p0 = p0;
    result.p1 = p1;
    return result;
}

int lineLength (line l) {
    if (l.p0.x == l.p1.x) {
        int length = l.p1.y - l.p0.y;
        if (length < 0) {
            length = -length;
        }
        return length;
    }
    else {
        int length = l.p1.x - l.p0.x;
        if (length < 0) {
            length = -length;
        }
        return length;
    }
}

bool linesIntersect (line l0, line l1, point *result) {
    // l0 is vertical
    if (l0.p0.x == l0.p1.x) {
        // lines are parallel
        if (l1.p0.x == l1.p1.x) {
            // TODO(ebuchholz): do we need to handle this case?
        }
        // lines are perpendicular
        else {
            int l0x = l0.p0.x;
            int l1y = l1.p0.y;
            if ((l1.p0.x > l0x && l1.p1.x > l0x)) {
                return false;
            }
            if ((l1.p0.x < l0x && l1.p1.x < l0x)) {
                return false;
            }
            if ((l1y < l0.p0.y && l1y < l0.p1.y)) {
                return false;
            }
            if ((l1y > l0.p0.y && l1y > l0.p1.y)) {
                return false;
            }
            *result = Point(l0x, l1y);
            return true;
        }
    }
    // l0 must be horizontal
    else {
        // lines are parallel
        if (l1.p0.y == l1.p1.y) {
            // TODO(ebuchholz): do we need to handle this case?
        }
        // lines are perpendicular
        else {
            int l0y = l0.p0.y;
            int l1x = l1.p0.x;
            if ((l1.p0.y > l0y && l1.p1.y > l0y)) {
                return false;
            }
            if ((l1.p0.y < l0y && l1.p1.y < l0y)) {
                return false;
            }
            if ((l1x < l0.p0.x && l1x < l0.p1.x)) {
                return false;
            }
            if ((l1x > l0.p0.x && l1x > l0.p1.x)) {
                return false;
            }
            *result = Point(l1x, l0y);
            return true;
        }
    }
    return false;
}

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

    line firstWireLines[400];
    int numFirstWireLines = 0;
    char word[10];

    // first wire
    fgets(inputLine, stringLength, stdin);
    point currentPoint = Point(0, 0);
    char *cursor = inputLine;
    int runningLength = 0;
    while (*cursor != 0) {
        char direction = *cursor;
        ++cursor;

        cursor = readUntilCharacter(cursor, word, ',');
        ++cursor;
        int length = atoi(word);

        point nextPoint = currentPoint;
        switch (direction) {
            case 'U': {
                nextPoint.y -= length;
            } break;
            case 'D': {
                nextPoint.y += length;
            } break;
            case 'L': {
                nextPoint.x -= length;
            } break;
            case 'R': {
                nextPoint.x += length;
            } break;
            default: {
                ASSERT(0);
            } break;
        }

        line l = Line(currentPoint, nextPoint);
        l.runningLength = runningLength;
        firstWireLines[numFirstWireLines] = l;
        runningLength = runningLength + lineLength(l);
        ++numFirstWireLines;
        currentPoint = nextPoint;
    }

    line secondWireLines[400];
    int numSecondWireLines = 0;

    // second wire
    fgets(inputLine, stringLength, stdin);
    currentPoint = Point(0, 0);
    cursor = inputLine;
    runningLength = 0;
    while (*cursor != 0) {
        char direction = *cursor;
        ++cursor;

        cursor = readUntilCharacter(cursor, word, ',');
        ++cursor;
        int length = atoi(word);

        point nextPoint = currentPoint;
        switch (direction) {
            case 'U': {
                nextPoint.y -= length;
            } break;
            case 'D': {
                nextPoint.y += length;
            } break;
            case 'L': {
                nextPoint.x -= length;
            } break;
            case 'R': {
                nextPoint.x += length;
            } break;
            default: {
                ASSERT(0);
            } break;
        }

        line l = Line(currentPoint, nextPoint);
        l.runningLength = runningLength;
        secondWireLines[numSecondWireLines] = l;
        runningLength = runningLength + lineLength(l);
        ++numSecondWireLines;
        currentPoint = nextPoint;
    }

    line_intersection intersections[30000];
    int numIntersections = 0;
    for (int i = 0; i < numFirstWireLines; ++i) { 
        for (int j = 0; j < numSecondWireLines; ++j) { 
            line firstLine = firstWireLines[i];
            line secondLine = secondWireLines[j];

            point result;
            if (linesIntersect(firstLine, secondLine, &result)) {
                line_intersection lineIntersection;
                lineIntersection.intersection = result;
                line firstLineSegment = Line(firstLine.p0, result);
                line secondLineSegment = Line(secondLine.p0, result);
                int firstLineSteps = firstLine.runningLength + lineLength(firstLineSegment);
                int secondLineSteps = secondLine.runningLength + lineLength(secondLineSegment);
                lineIntersection.numSteps = firstLineSteps + secondLineSteps;
                intersections[numIntersections] = lineIntersection;
                numIntersections++;
            }
        }
    }

    int lowestSteps = 90000000;
    for (int i = 0; i < numIntersections; ++i) {
        line_intersection lineIntersection = intersections[i];
        // ignore origin
        if (lineIntersection.intersection.x == 0 && lineIntersection.intersection.y == 0) {
            continue;
        }
        if (lineIntersection.numSteps < lowestSteps) {
            lowestSteps = lineIntersection.numSteps;
        }
    }

    printf("%d\n", lowestSteps);

    return 0;
}
