#include "spark.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSmoke_on, 0x00660110, 1);

void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on) {

    C2V(gSmoke_on) = pSmoke_on;
}
C2_HOOK_FUNCTION(0x004fc9d0, SetSmokeOn)

int C2_HOOK_FASTCALL GetSmokeOn(void) {

    return C2V(gSmoke_on);
}
C2_HOOK_FUNCTION(0x004fca30, GetSmokeOn)
