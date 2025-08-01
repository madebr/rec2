#include "flap.h"


void (C2_HOOK_FASTCALL * DoDetaching_original)(void);
void C2_HOOK_FASTCALL DoDetaching(void) {

#if defined(C2_HOOKS_ENABLED)
    DoDetaching_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00436ad0, DoDetaching, DoDetaching_original)

void (C2_HOOK_FASTCALL * SetBitForDetachment_original)(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4);
void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4) {

#if defined(C2_HOOKS_ENABLED)
    SetBitForDetachment(pActor, pCar, pArg3, pArg4);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042d7e0, SetBitForDetachment, SetBitForDetachment_original)

void (C2_HOOK_FASTCALL * DoFlapping_original)(void);
void C2_HOOK_FASTCALL DoFlapping(void) {

#if defined(C2_HOOKS_ENABLED)
    DoFlapping_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004381b0, DoFlapping, DoFlapping_original)

void (C2_HOOK_FASTCALL * DoFullyDetaching_original)(void);
void C2_HOOK_FASTCALL DoFullyDetaching(void) {

#if defined(C2_HOOKS_ENABLED)
    DoFullyDetaching_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00438900, DoFullyDetaching, DoFullyDetaching_original)

void (C2_HOOK_FASTCALL * DoBending_original)(void);
void C2_HOOK_FASTCALL DoBending(void) {

#if defined(C2_HOOKS_ENABLED)
    DoBending_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00438a90, DoBending, DoBending_original)
