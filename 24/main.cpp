#include "advent.h"
#include "win_console.cpp"

#define GRID_DIM 5

char getCharAtRowCol (int row, int col, char *grid) {
    if (row < 0 || row >= GRID_DIM) {
        return '.';
    }
    if (col < 0 || col >= GRID_DIM) {
        return '.';
    }

    return grid[row * GRID_DIM + col];
}

char *getGridAtLevel (int level, char **grids) {
    return grids[level + 150];
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int stringLength = 10;
    char *line = (char *)allocateSize(&memory, stringLength);

    int numGrids = 300;
    char **grids = (char **)allocateSize(&memory, sizeof(char *) * numGrids);
    char **tempGrids = (char **)allocateSize(&memory, sizeof(char *) * numGrids);
    for (int i = 0; i < numGrids; ++i) {
        char **gridPointer = &grids[i];
        *gridPointer = (char *)allocateSize(&memory, GRID_DIM * GRID_DIM);
        char **tempGridPointer = &tempGrids[i];
        *tempGridPointer = (char *)allocateSize(&memory, GRID_DIM * GRID_DIM);

        char *grid = *gridPointer;
        for (int row = 0; row < GRID_DIM; ++row) {
            for (int col = 0; col < GRID_DIM; ++col) {
                grid[row * GRID_DIM + col] = '.';
            }
        }

        char *tempGrid = *tempGridPointer;
        for (int row = 0; row < GRID_DIM; ++row) {
            for (int col = 0; col < GRID_DIM; ++col) {
                tempGrid[row * GRID_DIM + col] = '.';
            }
        }
    }

    char *startGrid = getGridAtLevel(0, grids);
    int inputRow = 0;
    while (fgets(line, stringLength, stdin)) {
        for (int col = 0; col < GRID_DIM; ++col) {
            startGrid[inputRow * GRID_DIM + col] = line[col];
        }
        ++inputRow;
    }

    //win_console winConsole;
    //float targetFrameRate = 1000.0f / 3.0f;
    //initWinConsole(&winConsole, targetFrameRate);
    //setWinConsoleOutputActive(&winConsole);

    //int windowWidth = 40;
    //int windowHeight = 20;
    //char *window = (char *)allocateSize(&memory, windowWidth * windowHeight);
    //int requiredWindowBufferSize = initWinConsoleBuffer(&winConsole, windowWidth, windowHeight);
    //void *windowBuffer = allocateSize(&memory, requiredWindowBufferSize);
    //setWindowBufferMemory(&winConsole, windowBuffer);

    //for (int i = 0; i < windowHeight; ++i) {
    //    for (int j = 0; j < windowWidth; ++j) {
    //        window[i * windowWidth + j] = ' ';
    //    }
    //}

    // found by inspecting output
    char *repeatedPattern = ".##.###.###.##..######.#.";

    int power = 1;
    int rating = 0;
    for (int i = 0; i < GRID_DIM * GRID_DIM; ++i) {
        if (repeatedPattern[i] == '#') {
            rating += power;
        }
        power *= 2;
    }
    printf("%d\n", rating);

    int step = 0;
    while (step < 200) {
        //startWinConsoleFrame(&winConsole);

        // apply rules
        for (int gridIndex = 1; gridIndex < numGrids - 1; ++gridIndex) {
            int level = gridIndex - 150;

            if (level == -1) {
                int t = 0;
            }
            char *grid = getGridAtLevel(level, grids);
            char *tempGrid = getGridAtLevel(level, tempGrids);
            for (int i = 0; i < GRID_DIM; ++i) {
                for (int j = 0; j < GRID_DIM; ++j) {
                    if (i == 2 && j == 2) {
                        continue;
                    }
                    int numAdjacentBugs = 0;
                    if(i == 0) {
                        char *outerGrid = getGridAtLevel(level - 1, grids);
                        char upperTile = getCharAtRowCol(1, 2, outerGrid);
                        if (upperTile == '#') {
                            ++numAdjacentBugs;
                        }
                    }
                    else if (i == 3 && j == 2) {
                        char *innerGrid = getGridAtLevel(level + 1, grids);
                        for (int innerCol = 0; innerCol < GRID_DIM; ++ innerCol) {
                            char upperTile = getCharAtRowCol(4, innerCol, innerGrid);
                            if (upperTile == '#') {
                                ++numAdjacentBugs;
                            }
                        }
                    }
                    else {
                        char upperTile = getCharAtRowCol(i-1, j, grid);
                        if (upperTile == '#') {
                            ++numAdjacentBugs;
                        }
                    }

                    if(i == 4) {
                        char *outerGrid = getGridAtLevel(level - 1, grids);
                        char lowerTile = getCharAtRowCol(3, 2, outerGrid);
                        if (lowerTile == '#') {
                            ++numAdjacentBugs;
                        }
                    }
                    else if (i == 1 && j == 2) {
                        char *innerGrid = getGridAtLevel(level + 1, grids);
                        for (int innerCol = 0; innerCol < GRID_DIM; ++ innerCol) {
                            char lowerTile = getCharAtRowCol(0, innerCol, innerGrid);
                            if (lowerTile == '#') {
                                ++numAdjacentBugs;
                            }
                        }
                    }
                    else {
                        char lowerTile = getCharAtRowCol(i+1, j, grid);
                        if (lowerTile == '#') {
                            ++numAdjacentBugs;
                        }
                    }

                    if(j == 0) {
                        char *outerGrid = getGridAtLevel(level - 1, grids);
                        char leftTile = getCharAtRowCol(2, 1, outerGrid);
                        if (leftTile == '#') {
                            ++numAdjacentBugs;
                        }
                    }
                    else if (i == 2 && j == 3) {
                        char *innerGrid = getGridAtLevel(level + 1, grids);
                        for (int innerRow = 0; innerRow < GRID_DIM; ++ innerRow) {
                            char leftTile = getCharAtRowCol(innerRow, 4, innerGrid);
                            if (leftTile == '#') {
                                ++numAdjacentBugs;
                            }
                        }
                    }
                    else {
                        char leftTile = getCharAtRowCol(i, j-1, grid);
                        if (leftTile == '#') {
                            ++numAdjacentBugs;
                        }
                    }

                    if(j == 4) {
                        char *outerGrid = getGridAtLevel(level - 1, grids);
                        char rightTile = getCharAtRowCol(2, 3, outerGrid);
                        if (rightTile == '#') {
                            ++numAdjacentBugs;
                        }
                    }
                    else if (i == 2 && j == 1) {
                        char *innerGrid = getGridAtLevel(level + 1, grids);
                        for (int innerRow = 0; innerRow < GRID_DIM; ++ innerRow) {
                            char rightTile = getCharAtRowCol(innerRow, 0, innerGrid);
                            if (rightTile == '#') {
                                ++numAdjacentBugs;
                            }
                        }
                    }
                    else {
                        char rightTile = getCharAtRowCol(i, j+1, grid);
                        if (rightTile == '#') {
                            ++numAdjacentBugs;
                        }
                    }

                    char currentTile = getCharAtRowCol(i, j, grid);
                    if (currentTile == '#') {
                        if (numAdjacentBugs == 1) {
                            tempGrid[i * GRID_DIM + j] = '#';
                        }
                        else {
                            tempGrid[i * GRID_DIM + j] = '.';
                        }
                    }
                    else {
                        if (numAdjacentBugs == 1 || numAdjacentBugs == 2) {
                            tempGrid[i * GRID_DIM + j] = '#';
                        }
                        else {
                            tempGrid[i * GRID_DIM + j] = '.';
                        }
                    }
                }
            }

            //for (int i = 0; i < GRID_DIM; ++i) {
            //    for (int j = 0; j < GRID_DIM; ++j) {
            //        printf("%c", grid[(i * GRID_DIM) + j]);
            //    }
            //    printf("\n");
            //}
            //printf("\n");

            //for (int i = 0; i < GRID_DIM; ++i) {
            //    for (int j = 0; j < GRID_DIM; ++j) {
            //        window[(i + 7) * windowWidth + (j+18)] = startGrid[(i * GRID_DIM) + j];
            //    }
            //}

            //writeWinConsoleCharsToWholeScreen(&winConsole, window);
            //renderWinConsole(&winConsole);
            //waitForEndOfWinConsoleFrame(&winConsole);
        }

        for (int gridIndex = 0; gridIndex < numGrids; ++gridIndex) {
            char *temp = grids[gridIndex];
            grids[gridIndex] = tempGrids[gridIndex];
            tempGrids[gridIndex] = temp;
        }
            
        ++step;
    }

    int numBugs = 0;
    for (int gridIndex = 0; gridIndex < numGrids; ++gridIndex) {
        char *grid = grids[gridIndex];

        for (int row = 0; row < GRID_DIM; ++row) {
            for (int col = 0; col < GRID_DIM; ++col) {
                if (grid[row * GRID_DIM + col] == '#') {
                    ++numBugs;
                }
            }
        }
    }
    printf("%d\n", numBugs);

    //setWinConsoleStandardOutputActive(&winConsole);
    //releaseWinConsoleHandles(&winConsole);

    return 0;
}
