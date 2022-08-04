#ifndef REC2_GRAFDATA_H
#define REC2_GRAFDATA_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tGraf_data, gGraf_data, 2);
C2_HOOK_VARIABLE_DECLARE(tGraf_data*, gCurrent_graf_data);
C2_HOOK_VARIABLE_DECLARE(int, gReal_graf_data_index);
C2_HOOK_VARIABLE_DECLARE(int, gGraf_data_index);
C2_HOOK_VARIABLE_DECLARE(float, gMap_render_x);
C2_HOOK_VARIABLE_DECLARE(float, gMap_render_y);
C2_HOOK_VARIABLE_DECLARE(float, gMap_render_width);
C2_HOOK_VARIABLE_DECLARE(float, gMap_render_height);

void C2_HOOK_FASTCALL CalcGrafDataIndex(void);

#endif // REC2_GRAFDATA_H
