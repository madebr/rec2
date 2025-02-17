#if _WIN32
#include <windows.h>
#endif

#ifdef _MSC_VER
#pragma runtime_checks("scu", off)
#endif

#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    (void)hinstDLL;

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (lpvReserved != NULL) {
        break;
        }
        break;
    }
    return TRUE;
}
#else
void entry(void) __attribute__((constructor));
void entry(void) {

}
#endif
