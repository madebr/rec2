#include "win32.h"

#include "win32_dinput.h"

#include "errors.h"
#include "globvars.h"
#include "init.h"
#include "main.h"
#include "utility.h"

#include "c2_stdio.h"

#include "brender/brender.h"

#include <windows.h>
#include <dinput.h>

C2_HOOK_VARIABLE_IMPLEMENT(int, gIsFatalError, 0x006ad498);
C2_HOOK_VARIABLE_IMPLEMENT(int, gExitCode, 0x006ad494);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gReal_back_screen, 0x0074d360);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gScreen, 0x0074d3e0);

C2_HOOK_VARIABLE_IMPLEMENT(HWND, gHWnd, 0x006ad4c8);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gWindowActiveState, 0x006621e0, 2); // FIXME: enum: 0, 1 or 2


C2_HOOK_VARIABLE_IMPLEMENT(int, gWindowMovingResizing, 0x006ad4d0);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTINPUTDEVICE, gDirectInputDevice, 0x006acea0);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gRenderer, 0x006621dc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMouseLButtonDown, 0x006ad488);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMouseRButtonDown, 0x006ad48c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMouseCaptured, 0x006ad490);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(POINT, gCursorPos_LastClick, 0x006621d4, {-1,-1});
C2_HOOK_VARIABLE_IMPLEMENT(POINT, gCurrentCursorPos, 0x006ad458);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCursorPos_LastClick_Valid, 0x006ad454);
C2_HOOK_VARIABLE_IMPLEMENT(int, gScaleMouse, 0x006ad468);
C2_HOOK_VARIABLE_IMPLEMENT(int, gScreenWidth, 0x006ad460);
C2_HOOK_VARIABLE_IMPLEMENT(int, gScreenHeight, 0x006ad464);

C2_HOOK_VARIABLE_IMPLEMENT(int, gKeyboardBufferLength, 0x006ad4b0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gKeyboardBuffer, 20, 0x006ace88);

C2_HOOK_VARIABLE_IMPLEMENT(int, gPerformanceCounterInitialized, 0x006acc70);
C2_HOOK_VARIABLE_IMPLEMENT(LARGE_INTEGER, gPerformanceCounterStart, 0x006acc80);
C2_HOOK_VARIABLE_IMPLEMENT(LARGE_INTEGER, gPerformanceCounterFrequency_kHz, 0x006ad1f0);


C2_HOOK_VARIABLE_IMPLEMENT(int, gTimeLastKeyboardInput, 0x006ad49c);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gFatalErrorMessage, 512, 0x006acc88);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFatalErrorMessageValid, 0x006ad498);

void C2_HOOK_FASTCALL KeyBegin(void) {
    C2V(gReal_back_screen) = NULL;
    C2V(gScreen) = NULL;
    ShowCursor(FALSE);
    KeyScanCodeBegin();
    KeyDInputBegin();
}
C2_HOOK_FUNCTION(0x0051b9f0, KeyBegin)

void C2_HOOK_FASTCALL PDSetFileVariables() {
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

void (C2_HOOK_FASTCALL * PDFatalError_original)(char* pThe_str);
void C2_HOOK_FASTCALL PDFatalError(char* pThe_str) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    PDFatalError_original(pThe_str);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL
(0x0051af20, PDFatalError, PDFatalError_original)

static void(C2_HOOK_FASTCALL * PDShutdownSystem_original)(void);
void C2_HOOK_FASTCALL PDShutdownSystem() {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    PDShutdownSystem_original();
    C2_HOOK_FINISH();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051c110, PDShutdownSystem, PDShutdownSystem_original)


static void(C2_HOOK_FASTCALL * PDUnlockRealBackScreen_original)(void);
void C2_HOOK_FASTCALL PDUnlockRealBackScreen() {
#if defined(C2_HOOKS_ENABLED)
    PDUnlockRealBackScreen_original();
#else
    if (C2V(gReal_back_screen)->pixels != NULL) {
        BrPixelmapDirectUnlock(gReal_back_screen);
        return 1;
    }
    return 0;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00516c30, PDUnlockRealBackScreen, PDUnlockRealBackScreen_original)


void DeActivateApp(void) {
    dr_dprintf("DeActivateApp() - START");
    if (!C2V(gWindowMovingResizing) && C2V(gWindowActiveState) == 2) {
        dr_dprintf("DeActivateApp() - deactivating app");
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
    dr_dprintf("DeActivateApp() - END; active state now %d", C2V(gWindowActiveState));
}

WNDPROC Carma2MainWndProc_original;
LRESULT CALLBACK Carma2MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
#if 0//defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    LRESULT res = Carma2MainWndProc_original(hWnd, uMsg, wParam, lParam);
    C2_HOOK_FINISH();
    return res;
#else
    PAINTSTRUCT paint;
    POINT point;
    char buffer[256];

    C2_HOOK_START();
    switch (uMsg) {
    case WM_PAINT:
        BeginPaint(hWnd, &paint);
        EndPaint(hWnd, &paint);
        return 0;
    case WM_DESTROY:
        dr_dprintf("WM_DESTROY received - doing nothing.");
        break;
    case WM_MOVE:
    case WM_SIZE:
        if (IsIconic(hWnd)) {
            dr_dprintf("WM_SIZE/WM_MOVE: Window is iconic");
            DeActivateApp();
        }
        break;
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
            C2V(gCursorPos_LastClick) = C2V(gCurrentCursorPos);
            C2V(gCursorPos_LastClick_Valid) = 1;
            break;
        }
        GetCursorPos(&point);
        if (!C2V(gScaleMouse) || strcmp(C2V(gRenderer), "D3D") == 0) {
            ScreenToClient(C2V(gHWnd), &point);
            C2V(gCurrentCursorPos) = C2V(gCursorPos_LastClick) = point;
        } else {
            C2V(gCurrentCursorPos).x = C2V(gCursorPos_LastClick).x = (int)((double)point.x / (C2V(gScreenWidth) / 640.));
            C2V(gCurrentCursorPos).y = C2V(gCursorPos_LastClick).y = (int)((double)point.y / (C2V(gScreenHeight) / 480.));
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
            dr_dprintf("KEY PRESSED, BUFFER NOW IS: '%s'", buffer);
        }
        break;
    case WM_ACTIVATEAPP:
        dr_dprintf("WM_ACTIVATEAPP: wparam is %d, lparam is %d, fg window is %p, main win is %p, hWnd is %p, isiconic is %d",
                   wParam, lParam, GetForegroundWindow(), C2V(gHWnd), hWnd, IsIconic(C2V(gHWnd)));
        if (C2V(gHWnd) != NULL) {
            if (GetForegroundWindow() == C2V(gHWnd) && !IsIconic(C2V(gHWnd))) {
                dr_dprintf("Activating app");
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
                    C2V(gTimeLastKeyboardInput) = (int)((li.QuadPart - C2V(gPerformanceCounterStart).QuadPart) / C2V(gPerformanceCounterFrequency_kHz).QuadPart);
                }
            } else {
                if (C2V(gReal_back_screen) != NULL && C2V(gReal_back_screen)->pixels != NULL) {
                    return 1;
                }
                DeActivateApp();
            }
        }
        break;
    default:
        break;
    }
    LRESULT res = DefWindowProcA(hWnd, uMsg, wParam, lParam);
    C2_HOOK_FINISH();
    return res;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051b0c0, Carma2MainWndProc, Carma2MainWndProc_original)

void (C2_HOOK_CDECL * Win32ServiceMessages_original)(void);
void C2_HOOK_CDECL Win32ServiceMessages(void) {
    C2_HOOK_START();
#if 0//defined(C2_HOOKS_ENABLED)
    Win32ServiceMessages_original();
#else
    MSG msg;
    dr_dprintf("Win32ServiceMessages() - START");
    while (1) {
        if (C2V(gWindowActiveState) == 1) {
            SetForegroundWindow(C2V(gHWnd));
        }

        if (C2V(gWindowActiveState) == 0) {
            if (GetMessageA(&msg, NULL, 0, 0) == -1) {
                dr_dprintf("Win32ServiceMessages() - breaking cos GetMessage() returned -1");
                break;
            }
        } else {
            if (PeekMessageA(&msg, NULL, 0, 0, 1) == 0) {
                dr_dprintf("Win32ServiceMessages() - breaking cos PeekMessage() returned 0");
                break;
            }
            if (C2V(gWindowActiveState) == 0) {
                if (GetMessageA(&msg, NULL, 0, 0) == -1) {
                    dr_dprintf("Win32ServiceMessages() - breaking cos GetMessage() returned -1");
                    break;
                }
            }
        }
        if (msg.message == WM_QUIT) {
            dr_dprintf("WM_QUIT received.");
            if (C2V(gWindowActiveState) == 2) {
                dr_dprintf("Active, so lock the surface");
                dr_dprintf("QuitGame being called...");
                QuitGame();
            }
            PDShutdownSystem();
        }
        TranslateMessage(&msg);
        dr_dprintf("Win32ServiceMessages() - dispatching message...");
        DispatchMessageA(&msg);
    }
    dr_dprintf("Win32ServiceMessages() - END");
#endif
    C2_HOOK_FINISH();
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051cad0, Win32ServiceMessages, Win32ServiceMessages_original)

int (C2_HOOK_FASTCALL * PDCheckDriveExists2_original)(const char* pThe_path, const char* pFile_name, tU32 pMin_size);
int C2_HOOK_FASTCALL PDCheckDriveExists2(const char* pThe_path, const char* pFile_name, tU32 pMin_size) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    int res = PDCheckDriveExists2_original(pThe_path, pFile_name, pMin_size);
    C2_HOOK_FINISH();
    return res;
#else
#error "Not implemented"
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
        lenBuffer = sizeof(pPath) - 1;  // FIXME: use array_sizeof
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
