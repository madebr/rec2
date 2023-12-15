#include "skidmark.h"

void (C2_HOOK_FASTCALL * InitSkids_original)(void);
void C2_HOOK_FASTCALL InitSkids(void) {

#if defined(C2_HOOKS_ENABLED)
    InitSkids_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e9c40, InitSkids, InitSkids_original)

void (C2_HOOK_FASTCALL * ReadSlick_original)(FILE* pF,tSlick_spec* pSlick_spec);
void C2_HOOK_FASTCALL ReadSlick(FILE* pF, tSlick_spec* pSlick_spec) {

#if defined(C2_HOOKS_ENABLED)
    ReadSlick_original(pF, pSlick_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ee5e0, ReadSlick, ReadSlick_original)
