#include "physics.h"

void (C2_HOOK_FASTCALL * InitPhysics_original)(void);
void C2_HOOK_FASTCALL InitPhysics(void) {

    C2_HOOK_ASSUME_UNUSED();
#if defined(C2_HOOKS_ENABLED)
    InitPhysics_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5ca0, InitPhysics, InitPhysics_original)

void (C2_HOOK_FASTCALL * LoadCollisionShape_original)(tCollision_shape** pShape, FILE* pF);
void C2_HOOK_FASTCALL LoadCollisionShape(tCollision_shape** pShape, FILE* pF) {

#if defined(C2_HOOKS_ENABLED)
    LoadCollisionShape_original(pShape, pF);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00488b00, LoadCollisionShape, LoadCollisionShape_original)
