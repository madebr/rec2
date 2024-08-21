#ifndef C2_WIN32_WIN32_H
#define C2_WIN32_WIN32_H

#include "c2_hooks.h"

#include "rec2_types.h"

#include <brender/brender.h>

#include <windows.h>
#include <dinput.h>

C2_HOOK_VARIABLE_DECLARE(int, gWin32_fatal_error_exit_code);

C2_HOOK_VARIABLE_DECLARE(HWND, gHWnd);
C2_HOOK_VARIABLE_DECLARE(int, gWindowActiveState);  // FIXME: enum (0,1) => background(d3d or glide), 2=> active

C2_HOOK_VARIABLE_DECLARE(int, gWindowMovingResizing);
C2_HOOK_VARIABLE_DECLARE(LPDIRECTINPUTDEVICEA, gDirectInputDevice);
C2_HOOK_VARIABLE_DECLARE(char*, gRenderer);
C2_HOOK_VARIABLE_DECLARE(int, gMouseLButtonDown);
C2_HOOK_VARIABLE_DECLARE(int, gMouseRButtonDown);
C2_HOOK_VARIABLE_DECLARE(int, gMouseCaptured);
C2_HOOK_VARIABLE_DECLARE(POINT, gCursorPos_LastClick);
C2_HOOK_VARIABLE_DECLARE(POINT, gPD_mouse_position);
C2_HOOK_VARIABLE_DECLARE(int, gCursorPos_LastClick_Valid);
C2_HOOK_VARIABLE_DECLARE(int, gScaleMouse);
C2_HOOK_VARIABLE_DECLARE(int, gScreenWidth);
C2_HOOK_VARIABLE_DECLARE(int, gScreenHeight);

C2_HOOK_VARIABLE_DECLARE(int, gKeyboardBufferLength);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gKeyboardBuffer, 20);

C2_HOOK_VARIABLE_DECLARE(int, gPerformanceCounterInitialized);
C2_HOOK_VARIABLE_DECLARE(LARGE_INTEGER, gPerformanceCounterStart);
C2_HOOK_VARIABLE_DECLARE(LARGE_INTEGER, gPerformanceCounterFrequency_s);
C2_HOOK_VARIABLE_DECLARE(LARGE_INTEGER, gPerformanceCounterFrequency_ms);
C2_HOOK_VARIABLE_DECLARE(LARGE_INTEGER, gPerformanceCounterFrequency_us);

C2_HOOK_VARIABLE_DECLARE(int, gTimeLastKeyboardInput);

C2_HOOK_VARIABLE_DECLARE(HINSTANCE, gHInstance);

C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gFatalErrorMessage, 512);
C2_HOOK_VARIABLE_DECLARE(int, gFatalErrorMessageValid);

LRESULT CALLBACK Carma2MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int C2_HOOK_FASTCALL PDFileUnlock(char* pThe_path);

int C2_HOOK_CDECL IsNetworkShare(const char* path);

int C2_HOOK_CDECL IsValidDriveIndex(int driveIndex);

void C2_HOOK_FASTCALL PDRevertPalette(void);

void C2_HOOK_FASTCALL PDMouseButtons(int* pLeftButtonDown, int* pRightButtonDown);

void C2_HOOK_FASTCALL PDExtractFilename(char* pDest, const char* pPath);

void C2_HOOK_FASTCALL PDExtractDirectory(char* pDest, const char* pPath);

void C2_HOOK_CDECL Win32ServiceMessages(void);

#endif // C2_WIN32_WIN32_H
