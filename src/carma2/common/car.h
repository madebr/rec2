#ifndef REC2_CAR_H
#define REC2_CAR_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gCar_simplification_level);

void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c);

#endif //REC2_CAR_H
