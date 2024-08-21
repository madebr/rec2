#include "win32.h"

#include "win32_dinput.h"
#include "win32_ssdx.h"

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

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gDefault_spec_index, 0x00662200, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gDefault_data_index, 0x00662204, 1);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tGraf_spec, gGraf_specs, 2, 0x00662208); // FIXME: implement
C2_HOOK_VARIABLE_IMPLEMENT(int, gGraf_spec_index, 0x00762324);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNbPixelBits, 0x0074ca60);
C2_HOOK_VARIABLE_IMPLEMENT(int, gUnknown_int_0074ca94, 0x0074ca94);
C2_HOOK_VARIABLE_IMPLEMENT(int, gUnknown_int_0074cf48, 0x0074cf48);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gFatalErrorMessage, 512, 0x006acc88);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFatalErrorMessageValid, 0x006ad498);
C2_HOOK_VARIABLE_IMPLEMENT(int, gWin32_fatal_error_exit_code, 0x006ad494);

C2_HOOK_VARIABLE_IMPLEMENT(HWND, gHWnd, 0x006ad4c8);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gWindowActiveState, 0x006621e0, 2); // FIXME: enum: 0, 1 or 2

C2_HOOK_VARIABLE_IMPLEMENT(int, gWindowMovingResizing, 0x006ad4d0);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTINPUTDEVICEA, gDirectInputDevice, 0x006acea0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(char*, gRenderer, 0x006621dc, "3DFX_WIN");
C2_HOOK_VARIABLE_IMPLEMENT(int, gMouseLButtonDown, 0x006ad488);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMouseRButtonDown, 0x006ad48c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMouseCaptured, 0x006ad490);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(POINT, gCursorPos_LastClick, 0x006621d4, {-1,-1});
C2_HOOK_VARIABLE_IMPLEMENT(POINT, gPD_mouse_position, 0x006ad458);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCursorPos_LastClick_Valid, 0x006ad454);
C2_HOOK_VARIABLE_IMPLEMENT(int, gScaleMouse, 0x006ad468);
C2_HOOK_VARIABLE_IMPLEMENT(int, gScreenWidth, 0x006ad460);
C2_HOOK_VARIABLE_IMPLEMENT(int, gScreenHeight, 0x006ad464);

C2_HOOK_VARIABLE_IMPLEMENT(int, gKeyboardBufferLength, 0x006ad4b0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gKeyboardBuffer, 20, 0x006ace88);

C2_HOOK_VARIABLE_IMPLEMENT(int, gPerformanceCounterInitialized, 0x006acc70);
C2_HOOK_VARIABLE_IMPLEMENT(LARGE_INTEGER, gPerformanceCounterStart, 0x006acc80);
C2_HOOK_VARIABLE_IMPLEMENT(LARGE_INTEGER, gPerformanceCounterFrequency_s, 0x006acc68);
C2_HOOK_VARIABLE_IMPLEMENT(LARGE_INTEGER, gPerformanceCounterFrequency_ms, 0x006ad1f0);
C2_HOOK_VARIABLE_IMPLEMENT(LARGE_INTEGER, gPerformanceCounterFrequency_us, 0x006ac858);

C2_HOOK_VARIABLE_IMPLEMENT(int, gTimeLastKeyboardInput, 0x006ad49c);

C2_HOOK_VARIABLE_IMPLEMENT(br_diaghandler, gPD_error_handler, 0x006aceb0);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(char, gExtendedAsciiToNormalAscii, 128, 0x00662150, {
    ' ', ' ',  ' ',  ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '\'', '\'', ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '~',  'c',  '$', '*',  'Y', '|', ' ', '"', 'c', 'a', '<', '-', '-', 'R', '-',
    'o', ' ',  '2',  '3', '\'', 'u', ' ', '.', ',', '1', 'o', '>', ' ', ' ', ' ', '{',
    'A', 'A',  'A',  'A', 'A',  'A', 'A', 'C', 'E', 'E', 'E', 'E', 'I', 'I', 'I', 'I',
    'D', '}',  'O',  'O', 'O',  'O', 'O', 'x', '0', 'U', 'U', 'U', 'U', 'Y', 'b', 'B',
    'a', 'a',  'a',  'a', 'a',  'a', 'a', 'C', 'e', 'e', 'e', 'e', 'i', 'i', 'i', 'i',
    'd', '}',  'o',  'o', 'o',  'o', 'o', '/', '0', 'u', 'u', 'u', 'u', 'y', 'b', 'y',
});

C2_HOOK_VARIABLE_IMPLEMENT(int, gWin32ActionReplayBufferAllocated, 0x006ad4d4);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPDActionReplayBuffer, 0x006ad470);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDActionReplayBufferSize, 0x006ad474);

void C2_HOOK_FASTCALL Win32AllocateActionReplayBuffer(void) {
    MEMORYSTATUS memory_status;
    unsigned int bufferSize;
    void *buffer;

    if (C2V(gWin32ActionReplayBufferAllocated)) {
        return;
    }
    C2V(gWin32ActionReplayBufferAllocated) = 1;
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
    C2V(gPDActionReplayBuffer) = buffer;
    if (buffer == NULL) {
        C2V(gPDActionReplayBufferSize) = 0;
    } else {
        C2V(gPDActionReplayBufferSize) = bufferSize;
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
C2_HOOK_FUNCTION(0x0051b810, Win32AllocateActionReplayBuffer)

void C2_HOOK_FASTCALL PDInitialiseSystem(void) {
    C2V(gBack_screen) = NULL;
    C2V(gScreen) = NULL;
    ShowCursor(FALSE);
    KeyBegin();
    Win32InitInputDevice();
}
C2_HOOK_FUNCTION(0x0051b9f0, PDInitialiseSystem)

void C2_HOOK_FASTCALL PDSetFileVariables(void) {
    C2V(gDir_separator) = "\\";
}
C2_HOOK_FUNCTION(0x0051c6f0, PDSetFileVariables)

void C2_HOOK_FASTCALL PDBuildAppPath(char* pThe_path) {
    GetCurrentDirectoryA(253, pThe_path);
    GetShortPathNameA(pThe_path, pThe_path, 253);
    strcat(pThe_path, "\\");
    dr_dprintf("Application path '%s'", pThe_path);
}
C2_HOOK_FUNCTION(0x0051c700, PDBuildAppPath)

void (C2_HOOK_FASTCALL * PDFatalError_original)(const char* pThe_str);
void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str) {
#if 0 //defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    PDFatalError_original(pThe_str);
#else
    dr_dprintf("FATAL ERROR: %s", pThe_str);
    if (pThe_str == NULL) {
        pThe_str = "NULL str1";
    }
    C2V(gFatalErrorMessageValid) = 1;
    sprintf(C2V(gFatalErrorMessage), "%s\n%s", pThe_str, "");

    C2V(gWin32_fatal_error_exit_code) = 700;
    if (C2V(gBack_screen) != NULL) {
        if (C2V(gBack_screen)->pixels != NULL) {
            C2V(gWin32_fatal_error_exit_code) = 700;
            PDUnlockRealBackScreen();
        }
    }
    if (C2V(gBr_initialized)) {
        RemoveAllBrenderDevices();
    }
    PDShutdownSystem();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL
(0x0051af20, PDFatalError, PDFatalError_original)

void C2_HOOK_FASTCALL PDNonFatalError(char* pThe_str) {
    dr_dprintf("*** ERROR...");
    dr_dprintf(pThe_str);
}
C2_HOOK_FUNCTION(0x0051afb0, PDNonFatalError)

int C2_HOOK_FASTCALL PDIsWindowInactive(void) {
    return C2V(gWindowActiveState) == 1;
}
C2_HOOK_FUNCTION(0x0051afd0, PDIsWindowInactive)

char C2_HOOK_FASTCALL PDConvertToASCIILessThan128(char pChar) {
    if ((unsigned char)pChar > 0x7f) {
        char c = C2V(gExtendedAsciiToNormalAscii)[(unsigned char)pChar - 128];
        dr_dprintf("PDConvertToASCIILessThan128() Returning %d", c);
        return c;
    }
    dr_dprintf("PDConvertToASCIILessThan128() Returning %d", pChar);
    return pChar;
}
C2_HOOK_FUNCTION(0x0051afe0, PDConvertToASCIILessThan128)

int (C2_HOOK_FASTCALL * PDGetKeyboardCharacter_original)(void);
int C2_HOOK_FASTCALL PDGetKeyboardCharacter(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return PDGetKeyboardCharacter_original();
#else
    int key;
    Win32ServiceMessages();
    if (C2V(gKeyboardBufferLength) == 0) {
        return 0;
    }
    key = C2V(gKeyboardBuffer)[0];
    if (C2V(gKeyboardBufferLength) > 1) {
        c2_memmove(&C2V(gKeyboardBuffer)[1], &C2V(gKeyboardBuffer)[0], C2V(gKeyboardBufferLength) - 1);
    }
    C2V(gKeyboardBufferLength) -= 1;
    dr_dprintf("KEY RETURNED %d", key);
    return key;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051b040, PDGetKeyboardCharacter, PDGetKeyboardCharacter_original)

void (C2_HOOK_FASTCALL * PDClearKeyboardBuffer_original)(void);
void C2_HOOK_FASTCALL PDClearKeyboardBuffer(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    PDClearKeyboardBuffer_original();
#else

    dr_dprintf("KEYBOARD BUFFER CLEARED");
    C2V(gKeyboardBufferLength) = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051b0a0, PDClearKeyboardBuffer, PDClearKeyboardBuffer_original)

C2_NORETURN_FUNCPTR static void (C2_HOOK_FASTCALL * PDShutdownSystem_original)(void);
C2_NORETURN void C2_HOOK_FASTCALL PDShutdownSystem(void) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    PDShutdownSystem_original();
    C2_HOOK_FINISH();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051c110, PDShutdownSystem, PDShutdownSystem_original)

void C2_HOOK_FASTCALL PDLockRealBackScreen(void) {
}
C2_HOOK_FUNCTION(0x0051c2e0, PDLockRealBackScreen)

void C2_HOOK_FASTCALL PDUnlockRealBackScreen(void) {
}
C2_HOOK_FUNCTION(0x0051c2f0, PDUnlockRealBackScreen)

void DeActivateApp(void) {
    DR_DPRINTF("DeActivateApp() - START");
    if (!C2V(gWindowMovingResizing) && C2V(gWindowActiveState) == 2) {
        DR_DPRINTF("DeActivateApp() - deactivating app");
        C2V(gWindowMovingResizing) = 1;
        if (C2V(gDirectInputDevice) != NULL) {
            IDirectInputDevice_Unacquire(C2V(gDirectInputDevice));
        }
        if (C2V(gHWnd) != NULL) {
            SetWindowPos(C2V(gHWnd), NULL, 0, 0, 0, 0, SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOZORDER);
        }
        C2V(gWindowActiveState) = (strcmp(C2V(gRenderer), "D3D") == 0) ? 0 : 1;
        C2V(gWindowMovingResizing) = 0;
    }
    DR_DPRINTF("DeActivateApp() - END; active state now %d", C2V(gWindowActiveState));
}

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
            DestroyWindow(C2V(gHWnd));
            C2V(gHWnd) = NULL;
            PDShutdownSystem();
            return 0;
        }
        break;
    case WM_LBUTTONDOWN:
        C2V(gMouseLButtonDown) = C2V(gWindowActiveState) == 2 ? 1 : 0;
        if (!C2V(gMouseCaptured) && C2V(gWindowActiveState) == 2) {
            SetCapture(C2V(gHWnd));
            C2V(gMouseCaptured) = 1;
        }
        break;
    case WM_LBUTTONUP:
        C2V(gMouseLButtonDown) = 0;
        if (C2V(gMouseCaptured)) {
            ReleaseCapture();
            C2V(gMouseCaptured) = 0;
        }
        break;
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
        if (C2V(gWindowActiveState) != 2) {
            C2V(gCursorPos_LastClick) = C2V(gPD_mouse_position);
            C2V(gCursorPos_LastClick_Valid) = 1;
            break;
        }
        GetCursorPos(&point);
        if (!C2V(gScaleMouse) || strcmp(C2V(gRenderer), "D3D") == 0) {
            ScreenToClient(C2V(gHWnd), &point);
            C2V(gPD_mouse_position) = C2V(gCursorPos_LastClick) = point;
        } else {
            C2V(gPD_mouse_position).x = C2V(gCursorPos_LastClick).x = (int)((double)point.x / (C2V(gScreenWidth) / 640.));
            C2V(gPD_mouse_position).y = C2V(gCursorPos_LastClick).y = (int)((double)point.y / (C2V(gScreenHeight) / 480.));
        }
        C2V(gCursorPos_LastClick_Valid) = 1;
        break;
    case WM_RBUTTONDOWN:
        C2V(gMouseRButtonDown) = C2V(gWindowActiveState) == 2 ? 1 : 0;
        if (!C2V(gMouseCaptured) && C2V(gWindowActiveState) == 2) {
            SetCapture(C2V(gHWnd));
            C2V(gMouseCaptured) = 1;
        }
        break;
    case WM_RBUTTONUP:
        C2V(gMouseRButtonDown) = 0;
        if (C2V(gMouseCaptured)) {
            ReleaseCapture();
            C2V(gMouseCaptured) = 0;
        }
        break;
    case WM_CHAR:
        if (0x1f < (char)wParam  && (char)wParam != 0x7f) {
            // FIXME: use C2_ARRSIZE
            if (C2V(gKeyboardBufferLength) >= 20) {
                memmove(C2V(gKeyboardBuffer), &C2V(gKeyboardBuffer)[1], 19); // FIXME: use ARRAY_SIZEOF or something similar
                C2V(gKeyboardBufferLength) = 19; // FIXME: use ARRAY_SIZEOF or something similar
            }
            C2V(gKeyboardBuffer)[C2V(gKeyboardBufferLength)] = (char)wParam;
            C2V(gKeyboardBufferLength)++;
            strncpy(buffer, C2V(gKeyboardBuffer), C2V(gKeyboardBufferLength));
            buffer[C2V(gKeyboardBufferLength)] = '\0';
            DR_DPRINTF("KEY PRESSED, BUFFER NOW IS: '%s'", buffer);
        }
        break;
#if !defined(KEEP_ACTIVE_IN_BACKGROUND)
    case WM_ACTIVATEAPP:
        DR_DPRINTF("WM_ACTIVATEAPP: wparam is %d, lparam is %d, fg window is %p, main win is %p, hWnd is %p, isiconic is %d",
                   wParam, lParam, GetForegroundWindow(), C2V(gHWnd), hWnd, IsIconic(C2V(gHWnd)));
        if (C2V(gHWnd) != NULL) {
            if (GetForegroundWindow() == C2V(gHWnd) && !IsIconic(C2V(gHWnd))) {
                DR_DPRINTF("Activating app");
                if (C2V(gDirectInputDevice) != NULL) {
                    IDirectInputDevice_Acquire(C2V(gDirectInputDevice));
                }
                if (C2V(gHWnd) != NULL && C2V(gScreenWidth) != 0) {
                    SetWindowPos(C2V(gHWnd), NULL, 0, 0, C2V(gScreenWidth), C2V(gScreenHeight), SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOZORDER);
                }
                C2V(gWindowActiveState) = 2;
                if (C2V(gPerformanceCounterInitialized)) {
                    LARGE_INTEGER li;
                    QueryPerformanceCounter(&li);
                    C2V(gTimeLastKeyboardInput) = (int)((li.QuadPart - C2V(gPerformanceCounterStart).QuadPart) / C2V(gPerformanceCounterFrequency_ms).QuadPart);
                }
            } else {
                if (C2V(gBack_screen) != NULL && C2V(gBack_screen)->pixels != NULL) {
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
C2_HOOK_FUNCTION(0x0051b0c0, Carma2MainWndProc)

int C2_HOOK_FASTCALL PDGetASCIIFromKey(int pKey) {

    if (PDKeyDown(0)) {
        return C2V(gASCII_shift_table)[pKey];
    } else {
        return C2V(gASCII_table)[pKey];
    }
}
C2_HOOK_FUNCTION(0x0051cab0, PDGetASCIIFromKey)

void C2_HOOK_CDECL Win32ServiceMessages(void) {
    MSG msg;
    DR_DPRINTF("Win32ServiceMessages() - START");
    while (1) {
        if (C2V(gWindowActiveState) == 1) {
            SetForegroundWindow(C2V(gHWnd));
        }

        if (C2V(gWindowActiveState) == 0) {
            if (GetMessageA(&msg, NULL, 0, 0) == -1) {
                DR_DPRINTF("Win32ServiceMessages() - breaking cos GetMessage() returned -1");
                break;
            }
        } else {
            if (PeekMessageA(&msg, NULL, 0, 0, 1) == 0) {
                DR_DPRINTF("Win32ServiceMessages() - breaking cos PeekMessage() returned 0");
                break;
            }
            if (C2V(gWindowActiveState) == 0) {
                if (GetMessageA(&msg, NULL, 0, 0) == -1) {
                    DR_DPRINTF("Win32ServiceMessages() - breaking cos GetMessage() returned -1");
                    break;
                }
            }
        }
        if (msg.message == WM_QUIT) {
            DR_DPRINTF("WM_QUIT received.");
            if (C2V(gWindowActiveState) == 2) {
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
C2_HOOK_FUNCTION(0x0051cad0, Win32ServiceMessages)

int (C2_HOOK_FASTCALL * PDCheckDriveExists2_original)(const char* pThe_path, const char* pFile_name, tU32 pMin_size);
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
        strcat(the_path, C2V(gDir_separator));
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
C2_HOOK_FUNCTION_ORIGINAL(0x0051d500, PDCheckDriveExists2, PDCheckDriveExists2_original)

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
C2_HOOK_FUNCTION(0x004910d0, PDReadSourceLocation)


int C2_HOOK_FASTCALL PDDoWeLeadAnAustereExistance(void) {
    return 0;
}
C2_HOOK_FUNCTION(0x0051d600, PDDoWeLeadAnAustereExistance)

void (C2_HOOK_FASTCALL * PDSetPaletteEntries_original)(br_pixelmap* pPalette, int pFirst_colour, int pCount);
void C2_HOOK_FASTCALL PDSetPaletteEntries(br_pixelmap* pPalette, int pFirst_colour, int pCount) {
#if defined(C2_HOOKS_ENABLED)
    PDSetPaletteEntries_original(pPalette, pFirst_colour, pCount);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051c850, PDSetPaletteEntries, PDSetPaletteEntries_original)

void C2_HOOK_FASTCALL PDSetPalette(br_pixelmap *pixelmap) {
    BrPixelmapPaletteSet(C2V(gScreen), pixelmap);
}
C2_HOOK_FUNCTION(0x0051c840, PDSetPalette)

int C2_HOOK_FASTCALL PDServiceSystem(tU32 pTime_since_last_call) {

    Win32ServiceMessages();
    return 0;
}
C2_HOOK_FUNCTION(0x0051cbe0, PDServiceSystem)

void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only) {
    BrPixelmapDoubleBuffer(C2V(gScreen), C2V(gBack_screen));
}
C2_HOOK_FUNCTION(0x0051c520, PDScreenBufferSwap)

void C2_HOOK_FASTCALL PDInitTimer(void) {
    LARGE_INTEGER freq;
    LARGE_INTEGER current_count;


    C2V(gPerformanceCounterInitialized) = 0;
    if (QueryPerformanceFrequency(&freq) != 0) {
        C2V(gPerformanceCounterInitialized) = 1;
        C2V(gPerformanceCounterFrequency_s) = freq;
        QueryPerformanceCounter(&current_count);
        C2V(gPerformanceCounterStart) = current_count;
        C2V(gPerformanceCounterFrequency_ms).QuadPart = C2V(gPerformanceCounterFrequency_s).QuadPart / 1000;
        C2V(gPerformanceCounterFrequency_us).QuadPart = C2V(gPerformanceCounterFrequency_s).QuadPart / 1000000;
    }
}
C2_HOOK_FUNCTION(0x0051d7c0, PDInitTimer)

int (C2_HOOK_FASTCALL * PDGetTotalTime_original)(void);
int C2_HOOK_FASTCALL PDGetTotalTime(void) {
#if 0 //defined(C2_HOOKS_ENABLED)
    return PDGetTotalTime_original();
#else
    if (C2V(gPerformanceCounterInitialized)) {
        LARGE_INTEGER perfCountValue;
        QueryPerformanceCounter(&perfCountValue);
        // Is it okay to convert unsigned to int here?
        return (int)((perfCountValue.QuadPart - C2V(gPerformanceCounterStart).QuadPart) / C2V(gPerformanceCounterFrequency_ms).QuadPart);
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051d410, PDGetTotalTime, PDGetTotalTime_original)

int C2_HOOK_FASTCALL PDGetMicroseconds(void) {
#if 0 //defined(C2_HOOKS_ENABLED)
    return PDGetTotalTime_original();
#else
    if (C2V(gPerformanceCounterInitialized)) {
        LARGE_INTEGER perfCountValue;
        QueryPerformanceCounter(&perfCountValue);
        // Is it okay to convert unsigned to int here?
        return (int)((perfCountValue.QuadPart - C2V(gPerformanceCounterStart).QuadPart) / C2V(gPerformanceCounterFrequency_us).QuadPart);
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION(0x0051d990, PDGetMicroseconds)

void C2_HOOK_FASTCALL PDEnterDebugger(const char* pStr) {

    dr_dprintf("PDEnterDebugger(): %s", pStr);
    ShowCursor(1);
    __debugbreak();
    ShowCursor(0);
}

int C2_HOOK_FASTCALL PDFileUnlock(char* pThe_path) {
    DWORD dwAttributes;

    dwAttributes = GetFileAttributesA(pThe_path);
    if (dwAttributes != INVALID_FILE_ATTRIBUTES) {
        return SetFileAttributesA(pThe_path, dwAttributes & ~FILE_ATTRIBUTE_READONLY);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0051d4b0, PDFileUnlock)

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
C2_HOOK_FUNCTION(0x00486c30, PDEnumPath)

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
C2_HOOK_FUNCTION(0x00578380, IsNetworkShare)

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
C2_HOOK_FUNCTION(0x00585ee0, IsValidDriveIndex)

int C2_HOOK_FASTCALL PDmkdir(const char* path) {

    return c2_mkdir(path);
}
C2_HOOK_FUNCTION(0x00486c20, PDmkdir)

void C2_HOOK_FASTCALL MAMSUnlock(void** pPtr) {

    void* mem = *pPtr;
    if (mem == NULL) {
        FatalError(kFatalError_OOM_S, "NULL POINTER BEING FREED");
    }
    HeapFree(GetProcessHeap(), 0, mem);
    *pPtr = NULL;
}
C2_HOOK_FUNCTION(0x0044c9f0, MAMSUnlock)

void C2_HOOK_FASTCALL MAMSLock(void** pPtr) {
}
C2_HOOK_FUNCTION(0x0044ca30, MAMSLock)

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
C2_HOOK_FUNCTION(0x0051d640, PDForEveryFileRecurse)

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
C2_HOOK_FUNCTION(0x0051c760, PDForEveryFile)

void C2_HOOK_FASTCALL PDGetFormattedDate(char* pTimeStr) {
    SYSTEMTIME time;
    char buffer[256];

    GetLocalTime(&time);
    GetDateFormatA(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &time, NULL, buffer, sizeof(buffer));
    strcpy(pTimeStr, buffer);
}
C2_HOOK_FUNCTION(0x0051da20, PDGetFormattedDate)

void C2_HOOK_FASTCALL PDGetFormattedTime(char* pTimeStr) {
    SYSTEMTIME time;
    char buffer[256];

    GetLocalTime(&time);
    GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &time, NULL, buffer, sizeof(buffer));
    strcpy(pTimeStr, buffer);
}
C2_HOOK_FUNCTION(0x0051da90, PDGetFormattedTime)

int C2_HOOK_FASTCALL PDGetMouseClickPosition(int* pX_coord, int* pY_coord) {
    if (!C2V(gCursorPos_LastClick_Valid)) {
        C2V(gCursorPos_LastClick_Valid) = 0;
        return 0;
    }
    *pX_coord = C2V(gCursorPos_LastClick).x;
    *pY_coord = C2V(gCursorPos_LastClick).y;
    C2V(gCursorPos_LastClick_Valid) = 0;
    return 1;
}
C2_HOOK_FUNCTION(0x0051db00, PDGetMouseClickPosition)

static void C2_HOOK_CDECL OnWarnCallback(char* text) {

    dr_dprintf("*******************************************************************************");
    dr_dprintf("BRender WARNING: '%s'", text);
    dr_dprintf("*******************************************************************************");
}
C2_HOOK_FUNCTION(0x0051c5f0, OnWarnCallback)

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
    C2V(gFatalErrorMessageValid) = 1;
    sprintf(C2V(gFatalErrorMessage), "%s\n%s", msg, text);
    C2V(gWin32_fatal_error_exit_code) = 700;
    if (C2V(gBack_screen) != NULL && C2V(gBack_screen)->pixels != NULL) {
        PossibleUnlock(1);
    }
    if (C2V(gBr_initialized)) {
        RemoveAllBrenderDevices();
    }
    PDShutdownSystem();
}
C2_HOOK_FUNCTION(0x0051c620, OnErrorCallback)

void C2_HOOK_FASTCALL PDInstallErrorHandlers(void) {

    C2V(gPD_error_handler).identifier = "LlantisilioBlahBlahBlahOgOgOch";
    C2V(gPD_error_handler).warning = OnWarnCallback;
    C2V(gPD_error_handler).failure = OnErrorCallback;
    BrDiagHandlerSet(&C2V(gPD_error_handler));
}
C2_HOOK_FUNCTION(0x0051c6c0, PDInstallErrorHandlers)

int (C2_HOOK_FASTCALL* PDInitScreenVars_original)(int pArgc, const char** pArgv);
int C2_HOOK_FASTCALL PDInitScreenVars(int pArgc, const char** pArgv) {
#if 0//defined(C2_HOOKS_ENABLED)
    return PDInitScreenVars_original(pArgc, pArgv);
#else
    C2V(gGraf_spec_index) = C2V(gDefault_spec_index);
    C2V(gGraf_data_index) = C2V(gDefault_data_index);
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051c290, PDInitScreenVars, PDInitScreenVars_original)

void C2_HOOK_FASTCALL PDSaveOriginalPalette(void) {

}
C2_HOOK_FUNCTION(0x0051c270, PDSaveOriginalPalette)

void C2_HOOK_FASTCALL PDInitScreen(void) {

}
C2_HOOK_FUNCTION(0x0051c2b0, PDInitScreen)

void (C2_HOOK_FASTCALL * PDAllocateScreenAndBack_original)(void);
void C2_HOOK_FASTCALL PDAllocateScreenAndBack(void) {

    dr_dprintf("PDAllocateScreenAndBack() - START...");

    dr_dprintf("Setting up DirectSound stuff...");

    SSDXStart(C2V(gHWnd), 0, 1, SSDX_InitDirectSound);

    C2V(gNbPixelBits) = 16;
    BrDevBeginVar(&C2V(gScreen), C2V(gRenderer),
        BRT_WINDOW_FULLSCREEN_B, 1,
        BRT_WINDOW_HANDLE_H, C2V(gHWnd),
        BRT_WIDTH_I32, 640,
        BRT_HEIGHT_I32, 480,
        BRT_PIXEL_BITS_I32, 16,
        BRT_PIXEL_TYPE_U8, BR_PMT_RGB_565,
        0);
    if (C2V(gScreen) == NULL) {
        BrFailure("Unable to allocate Main Front Screen");
    }

    if (c2_strcmp(C2V(gScreen)->identifier, "Voodoo Graphics") == 0) {
        dr_dprintf("%s: lock seldom", C2V(gScreen)->identifier);
    } else {
        dr_dprintf("%s: lock often", C2V(gScreen)->identifier);
        C2V(gScreen_lock_often) = 1;
    }
    C2V(gShadow_workaround) = 1; /* Not 100% sure this is shadow related */
    C2V(gEnable_texture_interpolation) = 1;
    C2V(gEnable_texture_antialiasing) = 1;
    C2V(gTexture_power_of_2) = 1;
    C2V(gUnknown_int_0074ca94) = 8;
    C2V(gUnknown_int_0074cf48) = 256;
    C2V(gAdapt_sky_model_for_cockpit) = 1;
    C2V(gNo_fog) = 1; /* Enables lut for horizon material */
    C2V(gRenderer_fixup_basename) = "VOODOO";
    C2V(gRenderer_fixup_extension) = ".TXT";
    C2V(gNo_render_indent) = 1;
    C2V(gTranslucent_smoke) = 1; /* maybe? */
    C2V(gEnable_perspective_maps) = 1;
    C2V(gNoTransients) = 1;
    C2V(gUse_actor_dimming) = 1;

    C2V(gScreen)->origin_x = 0;
    C2V(gScreen)->origin_y = 0;
    C2V(gBack_screen) = BrPixelmapMatch(C2V(gScreen), BR_PMMATCH_OFFSCREEN);
    if (C2V(gBack_screen) == NULL) {
        BrFailure("Unable to allocate Back Screen");
    }

    C2V(gPixelFlags) = kPixelFlags_unknown_0x4 | (C2V(gNbPixelBits) == 16 ? kPixelFlags_16bbp : 0);

    C2V(gDouble_back_screen) = BrPixelmapMatch(C2V(gBack_screen), BR_PMMATCH_OFFSCREEN);
    C2V(gHas_double_back_screen) = 1;

    c2_memset(C2V(gBack_screen)->pixels, 0, C2V(gBack_screen)->height * C2V(gBack_screen)->row_bytes);

    BrPixelmapDoubleBuffer(C2V(gScreen), C2V(gBack_screen));

    dr_dprintf("PDAllocateScreenAndBack() - END.");
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051c300, PDAllocateScreenAndBack, PDAllocateScreenAndBack_original)

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

void C2_HOOK_FASTCALL PDGetMousePosition(int *pX, int *pY) {
    if (C2V(gWindowActiveState) != 2) {
        *pX = C2V(gPD_mouse_position).x;
        *pY = C2V(gPD_mouse_position).y;
        return;
    }
    POINT pnt;
    GetCursorPos(&pnt);
    ScreenToClient(C2V(gHWnd), &pnt);
    *pX = pnt.x;
    *pY = pnt.y;
    C2V(gPD_mouse_position).x = *pX;
    C2V(gPD_mouse_position).y = *pY;
}
C2_HOOK_FUNCTION(0x0051c900, PDGetMousePosition)

void C2_HOOK_FASTCALL PDRevertPalette(void) {
}
C2_HOOK_FUNCTION(0x0051c280, PDRevertPalette)

void C2_HOOK_FASTCALL PDMouseButtons(int* pLeftButtonDown, int* pRightButtonDown) {

    *pLeftButtonDown = C2V(gMouseLButtonDown);
    if (pRightButtonDown != NULL) {
        *pRightButtonDown = C2V(gMouseRButtonDown);
    }
}
C2_HOOK_FUNCTION(0x0051c8e0, PDMouseButtons)

void C2_HOOK_FASTCALL PDExtractFilename(char* pDest, const char* pPath) {
    char dirname [256];
    char drive[4];
    char filename[256];
    char extension[256];

    _splitpath(pPath, drive, dirname, filename, extension);
    c2_sprintf(pDest, "%s%s", filename, extension);
}
C2_HOOK_FUNCTION(0x0056a3a9, PDExtractFilename)

void C2_HOOK_FASTCALL PDExtractDirectory(char* pDest, const char* pPath) {
    char dirname [256];
    char drive[4];
    char filename[256];
    char extension[256];

    _splitpath(pPath, drive, dirname, filename, extension);
    c2_sprintf(pDest, "%s%s", drive, dirname);
}
C2_HOOK_FUNCTION(0x0056a349, PDExtractDirectory)
