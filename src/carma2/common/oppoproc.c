#include "oppoproc.h"

void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...) {
}
C2_HOOK_FUNCTION(0x004b13c0, DoNotDprintf_oppoproc)
