/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef FRONTEND_NETWORK_OPTIONS_H
#define FRONTEND_NETWORK_OPTIONS_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tFrontend_spec gFrontend_NETWORK_OPTIONS;
extern tNet_game_options gFrontend_backup_net_options;
extern tStruct_00686508 gUNK_006886e8;
extern tStruct_00686508 gUNK_006883b0;


void C2_HOOK_FASTCALL DisplayNetworkOptions(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL BackupNetworkOptions(void);

int C2_HOOK_FASTCALL NetOptions_Infunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetOptions_Outfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetOptions_CreditsRoller(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetOptions_TargetRoller(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetOptions_Ok(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL RestoreNetworkOptions(void);

int C2_HOOK_FASTCALL NetOptions_Cancel(tFrontend_spec* pFrontend);

#endif /* FRONTEND_NETWORK_OPTIONS_H */