#include <windows.h>
#include <fileapi.h>

struct win_console {
    HANDLE inputHandle;
    HANDLE outputHandle;
    HANDLE standardOutputHandle;

    SMALL_RECT windowDimensions;
    COORD windowBufferSize;
    COORD windowWriteCoord;
    CHAR_INFO *windowBuffer;

    long long perfCountFrequency;
    LARGE_INTEGER lastCounter;
    float targetMSPerFrame;
};

struct win_console_key {
    bool down;
    bool justPressed;
};

struct win_console_input {
    win_console_key upKey;
    win_console_key downKey;
    win_console_key leftKey;
    win_console_key rightKey;
    win_console_key enterKey;
    win_console_key backspaceKey;
    char keyEntered;
};

void initWinConsole (win_console *winConsole, float targetMSPerFrame) {
    winConsole->inputHandle = 
        CreateFileA("CONIN$",
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ,
                    0,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    0);
    winConsole->standardOutputHandle = 
        CreateFileA("CONOUT$",
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_WRITE,
                    0,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    0);
    winConsole->outputHandle = 
        CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  0,
                                  CONSOLE_TEXTMODE_BUFFER,
                                  0);
    CONSOLE_CURSOR_INFO cursorInfo = {};
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(winConsole->outputHandle, &cursorInfo);

    LARGE_INTEGER perfCountFrequencyResult;
    QueryPerformanceFrequency(&perfCountFrequencyResult);
    winConsole->perfCountFrequency = perfCountFrequencyResult.QuadPart;
    winConsole->targetMSPerFrame = targetMSPerFrame;
}

void setWinConsoleOutputActive (win_console *winConsole) {
    SetConsoleActiveScreenBuffer(winConsole->outputHandle);
}

void setWinConsoleStandardOutputActive (win_console *winConsole) {
    SetConsoleActiveScreenBuffer(winConsole->standardOutputHandle);
}

int initWinConsoleBuffer (win_console *winConsole, int width, int height) {
    winConsole->windowDimensions = {};
    winConsole->windowDimensions.Left = 0;
    winConsole->windowDimensions.Top = 0;
    winConsole->windowDimensions.Bottom = (SHORT)height - 1;
    winConsole->windowDimensions.Right = (SHORT)width - 1;

    winConsole->windowBufferSize = {};
    winConsole->windowBufferSize.X = (SHORT)width;
    winConsole->windowBufferSize.Y = (SHORT)height;

    winConsole->windowWriteCoord = {};
    winConsole->windowWriteCoord.X = 0;
    winConsole->windowWriteCoord.Y = 0;
    
    //SetConsoleScreenBufferSize(winConsole->outputHandle, winConsole->windowBufferSize);
    CONSOLE_SCREEN_BUFFER_INFOEX screenBufferInfo;

    ZeroMemory(&screenBufferInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));
    screenBufferInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    
    GetConsoleScreenBufferInfoEx(winConsole->outputHandle, &screenBufferInfo);
    
    screenBufferInfo.dwCursorPosition = winConsole->windowWriteCoord;
    screenBufferInfo.dwSize = winConsole->windowBufferSize;
    screenBufferInfo.srWindow.Left = 0;
    screenBufferInfo.srWindow.Top = 0;
    screenBufferInfo.srWindow.Right = (SHORT)width;
    screenBufferInfo.srWindow.Bottom = (SHORT)height;

    SetConsoleScreenBufferInfoEx(winConsole->outputHandle, &screenBufferInfo);

    return sizeof(CHAR_INFO) * width * height;
} 

void setWindowBufferMemory (win_console *winConsole, void *memory) {
    winConsole->windowBuffer = (CHAR_INFO *)memory;

    // blank out the buffer to start
    for (int i = 0; i < winConsole->windowBufferSize.Y; ++i) {
        for (int j = 0; j < winConsole->windowBufferSize.X; ++j) {
            CHAR_INFO charInfo = {};
            charInfo.Char.AsciiChar = ' ';
            charInfo.Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
            winConsole->windowBuffer[i * winConsole->windowBufferSize.X + j] = charInfo;
        }
    }
}

void startWinConsoleFrame (win_console *winConsole) {
    QueryPerformanceCounter(&winConsole->lastCounter);
}

void waitForEndOfWinConsoleFrame (win_console *winConsole) {
    LARGE_INTEGER workCounter;
    QueryPerformanceCounter(&workCounter);
    long long counterElapsed = workCounter.QuadPart - winConsole->lastCounter.QuadPart;
    float elapsedMS = (1000.0f * (float)counterElapsed) / (float)winConsole->perfCountFrequency;

    if (elapsedMS < winConsole->targetMSPerFrame) {
        DWORD sleepMS = (DWORD)(winConsole->targetMSPerFrame - elapsedMS);
        if (sleepMS > 0) {
            Sleep(sleepMS);
        }
    }
}

void renderWinConsole (win_console *winConsole) {
    WriteConsoleOutput(winConsole->outputHandle,
                       winConsole->windowBuffer,       
                       winConsole->windowBufferSize,   
                       winConsole->windowWriteCoord,   
                       &winConsole->windowDimensions); 
}

void winConsoleWriteText (win_console *winConsole, char *text, int length, int x, int y) {
    for (int i = 0; i < length; ++i) {
        char letter = text[i];
        if (letter == 0) {
            break;
        }
        CHAR_INFO *charInfo = &winConsole->windowBuffer[y * winConsole->windowBufferSize.X + x + i];
        charInfo->Char.AsciiChar = letter;
    }
}

// assumes the characters buffer is the right size
void writeWinConsoleCharsToRect (win_console *winConsole, char *characters, 
                       int x, int y, int width, int height) 
{
    // TODO(ebuchholz)
}

void writeWinConsoleCharAtXY (win_console *winConsole, char character, int x, int y) {
    CHAR_INFO *charInfo = &winConsole->windowBuffer[y * winConsole->windowBufferSize.X + x];
    charInfo->Char.AsciiChar = character;
}

void writeWinConsoleCharsToWholeScreen (win_console *winConsole, char *characters) {
    for (int i = 0; i < winConsole->windowBufferSize.Y; ++i) {
        for (int j = 0; j < winConsole->windowBufferSize.X; ++j) {
            CHAR_INFO *charInfo = &winConsole->windowBuffer[i * winConsole->windowBufferSize.X + j];
            charInfo->Char.AsciiChar = characters[i * winConsole->windowBufferSize.X + j];
        }
    }
}

void releaseWinConsoleHandles (win_console *winConsole) {
    CloseHandle(winConsole->outputHandle);
    CloseHandle(winConsole->standardOutputHandle);
    CloseHandle(winConsole->inputHandle);
}

void getWinConsoleInput (win_console *winConsole, win_console_input *input) {
    INPUT_RECORD inputs[100];
    DWORD numInputsRead;
    PeekConsoleInput(winConsole->inputHandle, inputs, 100, &numInputsRead);

    for (unsigned int i = 0; i < numInputsRead; ++i) {
        INPUT_RECORD inputRecord = inputs[i];
        if (inputRecord.EventType == KEY_EVENT) {
            KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;
            switch (keyEvent.wVirtualKeyCode) {
                case VK_UP: {
                    if (keyEvent.bKeyDown) {
                        if (!input->upKey.down) {
                            input->upKey.justPressed = true;
                        }
                        input->upKey.down = true;
                    }
                    else { 
                        input->upKey.down = false;
                    }
                } break;
                case VK_DOWN: {
                    if (keyEvent.bKeyDown) {
                        if (!input->downKey.down) {
                            input->downKey.justPressed = true;
                        }
                        input->downKey.down = true;
                    }
                    else { 
                        input->downKey.down = false;
                    }
                } break;
                case VK_LEFT: {
                    if (keyEvent.bKeyDown) {
                        if (!input->leftKey.down) {
                            input->leftKey.justPressed = true;
                        }
                        input->leftKey.down = true;
                    }
                    else { 
                        input->leftKey.down = false;
                    }
                } break;
                case VK_RIGHT: {
                    if (keyEvent.bKeyDown) {
                        if (!input->rightKey.down) {
                            input->rightKey.justPressed = true;
                        }
                        input->rightKey.down = true;
                    }
                    else { 
                        input->rightKey.down = false;
                    }
                } break;
                case VK_RETURN: {
                    if (keyEvent.bKeyDown) {
                        if (!input->enterKey.down) {
                            input->enterKey.justPressed = true;
                        }
                        input->enterKey.down = true;
                    }
                    else { 
                        input->enterKey.down = false;
                    }
                } break;
                case VK_BACK: {
                    if (keyEvent.bKeyDown) {
                        if (!input->backspaceKey.down) {
                            input->backspaceKey.justPressed = true;
                        }
                        input->backspaceKey.down = true;
                    }
                    else { 
                        input->backspaceKey.down = false;
                    }
                } break;
                default: {
                    if (keyEvent.bKeyDown) {
                        char key = keyEvent.uChar.AsciiChar;
                        if (key >= 32 && key <= 127) {
                            input->keyEntered = key;
                        }
                    }
                } break;
            }
        }
    }

    FlushConsoleInputBuffer(winConsole->inputHandle);
}

void winConsoleResetInput (win_console_input *input) {
    input->upKey.justPressed = false;
    input->downKey.justPressed = false;
    input->leftKey.justPressed = false;
    input->rightKey.justPressed = false;
    input->enterKey.justPressed = false;
    input->backspaceKey.justPressed = false;
    input->keyEntered = 0;
}


