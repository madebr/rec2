#include "hooks.h"

#include <windows.h>
#include <detours.h>

#include <stdio.h>

#ifndef CONSOLE_TITLE
#define CONSOLE_TITLE "console"
#endif

#if defined(HOOK_INIT_FUNCTION)
void HOOK_INIT_FUNCTION(void);
#endif
#if defined(HOOK_DEINIT_FUNCTION)
void HOOK_DEINIT_FUNCTION(void);
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

        hook_run_functions();

        hook_print_stats();
        hook_check();
        char pathBuffer[512];
        GetModuleFileNameA(NULL, pathBuffer, sizeof(pathBuffer));
        printf("executable: \"%s\"\n", pathBuffer);
        GetCurrentDirectoryA(sizeof(pathBuffer), pathBuffer);
        printf("directory: \"%s\"\n", pathBuffer);
        printf("=================================\n");

#if defined(HOOK_INIT_FUNCTION)
        HOOK_INIT_FUNCTION();
#endif
    } else if (dwReason == DLL_PROCESS_DETACH) {
#if defined(HOOK_DEINIT_FUNCTION)
        HOOK_DEINIT_FUNCTION();
#endif

        DetourTransactionBegin();

        hook_unapply_all();
        DetourUpdateThread (GetCurrentThread());

        DetourTransactionCommit();
    }
    return TRUE;
}
