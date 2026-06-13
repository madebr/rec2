#ifndef GUARD_25_GRAFDATA_H
#define GUARD_25_GRAFDATA_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gGraf_data_index;
extern tGraf_data* gCurrent_graf_data;

extern void C2_HOOK_FASTCALL CalcGrafDataIndex(void);

#endif // GUARD_25_GRAFDATA_H
