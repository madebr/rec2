#include "funk.h"


void (C2_HOOK_FASTCALL * DoCamouflageThing_original)(tCar_spec* pCar);
void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar) {

#if defined(C2_HOOKS_ENABLED)
    DoCamouflageThing_original(pCar);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f8f30, DoCamouflageThing, DoCamouflageThing_original)
