#include "win32.h"

#include "win32_dinput.h"
#include "win32_ssdx.h"

#include "drmem.h"
#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "main.h"
#include "platform.h"
#include "utility.h"

#include "rec2_macros.h"

#include "c2_direct.h"
#include "c2_stdio.h"
#include "c2_string.h"

#include "brender/brender.h"
#include "rec2_types.h"

#include <direct.h>
#include <windows.h>
#include <dinput.h>

#if defined(C2_WIN32_DEBUG)
#define DR_DPRINTF(...) dr_dprintf(__VA_ARGS__)
#else
#define DR_DPRINTF(...)
#endif


// GLOBAL: CARMA2_HW 0x00662200
int gDefault_spec_index = 1;

// GLOBAL: CARMA2_HW 0x00662204
int gDefault_data_index = 1;

// GLOBAL: CARMA2_HW 0x00662208
tGraf_spec gGraf_specs[2] = {
    { 8, 1, 0, 320, 200, 0, 0, "32X20X8", "320x200 init string", 320, 320, 200, 0 },
    { 8, 1, 0, 640, 480, 0, 0, "64X48X8", "640x480 init string", 640, 640, 480, 0 },
};

// GLOBAL: CARMA2_HW 0x00762324
int gGraf_spec_index;

// GLOBAL: CARMA2_HW 0x0074ca60
int gNbPixelBits;

// GLOBAL: CARMA2_HW 0x0074ca94
int gUnknown_int_0074ca94;

// GLOBAL: CARMA2_HW 0x0074cf48
int gUnknown_int_0074cf48;


// GLOBAL: CARMA2_HW 0x006acc88
char gFatalErrorMessage[512];

// GLOBAL: CARMA2_HW 0x006ad498
int gIsFatalError;

// GLOBAL: CARMA2_HW 0x006ad494
int gExit_code;


// GLOBAL: CARMA2_HW 0x006ad4c8
void* gHWnd;

// GLOBAL: CARMA2_HW 0x006621e0
int gWindowActiveState = 2; // FIXME: enum: 0, 1 or 2


// GLOBAL: CARMA2_HW 0x006ad4d0
int gWindowMovingResizing;

// GLOBAL: CARMA2_HW 0x006acea0
LPDIRECTINPUTDEVICEA gDirectInputDevice;

// GLOBAL: CARMA2_HW 0x006621dc
char* gRenderer = "3DFX_WIN";

// GLOBAL: CARMA2_HW 0x006ad488
int gMouseLButtonDown;

// GLOBAL: CARMA2_HW 0x006ad48c
int gMouseRButtonDown;

// GLOBAL: CARMA2_HW 0x006ad490
int gMouseCaptured;

// GLOBAL: CARMA2_HW 0x006621d4
POINT gCursorPos_LastClick = {-1,-1};

// GLOBAL: CARMA2_HW 0x006ad458
POINT gPD_mouse_position;

// GLOBAL: CARMA2_HW 0x006ad454
int gCursorPos_LastClick_Valid;

// GLOBAL: CARMA2_HW 0x006ad468
int gScaleMouse;

// GLOBAL: CARMA2_HW 0x006ad460
int gScreenWidth;

// GLOBAL: CARMA2_HW 0x006ad464
int gScreenHeight;


// GLOBAL: CARMA2_HW 0x006ad4b0
int gKeyboardBufferLength;

// GLOBAL: CARMA2_HW 0x006ace88
char gKeyboardBuffer[20];


// GLOBAL: CARMA2_HW 0x006acc70
int gPerformanceCounterInitialized;

// GLOBAL: CARMA2_HW 0x006acc80
LARGE_INTEGER gPerformanceCounterStart;

// GLOBAL: CARMA2_HW 0x006acc68
LARGE_INTEGER gPerformanceCounterFrequency_s;

// GLOBAL: CARMA2_HW 0x006ad1f0
LARGE_INTEGER gPerformanceCounterFrequency_ms;

// GLOBAL: CARMA2_HW 0x006ac858
LARGE_INTEGER gPerformanceCounterFrequency_us;


// GLOBAL: CARMA2_HW 0x006ad49c
tU32 gTime_app_activated;


// GLOBAL: CARMA2_HW 0x006aceb0
br_diaghandler gPD_error_handler;

// GLOBAL: CARMA2_HW 0x00662150
char gExtendedAsciiToNormalAscii[128] = {
    ' ', ' ',  ' ',  ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '\'', '\'', ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '~',  'c',  '$', '*',  'Y', '|', ' ', '"', 'c', 'a', '<', '-', '-', 'R', '-',
    'o', ' ',  '2',  '3', '\'', 'u', ' ', '.', ',', '1', 'o', '>', ' ', ' ', ' ', '{',
    'A', 'A',  'A',  'A', 'A',  'A', 'A', 'C', 'E', 'E', 'E', 'E', 'I', 'I', 'I', 'I',
    'D', '}',  'O',  'O', 'O',  'O', 'O', 'x', '0', 'U', 'U', 'U', 'U', 'Y', 'b', 'B',
    'a', 'a',  'a',  'a', 'a',  'a', 'a', 'C', 'e', 'e', 'e', 'e', 'i', 'i', 'i', 'i',
    'd', '}',  'o',  'o', 'o',  'o', 'o', '/', '0', 'u', 'u', 'u', 'u', 'y', 'b', 'y',
};


// GLOBAL: CARMA2_HW 0x006ad4d4
int gWin32ActionReplayBufferAllocated;

// GLOBAL: CARMA2_HW 0x006ad470
void* gPDActionReplayBuffer;

// GLOBAL: CARMA2_HW 0x006ad474
int gPDActionReplayBufferSize;

// GLOBAL: CARMA2_HW 0x006ad46c
HPALETTE gPDPalette;


// GLOBAL: CARMA2_HW 0x0079ea84
SYSTEM_INFO gSystem_info;

// GLOBAL: CARMA2_HW 0x0068c830
DWORD gPage_size;

#define Win32AllocateActionReplayBuffer PDReallyAllocateActionReplayBuffer

// FUNCTION: CARMA2_HW 0x0051b810
void C2_HOOK_FASTCALL Win32AllocateActionReplayBuffer(void) {
    MEMORYSTATUS memory_status;
    unsigned int bufferSize;
    void *buffer;

    if (gWin32ActionReplayBufferAllocated) {
        return;
    }
    gWin32ActionReplayBufferAllocated = 1;
    memory_status.dwLength = sizeof(memory_status);
    GlobalMemoryStatus(&memory_status);
    dr_dprintf("Win32AllocateActionReplayBuffer(): Memory Status BEFORE Action Replay Allocation:\n"
               "             dwLength        %u\n"
               "             dwMemoryLoad    %u\n"
               "             dwTotalPhys     %u\n"
               "             dwAvailPhys     %u\n"
               "             dwTotalPageFile %u\n"
               "             dwAvailPageFile %u\n"
               "             dwTotalVirtual  %u\n"
               "             dwAvailVirtual  %u",
               memory_status.dwLength,
               memory_status.dwMemoryLoad,
               memory_status.dwTotalPhys,
               memory_status.dwAvailPhys,
               memory_status.dwTotalPageFile,
               memory_status.dwAvailPageFile,
               memory_status.dwTotalVirtual,
               memory_status.dwAvailVirtual);
    bufferSize = 8000000;
    if (memory_status.dwTotalPhys < 24000000) {
        bufferSize = 500000;
    } else if (memory_status.dwTotalPhys < 32000000) {
        bufferSize = 2000000;
    } else if (memory_status.dwTotalPhys < 48000000) {
        bufferSize = 4000000;
    }
    dr_dprintf("Win32AllocateActionReplayBuffer(): We want %d bytes...", bufferSize);
    if (memory_status.dwAvailPageFile + memory_status.dwAvailPhys < bufferSize) {
        bufferSize = memory_status.dwAvailPageFile + memory_status.dwAvailPhys - 0x100000;
        dr_dprintf("Win32AllocateActionReplayBuffer(): ...but there's only %d bytes available...", bufferSize);
    }
    if (bufferSize < 0x10000) {
        bufferSize = 0x10000;
        dr_dprintf("Win32AllocateActionReplayBuffer(): ...but we have to have a minimum size of %d bytes...", bufferSize);
    }
    while (1) {
        buffer = BrMemAllocate(bufferSize, kMem_action_replay_buffer);
        if (buffer != NULL) {
            break;
        }
        bufferSize -= 0x10000;
    }
    gPDActionReplayBuffer = buffer;
    if (buffer == NULL) {
        gPDActionReplayBufferSize = 0;
    } else {
        gPDActionReplayBufferSize = bufferSize;
        Sleep(1000);
    }
    dr_dprintf("Win32AllocateActionReplayBuffer(): Actually allocated %d bytes.", bufferSize);
    GlobalMemoryStatus(&memory_status);
    dr_dprintf("Win32AllocateActionReplayBuffer(): Memory Status AFTER Action Replay Allocation:\n"
               "             dwLength        %u\n"
               "             dwMemoryLoad    %u\n"
               "             dwTotalPhys     %u\n"
               "             dwAvailPhys     %u\n"
               "             dwTotalPageFile %u\n"
               "             dwAvailPageFile %u\n"
               "             dwTotalVirtual  %u\n"
               "             dwAvailVirtual  %u",
               memory_status.dwLength,
               memory_status.dwMemoryLoad,
               memory_status.dwTotalPhys,
               memory_status.dwAvailPhys,
               memory_status.dwTotalPageFile,
               memory_status.dwAvailPageFile,
               memory_status.dwTotalVirtual,
               memory_status.dwAvailVirtual);
}

// FUNCTION: CARMA2_HW 0x0051b9f0
void C2_HOOK_FASTCALL PDInitialiseSystem(void) {
    gBack_screen = NULL;
    gScreen = NULL;
    ShowCursor(FALSE);
    KeyBegin();
    Win32InitInputDevice();
}

// FUNCTION: CARMA2_HW 0x0051c6f0
void C2_HOOK_FASTCALL PDSetFileVariables(void) {
    gDir_separator = "\\";
}

// FUNCTION: CARMA2_HW 0x0051c700
void C2_HOOK_FASTCALL PDBuildAppPath(char* pThe_path) {
    GetCurrentDirectoryA(253, pThe_path);
    GetShortPathNameA(pThe_path, pThe_path, 253);
    strcat(pThe_path, "\\");
    dr_dprintf("Application path '%s'", pThe_path);
}

// FUNCTION: CARMA2_HW 0x0051af20
void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str) {
#if 0 //defined(C2_HOOKS_ENABLED)
    PDFatalError_original(pThe_str);
#else
    dr_dprintf("FATAL ERROR: %s", pThe_str);
    if (pThe_str == NULL) {
        pThe_str = "NULL str1";
    }
    gIsFatalError = 1;
    sprintf(gFatalErrorMessage, "%s\n%s", pThe_str, "");

    gExit_code = 700;
    if (gBack_screen != NULL) {
        if (gBack_screen->pixels != NULL) {
            gExit_code = 700;
            PDUnlockRealBackScreen();
        }
    }
    if (gBr_initialized) {
        DRBrEnd();
    }
    PDShutdownSystem();
#endif
}

// FUNCTION: CARMA2_HW 0x0051afb0
void C2_HOOK_FASTCALL PDNonFatalError(const char* pThe_str) {
    dr_dprintf("*** ERROR...");
    dr_dprintf(pThe_str);
}

// FUNCTION: CARMA2_HW 0x0051afd0
int C2_HOOK_FASTCALL PDIsWindowInactive(void) {
    return gWindowActiveState == 1;
}

// FUNCTION: CARMA2_HW 0x0051afe0
char C2_HOOK_FASTCALL PDConvertToASCIILessThan128(char pChar) {
    if ((unsigned char)pChar > 0x7f) {
        char c = gExtendedAsciiToNormalAscii[(unsigned char)pChar - 128];
        dr_dprintf("PDConvertToASCIILessThan128() Returning %d", c);
        return c;
    }
    dr_dprintf("PDConvertToASCIILessThan128() Returning %d", pChar);
    return pChar;
}

// FUNCTION: CARMA2_HW 0x0051b040
int C2_HOOK_FASTCALL PDGetKeyboardCharacter(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return PDGetKeyboardCharacter_original();
#else
    int key;
    Win32ServiceMessages();
    if (gKeyboardBufferLength == 0) {
        return 0;
    }
    key = gKeyboardBuffer[0];
    if (gKeyboardBufferLength > 1) {
        c2_memmove(&gKeyboardBuffer[1], &gKeyboardBuffer[0], gKeyboardBufferLength - 1);
    }
    gKeyboardBufferLength -= 1;
    dr_dprintf("KEY RETURNED %d", key);
    return key;
#endif
}

// FUNCTION: CARMA2_HW 0x0051b0a0
void C2_HOOK_FASTCALL PDClearKeyboardBuffer(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    PDClearKeyboardBuffer_original();
#else

    dr_dprintf("KEYBOARD BUFFER CLEARED");
    gKeyboardBufferLength = 0;
#endif
}

// FUNCTION: CARMA2_HW 0x0051c110
C2_NORETURN void C2_HOOK_FASTCALL PDShutdownSystem(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    PDShutdownSystem_original();
#else
    // GLOBAL: CARMA2_HW 0x006ad4d8
    static int been_here;

    gBack_screen = NULL;
    if (been_here) {
        CloseGlobalPackedFile();
        ExitProcess(702);
    } else {
        been_here = 1;
        dr_dprintf("PDShutdownSystem()");
        SSDXStop();
        SSDXRelease();
        CloseDirectInput();
        ShowCursor(TRUE);
        if (gHWnd != NULL) {
            dr_dprintf("Resizing main window...");
            SetWindowPos(gHWnd, NULL, -100, -100, 64, 64, SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOZORDER);
        }
        dr_dprintf("Servicing messages...");
        Win32ServiceMessages();
        dr_dprintf("Sending WM_SHOWWINDOW broadcast message...");
        SendMessageA(HWND_BROADCAST, WM_SHOWWINDOW, TRUE, 0);
        if (gIsFatalError) {
            dr_dprintf("Displaying fatal error...");
            MessageBoxA(NULL, gFatalErrorMessage, "Carmageddon Fatal error", MB_ICONERROR);
        }
        if (gHWnd != NULL) {
            dr_dprintf("Destroying window...");
            DestroyWindow(gHWnd);
            gHWnd = NULL;
        }
        dr_dprintf("End of PDShutdownSystem()");
        CloseDiagnostics();
        CloseGlobalPackedFile();
        ExitProcess(gExit_code);
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051c2e0
void C2_HOOK_FASTCALL PDLockRealBackScreen(void) {
}

// FUNCTION: CARMA2_HW 0x0051c2f0
void C2_HOOK_FASTCALL PDUnlockRealBackScreen(void) {
}

void DeActivateApp(void) {
    DR_DPRINTF("DeActivateApp() - START");
    if (!gWindowMovingResizing && gWindowActiveState == 2) {
        DR_DPRINTF("DeActivateApp() - deactivating app");
        gWindowMovingResizing = 1;
        if (gDirectInputDevice != NULL) {
            IDirectInputDevice_Unacquire(gDirectInputDevice);
        }
        if (gHWnd != NULL) {
            SetWindowPos(gHWnd, NULL, 0, 0, 0, 0, SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOZORDER);
        }
        gWindowActiveState = (strcmp(gRenderer, "D3D") == 0) ? 0 : 1;
        gWindowMovingResizing = 0;
    }
    DR_DPRINTF("DeActivateApp() - END; active state now %d", gWindowActiveState);
}

// FUNCTION: CARMA2_HW 0x0051b0c0
LRESULT CALLBACK Carma2MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT paint;
    POINT point;
    char buffer[256];

    switch (uMsg) {
    case WM_PAINT:
        BeginPaint(hWnd, &paint);
        EndPaint(hWnd, &paint);
        return 0;
    case WM_DESTROY:
        DR_DPRINTF("WM_DESTROY received - doing nothing.");
        break;
#if !defined(KEEP_ACTIVE_IN_BACKGROUND)
    case WM_MOVE:
    case WM_SIZE:
        if (IsIconic(hWnd)) {
            DR_DPRINTF("WM_SIZE/WM_MOVE: Window is iconic");
            DeActivateApp();
        }
        break;
#endif
    case WM_SYSCOMMAND:
        if (GET_SC_WPARAM(wParam) == SC_CLOSE) {
            DestroyWindow(gHWnd);
            gHWnd = NULL;
            PDShutdownSystem();
            return 0;
        }
        break;
    case WM_LBUTTONDOWN:
        gMouseLButtonDown = gWindowActiveState == 2 ? 1 : 0;
        if (!gMouseCaptured && gWindowActiveState == 2) {
            SetCapture(gHWnd);
            gMouseCaptured = 1;
        }
        break;
    case WM_LBUTTONUP:
        gMouseLButtonDown = 0;
        if (gMouseCaptured) {
            ReleaseCapture();
            gMouseCaptured = 0;
        }
        break;
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
        if (gWindowActiveState != 2) {
            gCursorPos_LastClick = gPD_mouse_position;
            gCursorPos_LastClick_Valid = 1;
            break;
        }
        GetCursorPos(&point);
        if (!gScaleMouse || strcmp(gRenderer, "D3D") == 0) {
            ScreenToClient(gHWnd, &point);
            gPD_mouse_position = gCursorPos_LastClick = point;
        } else {
            gPD_mouse_position.x = gCursorPos_LastClick.x = (int)((double)point.x / (gScreenWidth / 640.));
            gPD_mouse_position.y = gCursorPos_LastClick.y = (int)((double)point.y / (gScreenHeight / 480.));
        }
        gCursorPos_LastClick_Valid = 1;
        break;
    case WM_RBUTTONDOWN:
        gMouseRButtonDown = gWindowActiveState == 2 ? 1 : 0;
        if (!gMouseCaptured && gWindowActiveState == 2) {
            SetCapture(gHWnd);
            gMouseCaptured = 1;
        }
        break;
    case WM_RBUTTONUP:
        gMouseRButtonDown = 0;
        if (gMouseCaptured) {
            ReleaseCapture();
            gMouseCaptured = 0;
        }
        break;
    case WM_CHAR:
        if (0x1f < (char)wParam  && (char)wParam != 0x7f) {
            if (gKeyboardBufferLength >= REC2_ASIZE(gKeyboardBuffer)) {
                memmove(gKeyboardBuffer, &gKeyboardBuffer[1], REC2_ASIZE(gKeyboardBuffer) - 1);
                gKeyboardBufferLength = REC2_ASIZE(gKeyboardBuffer) - 1;
            }
            gKeyboardBuffer[gKeyboardBufferLength] = (char)wParam;
            gKeyboardBufferLength++;
            strncpy(buffer, gKeyboardBuffer, gKeyboardBufferLength);
            buffer[gKeyboardBufferLength] = '\0';
            DR_DPRINTF("KEY PRESSED, BUFFER NOW IS: '%s'", buffer);
        }
        break;
#if !defined(KEEP_ACTIVE_IN_BACKGROUND)
    case WM_ACTIVATEAPP:
        DR_DPRINTF("WM_ACTIVATEAPP: wparam is %d, lparam is %d, fg window is %p, main win is %p, hWnd is %p, isiconic is %d",
                   wParam, lParam, GetForegroundWindow(), gHWnd, hWnd, IsIconic(gHWnd));
        if (gHWnd != NULL) {
            if (GetForegroundWindow() == gHWnd && !IsIconic(gHWnd)) {
                DR_DPRINTF("Activating app");
                if (gDirectInputDevice != NULL) {
                    IDirectInputDevice_Acquire(gDirectInputDevice);
                }
                if (gHWnd != NULL && gScreenWidth != 0) {
                    SetWindowPos(gHWnd, NULL, 0, 0, gScreenWidth, gScreenHeight, SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOZORDER);
                }
                gWindowActiveState = 2;
                gTime_app_activated = PDGetTotalTime();
            } else {
                if (gBack_screen != NULL && gBack_screen->pixels != NULL) {
                    return 1;
                }
                DeActivateApp();
            }
        }
        break;
#endif
    default:
        break;
    }
    LRESULT res = DefWindowProcA(hWnd, uMsg, wParam, lParam);
    return res;
}

// FUNCTION: CARMA2_HW 0x0051cab0
int C2_HOOK_FASTCALL PDGetASCIIFromKey(int pKey) {

    if (PDKeyDown(0)) {
        return gASCII_shift_table[pKey];
    } else {
        return gASCII_table[pKey];
    }
}

// FUNCTION: CARMA2_HW 0x0051cad0
void C2_HOOK_CDECL Win32ServiceMessages(void) {
    MSG msg;
    DR_DPRINTF("Win32ServiceMessages() - START");
    while (1) {
        if (gWindowActiveState == 1) {
            SetForegroundWindow(gHWnd);
        }

        if (gWindowActiveState == 0) {
            if (GetMessageA(&msg, NULL, 0, 0) == -1) {
                DR_DPRINTF("Win32ServiceMessages() - breaking cos GetMessage() returned -1");
                break;
            }
        } else {
            if (PeekMessageA(&msg, NULL, 0, 0, 1) == 0) {
                DR_DPRINTF("Win32ServiceMessages() - breaking cos PeekMessage() returned 0");
                break;
            }
            if (gWindowActiveState == 0) {
                if (GetMessageA(&msg, NULL, 0, 0) == -1) {
                    DR_DPRINTF("Win32ServiceMessages() - breaking cos GetMessage() returned -1");
                    break;
                }
            }
        }
        if (msg.message == WM_QUIT) {
            DR_DPRINTF("WM_QUIT received.");
            if (gWindowActiveState == 2) {
                DR_DPRINTF("Active, so lock the surface");
                DR_DPRINTF("QuitGame being called...");
                QuitGame();
            }
            PDShutdownSystem();
        }
        TranslateMessage(&msg);
        DR_DPRINTF("Win32ServiceMessages() - dispatching message...");
        DispatchMessageA(&msg);
    }
    DR_DPRINTF("Win32ServiceMessages() - END");
}

// FUNCTION: CARMA2_HW 0x0051d500
int C2_HOOK_FASTCALL PDCheckDriveExists2(const char* pThe_path, const char* pFile_name, tU32 pMin_size) {
#if 0 // defined(C2_HOOKS_ENABLED)
    fprintf(stderr, "PDCheckDriveExists2_original=%p\n", PDCheckDriveExists2_original);
    int res = PDCheckDriveExists2_original(pThe_path, pFile_name, pMin_size);
    return res;
#else
    char the_path[256];
    tU32 file_size;
    HANDLE hFile;

    file_size = 0;
    if (pFile_name != NULL) {
        PathCat(the_path, pThe_path, pFile_name);
    } else {
        strcpy(the_path, pThe_path);
    }
    if (the_path[0] && the_path[1] == ':' && the_path[2] == '\0') {
        strcat(the_path, gDir_separator);
    }
    if (GetFileAttributesA(pThe_path) == INVALID_FILE_ATTRIBUTES) {
        return 0;
    }
    hFile = CreateFileA(the_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        file_size = GetFileSize(hFile, NULL);
        CloseHandle(hFile);
    }
    return file_size >= pMin_size;
#endif
}

// FUNCTION: CARMA2_HW 0x004910d0
int C2_HOOK_FASTCALL PDReadSourceLocation(tPath_name pPath) {
    HKEY hKey;
    LSTATUS status;
    char buffer[256];
    DWORD lenBuffer;

    status = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\SCI\\CARMAGEDDON2", 0, KEY_ALL_ACCESS, &hKey);
    if (status == ERROR_SUCCESS) {
        lenBuffer = sizeof(tPath_name) - 1;  // FIXME: use array_sizeof
        status = RegQueryValueExA(hKey, "SourceLocation", NULL, NULL, (LPBYTE)pPath, &lenBuffer);
        if (status == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return 1;
        }
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, status, 0, buffer, sizeof(buffer) - 1, NULL);
        RegCloseKey(hKey);
    }
    return 0;
}


// FUNCTION: CARMA2_HW 0x0051d600
int C2_HOOK_FASTCALL PDDoWeLeadAnAustereExistance(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051c850
void C2_HOOK_FASTCALL PDSetPaletteEntries(br_pixelmap* pPalette, int pFirst_colour, int pCount) {

#if 0//defined(C2_HOOKS_ENABLED)
    PDSetPaletteEntries_original(pPalette, pFirst_colour, pCount);
#else
    int i;
    PALETTEENTRY colours[256];
    tU32* pixels = pPalette->pixels;

    for (i = 0; i < pCount; i++) {
        tU32 c;

        c = pixels[pFirst_colour + i];
        colours[i].peFlags = 0;
        colours[i].peRed = c >> 16;
        colours[i].peGreen = c >> 8;
        colours[i].peBlue = c >> 0;
    }
    SetPaletteEntries(gPDPalette, pFirst_colour, pCount, colours);
    BrPixelmapDoubleBuffer(gScreen, gBack_screen);
#endif
}

// FUNCTION: CARMA2_HW 0x0051c840
void C2_HOOK_FASTCALL PDSetPalette(br_pixelmap *pixelmap) {
    BrPixelmapPaletteSet(gScreen, pixelmap);
}

// FUNCTION: CARMA2_HW 0x0051cbe0
int C2_HOOK_FASTCALL PDServiceSystem(tU32 pTime_since_last_call) {

    Win32ServiceMessages();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051c520
void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only) {
    BrPixelmapDoubleBuffer(gScreen, gBack_screen);
}

// FUNCTION: CARMA2_HW 0x0051d7c0
void C2_HOOK_FASTCALL PDInitTimer(void) {
    LARGE_INTEGER freq;
    LARGE_INTEGER current_count;


    gPerformanceCounterInitialized = 0;
    if (QueryPerformanceFrequency(&freq) != 0) {
        gPerformanceCounterInitialized = 1;
        gPerformanceCounterFrequency_s = freq;
        QueryPerformanceCounter(&current_count);
        gPerformanceCounterStart = current_count;
        gPerformanceCounterFrequency_ms.QuadPart = gPerformanceCounterFrequency_s.QuadPart / 1000;
        gPerformanceCounterFrequency_us.QuadPart = gPerformanceCounterFrequency_s.QuadPart / 1000000;
    }
}

// FUNCTION: CARMA2_HW 0x0051d410
tU32 C2_HOOK_FASTCALL PDGetTotalTime(void) {
#if 0 //defined(C2_HOOKS_ENABLED)
    return PDGetTotalTime_original();
#else
    if (gPerformanceCounterInitialized) {
        LARGE_INTEGER perfCountValue;
        QueryPerformanceCounter(&perfCountValue);
        return (tU32)((perfCountValue.QuadPart - gPerformanceCounterStart.QuadPart) / gPerformanceCounterFrequency_ms.QuadPart);
    }
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x0051d990
int C2_HOOK_FASTCALL PDGetMicroseconds(void) {
#if 0 //defined(C2_HOOKS_ENABLED)
    return PDGetTotalTime_original();
#else
    if (gPerformanceCounterInitialized) {
        LARGE_INTEGER perfCountValue;
        QueryPerformanceCounter(&perfCountValue);
        // Is it okay to convert unsigned to int here?
        return (int)((perfCountValue.QuadPart - gPerformanceCounterStart.QuadPart) / gPerformanceCounterFrequency_us.QuadPart);
    }
    return 0;
#endif
}

void C2_HOOK_FASTCALL PDEnterDebugger(const char* pStr) {

    dr_dprintf("PDEnterDebugger(): %s", pStr);
    ShowCursor(1);
    __debugbreak();
    ShowCursor(0);
}

// FUNCTION: CARMA2_HW 0x0051d4b0
int C2_HOOK_FASTCALL PDFileUnlock(const char* pThe_path) {
    DWORD dwAttributes;

    dwAttributes = GetFileAttributesA(pThe_path);
    if (dwAttributes != INVALID_FILE_ATTRIBUTES) {
        return SetFileAttributesA(pThe_path, dwAttributes & ~FILE_ATTRIBUTE_READONLY);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00486c30
void C2_HOOK_FASTCALL PDEnumPath(const char* path, tEnumPathCallback pCallback, void* data) {
    char originalCurrentDirectory[MAX_PATH];
    tPath_name filePath;
    BOOL bSucceeded;
    WIN32_FIND_DATAA findFileData;
    HANDLE hFindFile;
    size_t lenFilename;
    size_t lenLnk;
    int callback_res;

    GetCurrentDirectory(REC2_ASIZE(originalCurrentDirectory), originalCurrentDirectory);
    bSucceeded = SetCurrentDirectoryA(path);
    if (!bSucceeded) {
        return;
    }
    hFindFile = FindFirstFileA("*.???", &findFileData);
    if (hFindFile != NULL) {
        while (1) {
            if ((findFileData.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) == 0) {
                lenFilename = c2_strlen(findFileData.cFileName);
                lenLnk = c2_strlen(".lnk");
                if (lenFilename > lenLnk && c2_strcasecmp(&findFileData.cFileName[lenFilename - lenLnk], ".lnk") == 0) {
                    c2_memmove(filePath, findFileData.cFileName, lenFilename - lenLnk);
                    filePath[lenFilename - lenLnk] = '\0';
                } else {
                    c2_memmove(filePath, findFileData.cFileName, lenFilename + 1);
                }
                callback_res = pCallback(filePath, data);
                if (callback_res != 0) {
                    break;
                }
            }
            bSucceeded = FindNextFileA(hFindFile, &findFileData);
            if (!bSucceeded) {
                break;
            }
        }
        FindClose(hFindFile);
    }
    SetCurrentDirectoryA(originalCurrentDirectory);
}

// FUNCTION: CARMA2_HW 0x00578380
int C2_HOOK_CDECL IsNetworkShare(const char* path) {
    size_t lenPath;
    const char* pathPtr;
    const char* pathPtr2;

    lenPath = c2_strlen(path);
    if (lenPath > 4 && (path[0] == '\\' || path[0] == '/') && (path[1] == '\\' || path[1] == '/')) {
        pathPtr = c2_strpbrk(path + 3, "/\\");
        if (pathPtr != NULL && pathPtr[1] != '\0') {
            pathPtr2 = c2_strpbrk(pathPtr + 1, "/\\");
            if (pathPtr2 == NULL || pathPtr2[1] == '\0') {
                return 1;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00585ee0
int C2_HOOK_CDECL IsValidDriveIndex(int driveIndex) {
    char drivePath[4];
    UINT driveType;

    if (driveIndex == 0) {
        return 1;
    }
    drivePath[0] = 'A' + driveIndex - 1;
    drivePath[1] = ':';
    drivePath[2] = '\\';
    drivePath[3] = '\0';
    driveType = GetDriveTypeA(drivePath);
    return driveType != DRIVE_UNKNOWN && driveType != DRIVE_NO_ROOT_DIR;
}

// FUNCTION: CARMA2_HW 0x00486c20
int C2_HOOK_FASTCALL PDmkdir(const char* path) {

    return c2_mkdir(path);
}

// FUNCTION: CARMA2_HW 0x0044c9f0
void C2_HOOK_FASTCALL MAMSUnlock(void** pPtr) {

    void* mem = *pPtr;
    if (mem == NULL) {
        FatalError(kFatalError_OOM_S, "NULL POINTER BEING FREED");
    }
    HeapFree(GetProcessHeap(), 0, mem);
    *pPtr = NULL;
}

// FUNCTION: CARMA2_HW 0x0044ca30
void C2_HOOK_FASTCALL MAMSLock(void** pPtr) {
}

// FUNCTION: CARMA2_HW 0x0051d640
void C2_HOOK_FASTCALL PDForEveryFileRecurse(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine) {
    char found_path[256];
    WIN32_FIND_DATAA find_data;
    HANDLE hFindFile;
    char file_filter[256];
    char current_dir[260];

    GetCurrentDirectoryA(sizeof(current_dir), current_dir);
    if (SetCurrentDirectoryA(pThe_path)) {
        dr_dprintf("*PDForEveryFileRecurse() - NEW DIRECTORY '%s'", pThe_path);
        strcpy(file_filter, "*");
        hFindFile = FindFirstFileA(file_filter, &find_data);
        if (hFindFile != INVALID_HANDLE_VALUE) {
            do {
                if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
                    PathCat(found_path, pThe_path, find_data.cFileName);
                    SetCurrentDirectoryA(current_dir);
                    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                        dr_dprintf("PDForEveryFileRecurse() - Calling action proc with file '%s' in dir '%s'", found_path, current_dir);
                        pAction_routine(found_path);
                    } else {
                        PDForEveryFileRecurse(found_path, pAction_routine);
                    }
                    SetCurrentDirectoryA(pThe_path);
                }
            } while (FindNextFileA(hFindFile, &find_data) != 0);
            FindClose(hFindFile);
        }
        SetCurrentDirectoryA(current_dir);
    }
}

// FUNCTION: CARMA2_HW 0x0051c760
void C2_HOOK_FASTCALL PDForEveryFile(const char* pThe_path, tPDForEveryFile_cbfn pAction_routine) {
    char found_path[256];
    WIN32_FIND_DATAA find_data;
    HANDLE hFindFile;
    char file_filter[6];
    char current_dir[260];

    GetCurrentDirectoryA(sizeof(current_dir), current_dir);
    if (SetCurrentDirectoryA(pThe_path)) {
        strcpy(file_filter, "*.???");
        hFindFile = FindFirstFileA(file_filter, &find_data);
        if (hFindFile != INVALID_HANDLE_VALUE) {
            do {
                PathCat(found_path, pThe_path, find_data.cFileName);
                pAction_routine(found_path);
            } while (FindNextFileA(hFindFile, &find_data) != 0);
            FindClose(hFindFile);
        }
        SetCurrentDirectoryA(current_dir);
    }
}

// FUNCTION: CARMA2_HW 0x0051da20
void C2_HOOK_FASTCALL PDGetCurrentDate(char* pTimeStr) {
    SYSTEMTIME time;
    char buffer[256];

    GetLocalTime(&time);
    GetDateFormatA(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &time, NULL, buffer, sizeof(buffer));
    strcpy(pTimeStr, buffer);
}

// FUNCTION: CARMA2_HW 0x0051da90
void C2_HOOK_FASTCALL PDGetCurrentTime(char* pTimeStr) {
    SYSTEMTIME time;
    char buffer[256];

    C2_HOOK_BUG_ON(LOCALE_SYSTEM_DEFAULT != 0x800);
    C2_HOOK_BUG_ON(LOCALE_NOUSEROVERRIDE != 0x80000000);

    GetLocalTime(&time);
    GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &time, NULL, buffer, sizeof(buffer));
    strcpy(pTimeStr, buffer);
}

// FUNCTION: CARMA2_HW 0x0051db00
int C2_HOOK_FASTCALL PDGetMouseClickPosition(int* pX_coord, int* pY_coord) {
    if (!gCursorPos_LastClick_Valid) {
        gCursorPos_LastClick_Valid = 0;
        return 0;
    }
    *pX_coord = gCursorPos_LastClick.x;
    *pY_coord = gCursorPos_LastClick.y;
    gCursorPos_LastClick_Valid = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0051c5f0
static void C2_HOOK_CDECL OnWarnCallback(char* text) {

    dr_dprintf("*******************************************************************************");
    dr_dprintf("BRender WARNING: '%s'", text);
    dr_dprintf("*******************************************************************************");
}

// FUNCTION: CARMA2_HW 0x0051c620
static void C2_HOOK_CDECL OnErrorCallback(char* text) {
    const char *msg;

    dr_dprintf("*******************************************************************************");
    dr_dprintf("BRender FAILURE: '%s'", text);
    dr_dprintf("*******************************************************************************");
    msg = "BRender error detected:";
    if (msg == NULL) {
        msg = "NULL str1";
    }
    if (text == NULL) {
        msg = "NULL str2";
    }
    gIsFatalError = 1;
    sprintf(gFatalErrorMessage, "%s\n%s", msg, text);
    gExit_code = 700;
    if (gBack_screen != NULL && gBack_screen->pixels != NULL) {
        PossibleUnlock(1);
    }
    if (gBr_initialized) {
        DRBrEnd();
    }
    PDShutdownSystem();
}

// FUNCTION: CARMA2_HW 0x0051c6c0
void C2_HOOK_FASTCALL PDInstallErrorHandlers(void) {

    gPD_error_handler.identifier = "LlantisilioBlahBlahBlahOgOgOch";
    gPD_error_handler.warning = OnWarnCallback;
    gPD_error_handler.failure = OnErrorCallback;
    BrDiagHandlerSet(&gPD_error_handler);
}

// FUNCTION: CARMA2_HW 0x0051c290
int C2_HOOK_FASTCALL PDInitScreenVars(int pArgc, const char** pArgv) {
#if 0//defined(C2_HOOKS_ENABLED)
    return PDInitScreenVars_original(pArgc, pArgv);
#else
    gGraf_spec_index = gDefault_spec_index;
    gGraf_data_index = gDefault_data_index;
    return 1;
#endif
}

// FUNCTION: CARMA2_HW 0x0051c270
void C2_HOOK_FASTCALL PDSaveOriginalPalette(void) {

}

// FUNCTION: CARMA2_HW 0x0051c2b0
void C2_HOOK_FASTCALL PDInitScreen(void) {

}

// FUNCTION: CARMA2_HW 0x0051c300
void C2_HOOK_FASTCALL PDAllocateScreenAndBack(void) {

    dr_dprintf("PDAllocateScreenAndBack() - START...");

    dr_dprintf("Setting up DirectSound stuff...");

    SSDXStart(gHWnd, 0, 1, SSDX_InitDirectSound);

    gNbPixelBits = 16;
    BrDevBeginVar(&gScreen, gRenderer,
        BRT_WINDOW_FULLSCREEN_B, 1,
        BRT_WINDOW_HANDLE_H, gHWnd,
        BRT_WIDTH_I32, 640,
        BRT_HEIGHT_I32, 480,
        BRT_PIXEL_BITS_I32, 16,
        BRT_PIXEL_TYPE_U8, BR_PMT_RGB_565,
        0);
    if (gScreen == NULL) {
        BrFailure("Unable to allocate Main Front Screen");
    }

    if (c2_strcmp(gScreen->identifier, "Voodoo Graphics") == 0) {
        dr_dprintf("%s: lock seldom", gScreen->identifier);
    } else {
        dr_dprintf("%s: lock often", gScreen->identifier);
        gScreen_lock_often = 1;
    }
    gShadow_workaround = 1; /* Not 100% sure this is shadow related */
    gEnable_texture_interpolation = 1;
    gEnable_texture_antialiasing = 1;
    gTexture_power_of_2 = 1;
    gUnknown_int_0074ca94 = 8;
    gUnknown_int_0074cf48 = 256;
    gAdapt_sky_model_for_cockpit = 1;
    gNo_fog = 1; /* Enables lut for horizon material */
    gRenderer_fixup_basename = "VOODOO";
    gRenderer_fixup_extension = ".TXT";
    gNo_render_indent = 1;
    gNo_2d_effects = 1; /* maybe? */
    gEnable_perspective_maps = 1;
    gNoTransients = 1;
    gDevious_2d = 1;

    gScreen->origin_x = 0;
    gScreen->origin_y = 0;
    gBack_screen = BrPixelmapMatch(gScreen, BR_PMMATCH_OFFSCREEN);
    if (gBack_screen == NULL) {
        BrFailure("Unable to allocate Back Screen");
    }

    gPixelFlags = kPixelFlags_unknown_0x4 | (gNbPixelBits == 16 ? kPixelFlags_16bbp : 0);

    gDouble_back_screen = BrPixelmapMatch(gBack_screen, BR_PMMATCH_OFFSCREEN);
    gHas_double_back_screen = 1;

    c2_memset(gBack_screen->pixels, 0, gBack_screen->height * gBack_screen->row_bytes);

    BrPixelmapDoubleBuffer(gScreen, gBack_screen);

    dr_dprintf("PDAllocateScreenAndBack() - END.");
}

int GetRegisterSourceLocation(char* buffer, int* buffer_size) {
    HKEY hKey;
    LONG status;

    status = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\SCI\\CARMAGEDDON2", 0, KEY_ALL_ACCESS, &hKey);
    if (status == ERROR_SUCCESS) {
        status = RegQueryValueExA(hKey, "SourceLocation", NULL, NULL, (LPBYTE)buffer, (LPDWORD)buffer_size);
        if (status == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return 1;
        } else {
            char message[256];
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, status, 0, message, sizeof(message) - 1, NULL);
            RegCloseKey(hKey);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051c900
void C2_HOOK_FASTCALL PDGetMousePosition(int *pX, int *pY) {
    if (gWindowActiveState != 2) {
        *pX = gPD_mouse_position.x;
        *pY = gPD_mouse_position.y;
        return;
    }
    POINT pnt;
    GetCursorPos(&pnt);
    ScreenToClient(gHWnd, &pnt);
    *pX = pnt.x;
    *pY = pnt.y;
    gPD_mouse_position.x = *pX;
    gPD_mouse_position.y = *pY;
}

// FUNCTION: CARMA2_HW 0x0051c280
void C2_HOOK_FASTCALL PDRevertPalette(void) {
}

// FUNCTION: CARMA2_HW 0x0051c8e0
void C2_HOOK_FASTCALL PDMouseButtons(int* pLeftButtonDown, int* pRightButtonDown) {

    *pLeftButtonDown = gMouseLButtonDown;
    if (pRightButtonDown != NULL) {
        *pRightButtonDown = gMouseRButtonDown;
    }
}

// FUNCTION: CARMA2_HW 0x0056a3a9
void C2_HOOK_FASTCALL PDExtractFilename(char* pDest, const char* pPath) {
    char dirname [256];
    char drive[4];
    char filename[256];
    char extension[256];

    _splitpath(pPath, drive, dirname, filename, extension);
    c2_sprintf(pDest, "%s%s", filename, extension);
}

// FUNCTION: CARMA2_HW 0x0056a349
void C2_HOOK_FASTCALL PDExtractDirectory(char* pDest, const char* pPath) {
    char dirname [256];
    char drive[4];
    char filename[256];
    char extension[256];

    _splitpath(pPath, drive, dirname, filename, extension);
    c2_sprintf(pDest, "%s%s", drive, dirname);
}

// FUNCTION: CARMA2_HW 0x004928a0
void C2_HOOK_FASTCALL PDPageInMemory(void* pMemory) {
    tHeap_block_information* heap_block;

    C2_HOOK_BUG_ON(offsetof(tHeap_block_information, next) != 0x48);
    C2_HOOK_BUG_ON(offsetof(tHeap_block_information, field_0x72) != 0x72);
    C2_HOOK_BUG_ON(sizeof(tHeap_block_information) != 0x74);

    heap_block = pMemory;

    if (!IsBadReadPtr(heap_block, sizeof(tHeap_block_information)) && heap_block->field_0x72 == 0x4948) {
        for (; heap_block != NULL; heap_block = heap_block->next) {
            tHeap_memory *block;

            block = (tHeap_memory*)(heap_block + 1);
            for (;;) {
                DWORD size = block->flags & 0x5ffffffc;
                DWORD current;
                if (size == 0) {
                    break;
                }
                if (!(block->flags & 0x1)) {
                    if (gPage_size == 0) {
                        GetSystemInfo(&gSystem_info);
                        gPage_size = gSystem_info.dwPageSize;
                    }
                    for (current = 16 * gPage_size; current < size - 4; current += gPage_size) {
                        block->data[current + 16 - 16 * gPage_size] = block->data[current + 16 - 16 * gPage_size];
                        block->data[current + 16]                        = block->data[current + 16];
                    }
                }
                block = (tHeap_memory*)((BYTE*)block + size);
            }
        }
    }
}

void C2_HOOK_FASTCALL PDPageInProcessMemory(void) {

    PDPageInMemory(GetProcessHeap());
    Sleep(1500);
}

// FUNCTION: CARMA2_HW 0x0051d4d0
void C2_HOOK_FASTCALL PDFileDelete(const char* pPath, int pIgnore_read_only) {

    if (pIgnore_read_only) {
        DWORD flags = GetFileAttributesA(pPath);
        if (flags != INVALID_FILE_ATTRIBUTES) {
            SetFileAttributesA(pPath, flags & ~FILE_ATTRIBUTE_READONLY);
        }
    }
    DeleteFileA(pPath);
}
