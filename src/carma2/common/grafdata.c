#include "grafdata.h"

#include "errors.h"

#include "platform.h"

#include "rec2_macros.h"
#include "rec2_types.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tGraf_data, gGraf_data, 2, 0x00655f58); // FIXME: implement
C2_HOOK_VARIABLE_IMPLEMENT(tGraf_data*, gCurrent_graf_data, 0x0074b774);
C2_HOOK_VARIABLE_IMPLEMENT(int, gReal_graf_data_index, 0x0074b770);
C2_HOOK_VARIABLE_IMPLEMENT(int, gGraf_data_index, 0x0074d630);

void C2_HOOK_FASTCALL CalcGrafDataIndex(void) {
    int i;

    REC2_BUG_ON(sizeof(tGraf_data) != 1312);
    REC2_BUG_ON(sizeof(tGraf_spec) != 52);

    for (i = 0; i < REC2_ASIZE(C2V(gGraf_data)); i++) {
        if (C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width == C2V(gGraf_data)[i].width && C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height == C2V(gGraf_data)[i].height) {
            C2V(gReal_graf_data_index) = i;
            C2V(gGraf_data_index) = i;
            C2V(gCurrent_graf_data) = &C2V(gGraf_data)[i];
            return;
        }
    }
    FatalError(kFatalError_ScreenWidthHeightDoesNotMatchGrafData);
}
C2_HOOK_FUNCTION(0x0047b3e0, CalcGrafDataIndex)
