#include "hooks.h"

#include <windows.h>
#include <detours.h>

#include <stdio.h>

#ifndef CONSOLE_TITLE
#define CONSOLE_TITLE "console"
#endif

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
    if (DetourIsHelperProcess()) {
        return TRUE;
    }

    if (dwReason == DLL_PROCESS_ATTACH) {
        AllocConsole();

        SetConsoleTitle(TEXT(CONSOLE_TITLE));
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);

        DetourRestoreAfterWith();

        DetourTransactionBegin();

        DetourUpdateThread (GetCurrentThread());
        hook_apply_all();

        DetourTransactionCommit();

        hook_print_stats();
    } else if (dwReason == DLL_PROCESS_DETACH) {
        DetourTransactionBegin();

        hook_unapply_all();
        DetourUpdateThread (GetCurrentThread());

        DetourTransactionCommit();
    }
    return TRUE;
}
