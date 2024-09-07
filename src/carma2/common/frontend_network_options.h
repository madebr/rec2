#ifndef FRONTEND_NETWORK_OPTIONS_H
#define FRONTEND_NETWORK_OPTIONS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_NETWORK_OPTIONS);
C2_HOOK_VARIABLE_DECLARE(tNet_game_options, gFrontend_backup_net_options);
C2_HOOK_VARIABLE_DECLARE(tStruct_00686508, gUNK_006886e8);
C2_HOOK_VARIABLE_DECLARE(tStruct_00686508, gUNK_006883b0);


void C2_HOOK_FASTCALL DisplayNetworkOptions(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL BackupNetworkOptions(void);

int C2_HOOK_FASTCALL NetOptions_InFunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetOptions_Outfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetOptions_CreditsRoller(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetOptions_TargetRoller(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetOptions_Ok(tFrontend_spec* pFrontend);

#endif /* FRONTEND_NETWORK_OPTIONS_H */
