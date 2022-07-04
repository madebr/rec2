#include "c2_hooks.h"

#include <windows.h>

HWND (WINAPI *CreateWindowExA_original)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
                          int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                          HINSTANCE hInstance, LPVOID lpParam);
HWND WINAPI CreateWindowExA_hook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
                                            int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                            HINSTANCE hInstance, LPVOID lpParam)
{
#if 1
return CreateWindowExA_original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                    X, Y, nWidth, nHeight, hWndParent, hMenu,
                                    hInstance, lpParam);
#else
    return CreateWindowExA_original(dwExStyle, lpClassName, lpWindowName, (dwStyle & ~WS_POPUP),
                                    CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, hWndParent, hMenu,
                                    hInstance, lpParam);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(CreateWindowExA, CreateWindowExA_hook, CreateWindowExA_original)


BOOL (WINAPI * SetWindowPos_original)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL WINAPI SetWindowPos_hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    C2_HOOK_DEBUGF("x=%d y=%d cx=%d cy=%d flags=0x%x", X, Y, cx, cy, uFlags);
    return SetWindowPos_original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
//    return 1;
}
C2_HOOK_FUNCTION_ORIGINAL(SetWindowPos, SetWindowPos_hook, SetWindowPos_original)


HMODULE (WINAPI * LoadLibraryA_original)(LPCSTR lpLibFileName);
HMODULE WINAPI LoadLibraryA_hook(LPCSTR lpLibFileName) {
    C2_HOOK_START();
    HMODULE hRes = LoadLibraryA_original(lpLibFileName);
    C2_HOOK_DEBUGF("%s -> %p", lpLibFileName, hRes);
    C2_HOOK_FINISH();
    return hRes;
}
C2_HOOK_FUNCTION_ORIGINAL(LoadLibraryA, LoadLibraryA_hook, LoadLibraryA_original)
