#ifndef FRONTEND_CHANGECAR_H
#define FRONTEND_CHANGECAR_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_CHANGE_CAR);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_change_car_selected_car);


int C2_HOOK_FASTCALL ChangeCarMenuInfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL ChangeCarMenuOutfunc(tFrontend_spec* pFrontend);

#endif /* FRONTEND_CHANGECAR_H */
