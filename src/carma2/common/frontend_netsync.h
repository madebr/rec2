#ifndef FRONTEND_NETSYNC_H
#define FRONTEND_NETSYNC_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_NETSYNC);
C2_HOOK_VARIABLE_DECLARE(tU32, gNet_synch_start);


int C2_HOOK_FASTCALL NetSynchRaceStart2(tNet_synch_mode pMode);

int C2_HOOK_FASTCALL NetSync_Infunc(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL MungePlayers(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL NetSync_Draw(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetSync_MenuHandler(tFrontend_spec* pFrontend);

#endif /* FRONTEND_NETSYNC_H */
