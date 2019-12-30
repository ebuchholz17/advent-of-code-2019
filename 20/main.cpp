#include "advent.h"
#include "point.h"
#include "portal.h"

bool pointInBounds (int x, int y, int mapWidth, int mapHeight) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) { 
        return false;
    }
    return true;
}

void addPointAtXY (int x, int y, point lastPoint, char *map, int mapWidth, point_list *nextPoints, portal_list *portals) {
    char nextChar = map[y * mapWidth + x];
    point nextPoint = {};
    if (nextChar == '.') {
        nextPoint.x = x;
        nextPoint.y = y;
        nextPoint.steps = lastPoint.steps + 1;
        nextPoint.level = lastPoint.level;
        listPush(nextPoints, nextPoint);
    }
    else if (nextChar >= 'A' && nextChar <= 'Z') {
        bool portalFirstPoint;
        int steppedOnPortalIndex = listIndexByPos(portals, lastPoint, &portalFirstPoint);
        ASSERT(steppedOnPortalIndex != -1);
        portal steppedOnPortal = portals->values[steppedOnPortalIndex];
        bool outer = false;
        if (portalFirstPoint && steppedOnPortal.pos0IsOuter) {
            outer = true;
        }
        else if (!portalFirstPoint && !steppedOnPortal.pos0IsOuter) {
            outer = true;
        }

        if (outer) {
            if (lastPoint.level != 0) {
                if (portalFirstPoint) {
                    nextPoint = steppedOnPortal.pos1;
                }
                else {
                    nextPoint = steppedOnPortal.pos0;
                }
                nextPoint.level = lastPoint.level - 1;
            }
        }
        else {
            if (portalFirstPoint) {
                nextPoint = steppedOnPortal.pos1;
            }
            else {
                nextPoint = steppedOnPortal.pos0;
            }
            nextPoint.level = lastPoint.level + 1;
        }
        // arbitrary 50 level cap on depth
        if (nextPoint.x != 0 && nextPoint.y != 0 && nextPoint.level < 50) {
            nextPoint.steps = lastPoint.steps + 1;
            listPush(nextPoints, nextPoint);
        }
    }
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 50 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    int mapWidth = 129;
    int mapHeight = 125;
    char *map = (char *)allocateSize(&memory, mapWidth * mapHeight);
    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            map[i * mapWidth + j] = ' ';
        }
    }

    const int stringLength = 200;
    char *line = (char *)allocateSize(&memory, stringLength);
    int row = 0;
    while (fgets(line, stringLength, stdin)) {
        int col = 0;
        while (line[col] != 0 && line[col] != '\n') {
            map[row * mapWidth + col] = line[col];
            ++col;
        }
        ++row;
    }

    portal_list portals = portalListInit(&memory, 50);

    // scan for portals horizontally
    for (int i = 0; i < mapHeight; ++i) {
        char firstLetter = ' ';
        for (int j = 0; j < mapWidth; ++j) {
            char currentLetter = map[i * mapWidth + j];
            if (currentLetter >= 'A' && currentLetter <= 'Z') {
                if (firstLetter != ' ') {
                    point portalPos = {};
                    portalPos.y = i;
                    bool outer = false;
                    if (pointInBounds(j - 2, i, mapWidth, mapHeight) &&
                        map[i * mapWidth + j - 2] == '.') 
                    {
                        portalPos.x = j - 2;
                        if (j - 2 == -1 || j - 2 == mapWidth - 3) {
                            outer = true;
                        }
                        else {
                            outer = false;
                        }
                    }
                    else if (pointInBounds(j + 1, i, mapWidth, mapHeight) &&
                             map[i * mapWidth + j + 1] == '.') 
                    {
                        portalPos.x = j + 1;
                        if (j + 1 == 2 || j + 1 == mapWidth) {
                            outer = true;
                        }
                        else {
                            outer = false;
                        }
                    }
                    else {
                        ASSERT(0);
                    }

                    char name[3] = {};
                    name[0] = firstLetter;
                    name[1] = currentLetter;
                    name[2] = 0;

                    int portalIndex = listIndexByName(&portals, name);
                    if (portalIndex == -1) {
                        portal newPortal = {};
                        newPortal.name[0] = name[0];
                        newPortal.name[1] = name[1];
                        newPortal.name[2] = name[2];
                        newPortal.pos0 = portalPos;
                        newPortal.pos0IsOuter = outer;
                        listPush(&portals, newPortal);
                    }
                    else {
                        portal *existingPortal = &portals.values[portalIndex];
                        existingPortal->pos1 = portalPos;
                    }
                }
                else {
                    firstLetter = currentLetter;
                }
            }
            else {
                firstLetter = ' ';
            }
        }
    }

    // scan for portals vertically
    for (int j = 0; j < mapWidth; ++j) {
        char firstLetter = ' ';
        for (int i = 0; i < mapHeight; ++i) {
            char currentLetter = map[i * mapWidth + j];
            if (currentLetter >= 'A' && currentLetter <= 'Z') {
                if (firstLetter != ' ') {
                    point portalPos = {};
                    portalPos.x = j;
                    bool outer = false;
                    if (pointInBounds(j, (i - 2), mapWidth, mapHeight) &&
                        map[(i - 2) * mapWidth + j] == '.') 
                    {
                        portalPos.y = i - 2;
                        if (i - 2 == -1 || i - 2 == mapHeight - 3) {
                            outer = true;
                        }
                        else {
                            outer = false;
                        }
                    }
                    else if (pointInBounds(j, (i + 1), mapWidth, mapHeight) &&
                             map[(i + 1) * mapWidth + j] == '.') 
                    {
                        portalPos.y = i + 1;
                        if (i + 1 == 2 || i + 1 == mapHeight) {
                            outer = true;
                        }
                        else {
                            outer = false;
                        }
                    }
                    else {
                        ASSERT(0);
                    }

                    char name[3] = {};
                    name[0] = firstLetter;
                    name[1] = currentLetter;
                    name[2] = 0;

                    int portalIndex = listIndexByName(&portals, name);
                    if (portalIndex == -1) {
                        portal newPortal = {};
                        newPortal.name[0] = name[0];
                        newPortal.name[1] = name[1];
                        newPortal.name[2] = name[2];
                        newPortal.pos0 = portalPos;
                        newPortal.pos0IsOuter = outer;
                        listPush(&portals, newPortal);
                    }
                    else {
                        portal *existingPortal = &portals.values[portalIndex];
                        existingPortal->pos1 = portalPos;
                    }
                }
                else {
                    firstLetter = currentLetter;
                }
            }
            else {
                firstLetter = ' ';
            }
        }
    }

    portal startPortal = portals.values[listIndexByName(&portals, "AA")];
    point startPos = startPortal.pos0;
    startPos.level = 0;
    portal endPortal = portals.values[listIndexByName(&portals, "ZZ")];
    point endPos = endPortal.pos0;
    endPos.level = 0;

    point_list nextPoints = pointListInit(&memory, 1000000);
    point_list visitedPoints = pointListInit(&memory, 1000000);

    listPush(&nextPoints, startPos);

    int currentPointIndex = 0;
    while (nextPoints.numValues - currentPointIndex > 0) {
        point currentPoint = nextPoints.values[currentPointIndex];
        ++currentPointIndex;
        if (currentPoint.x == endPos.x && currentPoint.y == endPos.y && currentPoint.level == 0) {
            endPos.steps = currentPoint.steps;
            break;
        }
        if (listIndexOf(&visitedPoints, currentPoint) == -1) {
            listPush(&visitedPoints, currentPoint);

            addPointAtXY(currentPoint.x - 1, currentPoint.y, currentPoint, map, mapWidth, &nextPoints, &portals);
            addPointAtXY(currentPoint.x + 1, currentPoint.y, currentPoint, map, mapWidth, &nextPoints, &portals);
            addPointAtXY(currentPoint.x, currentPoint.y - 1, currentPoint, map, mapWidth, &nextPoints, &portals);
            addPointAtXY(currentPoint.x, currentPoint.y + 1, currentPoint, map, mapWidth, &nextPoints, &portals);
        }
    }

    printf("%d\n", endPos.steps);

    // init windows console 
    //win_console winConsole;
    //float targetFrameRate = 1000.0f / 60.0f;
    //initWinConsole(&winConsole, targetFrameRate);
    //setWinConsoleOutputActive(&winConsole);

    //int windowWidth = 100;
    //int windowHeight = 100;
    //char *window = (char *)allocateSize(&memory, windowWidth * windowHeight);
    //int requiredWindowBufferSize = initWinConsoleBuffer(&winConsole, windowWidth, windowHeight);
    //void *windowBuffer = allocateSize(&memory, requiredWindowBufferSize);
    //setWindowBufferMemory(&winConsole, windowBuffer);

    //startWinConsoleFrame(&winConsole);

    //writeWinConsoleCharsToWholeScreen(&winConsole, window);
        //getWinConsoleInput(&winConsole, &input);

        //char text[30];
        //_itoa_s(xPos, text, 30, 10);
        //winConsoleWriteText(&winConsole, text, 30, 1, 1);
        //renderWinConsole(&winConsole);
        //waitForEndOfWinConsoleFrame(&winConsole);

    // coords found by printing alarge area of the beam and exploring interactively: 921, 745

    // setWinConsoleStandardOutputActive(&winConsole);
    // releaseWinConsoleHandles(&winConsole);

    return 0;
}
