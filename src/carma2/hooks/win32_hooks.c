#include "c2_hooks.h"

#include <windows.h>

HWND (WINAPI *CreateWindowExA_original)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
                          int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                          HINSTANCE hInstance, LPVOID lpParam);
HWND WINAPI CreateWindowExA_hook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
                                            int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                            HINSTANCE hInstance, LPVOID lpParam)
{
    return CreateWindowExA_original(dwExStyle, lpClassName, lpWindowName, dwStyle & ~WS_POPUP,
//                                    X, Y, nWidth, nHeight, hWndParent, hMenu,
                                    CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, hWndParent, hMenu,
                                    hInstance, lpParam);
}
C2_HOOK_FUNCTION_ORIGINAL(CreateWindowExA, CreateWindowExA_hook, CreateWindowExA_original)
