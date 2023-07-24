#include "win32net.h"

#include "win32.h"

#include "errors.h"

#include "c2_string.h"

void C2_HOOK_FASTCALL PDNetObtainSystemUserName(char* pName, int pMax_length) {
    char buffer[16];
    DWORD size;
    char *p;

    dr_dprintf("PDNetObtainSystemUserName()");

    if (GetComputerNameA(buffer, &size) && size != 0) {
        c2_strncpy(pName, buffer, pMax_length - 1);
        pName[pMax_length - 1] = '\0';
        while ((p = c2_strpbrk(pName, "_=(){}[]<>!$%^&*/:@~;'#,?\\|`\"")) != NULL) {
            *p = '-';
        }
    }
}
C2_HOOK_FUNCTION(0x0051a190, PDNetObtainSystemUserName)

int (C2_HOOK_FASTCALL * PDNetShutdown_original)(void);
int C2_HOOK_FASTCALL PDNetShutdown(void) {

#if defined(C2_HOOKS_ENABLED)
    return PDNetShutdown_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00519a10, PDNetShutdown, PDNetShutdown_original)
